#include "godot_gdk.h"
#include "gdk_user.h"

#include "godot_cpp/core/class_db.hpp"

#include <Windows.h>
#include <winapifamily.h>
#include <objbase.h>
#include <XGameRuntimeInit.h>
#include <XTaskQueue.h>
#include <xsapi-c/services_c.h>
#include <XUser.h>
#include <XGame.h>
#include <XGameInvite.h>
#include <XPackage.h>
#include "gdk_package.h"
#include "gdk_system.h"

#include <iomanip>
#include <sstream>
#include <string>
#include "gdk_networking.h"

static XTaskQueueHandle queue;
static XUserLocalId xboxUserId;
static XUserHandle xboxUserHandle;
static const char* SCID;

static void OnGameInviteReceived(void* context, const char* inviteUri);

using namespace godot;

GodotGDK* GodotGDK::_instance = nullptr;

void GDKXUserChangeEvent::_bind_methods() {
	BIND_ENUM_CONSTANT(SignedInAgain);
	BIND_ENUM_CONSTANT(SigningOut);
	BIND_ENUM_CONSTANT(SignedOut);
	BIND_ENUM_CONSTANT(Gamertag);
	BIND_ENUM_CONSTANT(GamerPicture);
	BIND_ENUM_CONSTANT(Privileges);
}

void GodotGDK::_bind_methods() {
	ClassDB::bind_method(D_METHOD("InitializeGDK", "callback", "SCID"), &GodotGDK::InitializeGDK);

	ClassDB::bind_method(D_METHOD("get_xbox_title_id"), &GodotGDK::get_xbox_title_id);
	ClassDB::bind_method(D_METHOD("launch_new_game", "exe_path", "args", "default_user"), &GodotGDK::launch_new_game);
	ClassDB::bind_method(D_METHOD("launch_restart_on_crash", "args"), &GodotGDK::launch_restart_on_crash);

	ADD_SIGNAL(MethodInfo("game_invite_received", PropertyInfo(Variant::STRING, "invite_uri")));
	ADD_SIGNAL(MethodInfo("user_changed", 
				PropertyInfo(Variant::OBJECT, "user", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "GDKUser"),
				PropertyInfo(Variant::INT, "change_event", PROPERTY_HINT_ENUM, "SignedInAgain,SigningOut,SignedOut,Gamertag,GamerPicture,Privileges", PROPERTY_USAGE_DEFAULT, "GDKXUserChangeEvent::Enum")));
	ADD_SIGNAL(MethodInfo("device_association_changed", 
				PropertyInfo(Variant::PACKED_BYTE_ARRAY, "device_id"),
				PropertyInfo(Variant::OBJECT, "old_user", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "GDKUser"),
				PropertyInfo(Variant::OBJECT, "new_user", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "GDKUser")));
	ADD_SIGNAL(MethodInfo("default_audio_endpoint_utf16_changed",
				PropertyInfo(Variant::OBJECT, "user", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "GDKUser"),
				PropertyInfo(Variant::INT, "endpoint_kind", PROPERTY_HINT_ENUM, "CommunicationRender,CommunicationCapture", PROPERTY_USAGE_DEFAULT, "GDKXUserDefaultAudioEndpointKind::Enum"),
				PropertyInfo(Variant::STRING, "endpoint_id")));
	ADD_SIGNAL(MethodInfo("package_installed",
				PropertyInfo(Variant::DICTIONARY, "package_details")));
}

void GodotGDK::_notification(int p_what) {
	if (p_what == NOTIFICATION_PREDELETE) {
		if (_initialized) {
			if (_invite_registered) {
				XGameInviteUnregisterForEvent(_invite_token, false);
				_invite_registered = false;
			}

			_event_objects.clear();
			XUserUnregisterForChangeEvent(_user_change_token, false);
			XUserUnregisterForDeviceAssociationChanged(_device_association_change_token, false);
			XUserUnregisterForDefaultAudioEndpointUtf16Changed(_default_audio_endpoint_change_token, false);
			XPackageUnregisterPackageInstalled(_package_installed_token, false);
			XTaskQueueCloseHandle(queue);
			XGameRuntimeUninitialize();
		}
	}
}

GodotGDK *GodotGDK::get_singleton() {
	if (_instance) {
		return _instance;
	}

	_instance = memnew(GodotGDK);
	return _instance;
}

void UserChangeEventCallback(void* context, XUserLocalId userLocalId, XUserChangeEvent event) {
	XUserHandle userHandle;

	HRESULT hr = XUserFindUserByLocalId(userLocalId, &userHandle);
	ERR_FAIL_COND_MSG(FAILED(hr), vformat("UserChangeEvent Error: 0x%08ux", (uint64_t)hr));

	Ref<GDKUser> user = GDKUser::create(userHandle);
	GDKXUserChangeEvent::Enum event_enum = static_cast<GDKXUserChangeEvent::Enum>(event);

	GodotGDK* self = static_cast<GodotGDK*>(context);
	self->call_deferred("emit_signal", "user_changed", user, event_enum);
}

void DeviceAssociationChangeCallback(void* context, const XUserDeviceAssociationChange* change) {
	PackedByteArray device_id;
	device_id.resize(sizeof(APP_LOCAL_DEVICE_ID));
	memcpy(device_id.ptrw(), &change->deviceId, sizeof(APP_LOCAL_DEVICE_ID));

	HRESULT hr = S_OK;
	Ref<GDKUser> oldUser = nullptr;
	if (change->oldUser.value != 0) {
		XUserHandle oldUserHandle;
		hr = XUserFindUserByLocalId(change->oldUser, &oldUserHandle);
		ERR_FAIL_COND_MSG(FAILED(hr) && change->oldUser.value != 0, vformat("DeviceAssociationChange FindUserByLocalId(OldUser) Error: 0x%08ux", (uint64_t)hr));
		oldUser = GDKUser::create(oldUserHandle);
	}

	Ref<GDKUser> newUser = nullptr;
	if (change->newUser.value != 0) {
		XUserHandle newUserHandle;
		hr = XUserFindUserByLocalId(change->newUser, &newUserHandle);
		ERR_FAIL_COND_MSG(FAILED(hr) && change->newUser.value != 0, vformat("DeviceAssociationChange FindUserByLocalId(NewUser) Error: 0x%08ux", (uint64_t)hr));
		newUser = GDKUser::create(newUserHandle);
	}

	GodotGDK* self = static_cast<GodotGDK*>(context);
	self->call_deferred("emit_signal", "device_association_changed", device_id, oldUser, newUser);
}

void DefaultAudioEndpointUtf16ChangeCallback(void* context, XUserLocalId userLocalId, XUserDefaultAudioEndpointKind endpointKind, const wchar_t* endpointIdUtf16)
{
	XUserHandle userHandle;
	HRESULT hr = XUserFindUserByLocalId(userLocalId, &userHandle);
	ERR_FAIL_COND_MSG(FAILED(hr), vformat("DefaultAudioEndpointUtf16ChangeCallback FindUserByLocalId Error: 0x%08ux", (uint64_t)hr));

	GDKXUserDefaultAudioEndpointKind::Enum endpoint_enum = static_cast<GDKXUserDefaultAudioEndpointKind::Enum>(endpointKind);
	String endpoint_id = String(endpointIdUtf16);

	GodotGDK* self = static_cast<GodotGDK*>(context);
	self->call_deferred("emit_signal", "default_audio_endpoint_utf16_changed", GDKUser::create(userHandle), endpoint_enum, endpoint_id);
}

void PackageInstalledCallback(void* context, const XPackageDetails* details) {
	Dictionary package_details;
	package_details["package_identifier"] = String(details->packageIdentifier);
	package_details["version"] = GDKXVersion::create(&details->version);
	package_details["kind"] = (GDKXPackageKind::Enum)details->kind;
	package_details["display_name"] = String(details->displayName);
	package_details["description"] = String(details->description);
	package_details["publisher"] = String(details->publisher);
	package_details["store_id"] = String(details->storeId);
	package_details["installing"] = details->installing;
	package_details["index"] = details->index;
	package_details["count"] = details->count;
	package_details["age_restricted"] = details->ageRestricted;
	package_details["title_id"] = String(details->titleId);
	
	
	GodotGDK* self = static_cast<GodotGDK*>(context);
	self->call_deferred("emit_signal", "package_installed", details);
}

int GodotGDK::InitializeGDK(Callable cb, String scid) {

	HRESULT hr = XGameRuntimeInitialize();
	if (!CheckResult(hr,"Successfully initialized GDK", "Failed to initialize Xbox Game Runtime (GDK).")) {
		return hr;
	}

	XblInitArgs xblArgs = {};
	xblArgs.queue = queue;

	SCID = GodotGDK::CopyStringToChar(scid);
	xblArgs.scid = SCID;

	hr = XblInitialize(&xblArgs);
	if (!CheckResult(hr, "Successfully initialized xbox services", "Failed to initialize Xbox services"))
	{
		return hr;
	}

	queue = XblGetAsyncQueue();

	hr = XGameInviteRegisterForEvent(queue, this, &OnGameInviteReceived, &_invite_token);
	if (CheckResult(hr, "Game invite event registered", "Failed to register game invite event")) {
		_invite_registered = true;
	}

	hr = XUserRegisterForChangeEvent(queue, this, &UserChangeEventCallback, &_user_change_token);
	CheckResult(hr, "User change event registered", "Failed to register user change event");

	hr = XUserRegisterForDeviceAssociationChanged(queue, this, &DeviceAssociationChangeCallback, &_device_association_change_token);
	CheckResult(hr, "Device association change event registered", "Failed to register device association change event");

	hr = XUserRegisterForDefaultAudioEndpointUtf16Changed(queue, this, &DefaultAudioEndpointUtf16ChangeCallback, &_default_audio_endpoint_change_token);
	CheckResult(hr, "Default audio endpoint change event registered", "Failed to register default audio endpoint change event");

	hr = XPackageRegisterPackageInstalled(queue, this, &PackageInstalledCallback, &_package_installed_token);
	CheckResult(hr, "Package installed event registered", "Failed to register package installed event");
	
	for (int i = 0; i < _event_objects.size(); i++) {
		Ref<GDKEventObject> obj = _event_objects[i];
		obj->initialize();
	}

	hr = Identity_TrySignInDefaultUserSilently(queue, cb);
	CheckResult(hr, "Login successfully started", "Failed to start login");

	return hr;
}


HRESULT GodotGDK::Identity_TrySignInDefaultUserSilently(_In_ XTaskQueueHandle asyncQueue, Callable cb) {
	XAsyncBlock* asyncBlock = new XAsyncBlock();
	asyncBlock->queue = asyncQueue;
	Callable* cb_ptr = new Callable(cb);
	asyncBlock->context = cb_ptr;
	asyncBlock->callback = Identity_TrySignInDefaultUserSilently_Callback;

	// Request to silently sign in the default user.
	HRESULT hr = XUserAddAsync(XUserAddOptions::AddDefaultUserSilently, asyncBlock);

	if (!CheckResult(hr, "Successfully started login", "Failed to start login"))
	{
		delete asyncBlock;
	}

	return hr;
}

void CALLBACK GodotGDK::Identity_TrySignInDefaultUserSilently_Callback(_In_ XAsyncBlock* asyncBlock)
{
	HRESULT hr = XUserAddResult(asyncBlock, &xboxUserHandle);

	if (!CheckResult(hr, "Login successful", "Login failed")) {
		delete asyncBlock;
		return;
	}

	XblContextHandle handle;

	if (FAILED(XblContextCreateHandle(GodotGDK::GetUserHandle(), &handle))) {
		delete asyncBlock;
		return;
	}

	hr = XUserGetLocalId(xboxUserHandle, &xboxUserId);

	if (!CheckResult(hr, "Successfully retrieved userid", "Failed to retrieve userid")) {
		delete asyncBlock;
		return;
	}

	Callable* callback = static_cast<Callable*>(asyncBlock->context);

	if (callback && callback->is_valid()) {
		callback->call_deferred();
	}

	delete callback;
	delete asyncBlock;
}

XTaskQueueHandle GodotGDK::GetQueueHandle() {
	return queue;
}

XUserLocalId  GodotGDK::GetUserId() {
	return xboxUserId;
}

XUserHandle GodotGDK::GetUserHandle() {
	return xboxUserHandle;
}

XAsyncBlock* GodotGDK::CreateAsyncBlock() {
	XAsyncBlock* asyncBlock = new XAsyncBlock();
	XTaskQueueHandle queue_handle = GodotGDK::GetQueueHandle();
	asyncBlock->queue = queue_handle;
	return asyncBlock;
}

const char* GodotGDK::GetSCID() {
	return SCID;
}

bool GodotGDK::CheckResult(HRESULT result, const String& succeedMessage, const String& errorMessage) {
	if (FAILED(result)) {
		String resultString = "HRESULT=0x%X";
		String errorMsg = vformat("%s, HRESULT=0x%X", errorMessage, (uint64_t)result);
		ERR_PRINT(errorMsg);
		return false;
	}

	if(succeedMessage != "") {
		print_line(succeedMessage);
	}

	return true;
}

char* GodotGDK::CopyStringToChar(String string) {
	CharString cs = string.utf8();
	int len = cs.length();

	char* out = new char[len + 1];
	memcpy(out, cs.get_data(), len);
	out[len] = '\0';

	return out;
}

static void OnGameInviteReceived(void* context, const char* inviteUri) {
	GodotGDK* self = static_cast<GodotGDK*>(context);
	self->call_deferred("emit_signal", "game_invite_received", String(inviteUri ? inviteUri : ""));
}

int64_t GodotGDK::get_xbox_title_id() {
	uint32_t title_id = 0;
	HRESULT hr = XGameGetXboxTitleId(&title_id);
	ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XGameGetXboxTitleId Error: 0x%08ux", (uint64_t)hr));
	return (int64_t)title_id;
}

void GodotGDK::launch_new_game(const String &exe_path, const String &args, Ref<GDKUser> default_user) {
	XUserHandle user = default_user.is_valid() ? default_user->get_user() : nullptr;
	const char *args_ptr = args.is_empty() ? nullptr : args.utf8().get_data();
	XLaunchNewGame(exe_path.utf8().get_data(), args_ptr, user);
}

int64_t GodotGDK::launch_restart_on_crash(const String &args) {
	const char *args_ptr = args.is_empty() ? nullptr : args.utf8().get_data();
	HRESULT hr = XLaunchRestartOnCrash(args_ptr, 0);
	ERR_FAIL_COND_V_MSG(FAILED(hr), (int64_t)hr, vformat("XLaunchRestartOnCrash Error: 0x%08ux", (uint64_t)hr));
	return (int64_t)hr;
}

void GodotGDK::set_event_objects(TypedArray<GDKEventObject> in_objects) {
	_event_objects = in_objects;
}
