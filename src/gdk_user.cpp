#include "gdk_user.h"
#include "gdk_helpers.h"
#include "gdk_asyncblock.h"
#include "godot_gdk.h"

#include <classes/image.hpp>
#include <xsapi-c/xbox_live_context_c.h>

using namespace godot;

void GDKXUserAddOptions::_bind_methods() {
    BIND_BITFIELD_FLAG(None);
    BIND_BITFIELD_FLAG(AddDefaultUserSilently);
    BIND_BITFIELD_FLAG(AllowGuests);
    BIND_BITFIELD_FLAG(AddDefaultUserAllowingUI);
}

void GDKXUserState::_bind_methods() {
    BIND_ENUM_CONSTANT(SignedIn);
    BIND_ENUM_CONSTANT(SigningOut);
    BIND_ENUM_CONSTANT(SignedOut);
}

void GDKXUserGamertagComponent::_bind_methods() {
    BIND_ENUM_CONSTANT(Classic);
    BIND_ENUM_CONSTANT(Modern);
    BIND_ENUM_CONSTANT(ModernSuffix);
    BIND_ENUM_CONSTANT(UniqueModern);
}

void GDKXUserGamerPictureSize::_bind_methods() {
    BIND_ENUM_CONSTANT(Small);
    BIND_ENUM_CONSTANT(Medium);
    BIND_ENUM_CONSTANT(Large);
    BIND_ENUM_CONSTANT(ExtraLarge);
}

void GDKXUserAgeGroup::_bind_methods() {
    BIND_ENUM_CONSTANT(Unknown);
    BIND_ENUM_CONSTANT(Child);
    BIND_ENUM_CONSTANT(Teen);
    BIND_ENUM_CONSTANT(Adult);
}

void GDKXUserPrivilege::_bind_methods() {
    BIND_ENUM_CONSTANT(CrossPlay);
    BIND_ENUM_CONSTANT(Clubs);
    BIND_ENUM_CONSTANT(Sessions);
    BIND_ENUM_CONSTANT(Broadcast);
    BIND_ENUM_CONSTANT(ManageProfilePrivacy);
    BIND_ENUM_CONSTANT(GameDvr);
    BIND_ENUM_CONSTANT(MultiplayerParties);
    BIND_ENUM_CONSTANT(CloudManageSession);
    BIND_ENUM_CONSTANT(CloudJoinSession);
    BIND_ENUM_CONSTANT(CloudSavedGames);
    BIND_ENUM_CONSTANT(SocialNetworkSharing);
    BIND_ENUM_CONSTANT(UserGeneratedContent);
    BIND_ENUM_CONSTANT(Communications);
    BIND_ENUM_CONSTANT(Multiplayer);
    BIND_ENUM_CONSTANT(AddFriends);
}

void GDKXUserPrivilegeDenyReason::_bind_methods() {
    BIND_ENUM_CONSTANT(None);
    BIND_ENUM_CONSTANT(PurchaseRequired);
    BIND_ENUM_CONSTANT(Restricted);
    BIND_ENUM_CONSTANT(Banned);
    BIND_ENUM_CONSTANT(Unknown);
}

void GDKXUserPrivilegeOptions::_bind_methods() {
    BIND_BITFIELD_FLAG(None);
    BIND_BITFIELD_FLAG(AllUsers);
}

void GDKXUserGetTokenAndSignatureOptions::_bind_methods() {
    BIND_BITFIELD_FLAG(None);
    BIND_BITFIELD_FLAG(ForceRefresh);
    BIND_BITFIELD_FLAG(AllUsers);
}

void GDKXUserDefaultAudioEndpointKind::_bind_methods() {
    BIND_ENUM_CONSTANT(CommunicationRender);
    BIND_ENUM_CONSTANT(CommunicationCapture);
}


XTaskQueueRegistrationToken GDKUser::_user_change_token;
XTaskQueueRegistrationToken GDKUser::_device_association_change_token;
XTaskQueueRegistrationToken GDKUser::_default_audio_endpoint_change_token;

void UserChangeEventCallback(void* context, XUserLocalId userLocalId, XUserChangeEvent event) {
	XUserHandle userHandle;

	HRESULT hr = XUserFindUserByLocalId(userLocalId, &userHandle);
	ERR_FAIL_COND_MSG(FAILED(hr), vformat("UserChangeEvent Error: 0x%08ux", (uint64_t)hr));

	Ref<GDKUser> user = GDKUser::create(userHandle);
	GDKXUserChangeEvent::Enum event_enum = static_cast<GDKXUserChangeEvent::Enum>(event);

	Object* self = static_cast<Object*>(context);
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

	Object* self = static_cast<Object*>(context);
	self->call_deferred("emit_signal", "device_association_changed", device_id, oldUser, newUser);
}

void DefaultAudioEndpointUtf16ChangeCallback(void* context, XUserLocalId userLocalId, XUserDefaultAudioEndpointKind endpointKind, const wchar_t* endpointIdUtf16)
{
	XUserHandle userHandle;
	HRESULT hr = XUserFindUserByLocalId(userLocalId, &userHandle);
	ERR_FAIL_COND_MSG(FAILED(hr), vformat("DefaultAudioEndpointUtf16ChangeCallback FindUserByLocalId Error: 0x%08ux", (uint64_t)hr));

	GDKXUserDefaultAudioEndpointKind::Enum endpoint_enum = static_cast<GDKXUserDefaultAudioEndpointKind::Enum>(endpointKind);
	String endpoint_id = String(endpointIdUtf16);

	Object* self = static_cast<Object*>(context);
	self->call_deferred("emit_signal", "default_audio_endpoint_utf16_changed", GDKUser::create(userHandle), endpoint_enum, endpoint_id);
}

void GDKUser::_bind_methods() {
    ClassDB::bind_static_method("GDKUser", D_METHOD("add_user_async", "options"), &GDKUser::add_user_async);
    ClassDB::bind_static_method("GDKUser", D_METHOD("add_user_by_id_with_ui_async", "user_id"), &GDKUser::add_user_by_id_with_ui_async);
    ClassDB::bind_static_method("GDKUser", D_METHOD("get_max_users"), &GDKUser::get_max_users);
    ClassDB::bind_static_method("GDKUser", D_METHOD("find_user_by_id", "local_id"), &GDKUser::find_user_by_id);
    ClassDB::bind_static_method("GDKUser", D_METHOD("is_sign_out_present"), &GDKUser::is_sign_out_present);
    ClassDB::bind_static_method("GDKUser", D_METHOD("find_user_by_local_id", "local_id"), &GDKUser::find_user_by_local_id);
    ClassDB::bind_static_method("GDKUser", D_METHOD("find_user_by_device", "device_id"), &GDKUser::find_user_by_device);
    ClassDB::bind_method(D_METHOD("duplicate"), &GDKUser::duplicate);
    ClassDB::bind_method(D_METHOD("close_handle"), &GDKUser::close_handle);
    ClassDB::bind_method(D_METHOD("compare", "other"), &GDKUser::compare);
    ClassDB::bind_method(D_METHOD("sign_out"), &GDKUser::sign_out);
    ClassDB::bind_method(D_METHOD("get_id"), &GDKUser::get_id);
    ClassDB::bind_method(D_METHOD("get_local_id"), &GDKUser::get_local_id);
    ClassDB::bind_method(D_METHOD("is_guest"), &GDKUser::is_guest);
    ClassDB::bind_method(D_METHOD("get_state"), &GDKUser::get_state);
    ClassDB::bind_method(D_METHOD("get_gamer_tag", "component"), &GDKUser::get_gamer_tag);
    ClassDB::bind_method(D_METHOD("get_gamer_picture_async", "size"), &GDKUser::get_gamer_picture_async);
    ClassDB::bind_method(D_METHOD("get_user_age_group"), &GDKUser::get_user_age_group);
    ClassDB::bind_method(D_METHOD("check_privilege", "options", "privilege"), &GDKUser::check_privilege);
    ClassDB::bind_method(D_METHOD("resolve_privilege_with_ui_async", "options", "privilege"), &GDKUser::resolve_privilege_with_ui_async);
    ClassDB::bind_method(D_METHOD("get_token_and_signature_async", "options", "method", "url", "headers", "body"), &GDKUser::get_token_and_signature_async);
    ClassDB::bind_method(D_METHOD("resolve_issue_with_ui_async", "url"), &GDKUser::resolve_issue_with_ui_async);
    ClassDB::bind_method(D_METHOD("get_sign_out_deferral"), &GDKUser::get_sign_out_deferral);
    ClassDB::bind_method(D_METHOD("close_sign_out_deferral_handle", "handle"), &GDKUser::close_sign_out_deferral_handle);
    ClassDB::bind_method(D_METHOD("find_controller_for_user_with_ui_async"), &GDKUser::find_controller_for_user_with_ui_async);
    ClassDB::bind_method(D_METHOD("is_store_user"), &GDKUser::is_store_user);

    GodotGDK::initialize_listeners.push_back([](Object* node) {
		HRESULT hr = XUserRegisterForChangeEvent(GDKHelpers::get_async_queue(), node, &UserChangeEventCallback, &_user_change_token);
		GodotGDK::CheckResult(hr, "User change event registered", "Failed to register user change event");
	});
	GodotGDK::deinitialize_listeners.push_back([] {
		XUserUnregisterForChangeEvent(_user_change_token, false);
	});

	GodotGDK::initialize_listeners.push_back([](Object* node) {
		HRESULT hr = XUserRegisterForDeviceAssociationChanged(GDKHelpers::get_async_queue(), node, &DeviceAssociationChangeCallback, &_device_association_change_token);
		GodotGDK::CheckResult(hr, "Device association change event registered", "Failed to register device association change event");
	});
	GodotGDK::deinitialize_listeners.push_back([] {
			XUserUnregisterForDeviceAssociationChanged(_device_association_change_token, false);
	});

	GodotGDK::initialize_listeners.push_back([](Object* node) {
		HRESULT hr = XUserRegisterForDefaultAudioEndpointUtf16Changed(GDKHelpers::get_async_queue(), node, &DefaultAudioEndpointUtf16ChangeCallback, &_default_audio_endpoint_change_token);
		GodotGDK::CheckResult(hr, "Default audio endpoint change event registered", "Failed to register default audio endpoint change event");
	});
	GodotGDK::deinitialize_listeners.push_back([] {
			XUserUnregisterForDefaultAudioEndpointUtf16Changed(_default_audio_endpoint_change_token, false);
	});
}

Ref<GDKUser> GDKUser::create(XUserHandle user) {
    Ref<GDKUser> wrapper;
    if (user) {
        wrapper.instantiate();
        wrapper->_user = user;
        HRESULT hr = XblContextCreateHandle(user, &wrapper->_xbl_context);
        ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XblContextCreateHandle failed in GDKUser::create, Error: 0x%08ux", (uint64_t)hr));
    }
	return wrapper;
}

Ref<GDKAsyncBlock> GDKUser::add_user_async(BitField<GDKXUserAddOptions::Enum> options) {
    XUserAddOptions opts = (XUserAddOptions)((int64_t) options);
    XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);

        XUserHandle user = nullptr;
        HRESULT hr = XUserAddResult(async, &user);
        Dictionary return_data;
        return_data["hresult"] = hr;
        if (SUCCEEDED(hr)) {
            return_data["user"] = GDKUser::create(user);
        }

        wrapper->emit(return_data);
    });

    HRESULT hr = XUserAddAsync(opts, asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserAddAsync Error: 0x%08ux", (uint64_t)hr));
	return asyncBlock;
}

Ref<GDKAsyncBlock> GDKUser::add_user_by_id_with_ui_async(int64_t user_id) {
	/* 
        https://github.com/godotengine/godot-proposals/issues/9740
        Suggested to use int64 instead of uint64 cuz they are the same size
    */
    
    XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);

        XUserHandle user = nullptr;
        HRESULT hr = XUserAddByIdWithUiResult(async, &user);
        Dictionary return_data;
        return_data["hresult"] = hr;
        if (SUCCEEDED(hr)) {
            return_data["user"] = GDKUser::create(user);
        }

        wrapper->emit(return_data);
    });
    HRESULT hr = XUserAddByIdWithUiAsync(user_id, asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserAddByIdWithUiAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

int GDKUser::get_max_users() {
    uint32_t max_users = 0;
    HRESULT hr = XUserGetMaxUsers(&max_users);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XUserGetMaxUsers Error: 0x%08ux", (uint64_t)hr));
	return max_users;
}

Ref<GDKUser> GDKUser::find_user_by_id(int64_t user_id) {
    XUserHandle handle;
    HRESULT hr = XUserFindUserById(user_id, &handle);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserFindUserById Error: 0x%08ux", (uint64_t)hr));
	return GDKUser::create(handle);
}

bool GDKUser::is_sign_out_present() {
	return XUserIsSignOutPresent();
}

Ref<GDKUser> GDKUser::find_user_by_local_id(int64_t local_id) {
    XUserLocalId id;
    id.value = local_id;

    XUserHandle user = nullptr;
    HRESULT hr = XUserFindUserByLocalId(id, &user);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserFindUserByLocalId Error: 0x%08ux", (uint64_t)hr));
    return GDKUser::create(user);
}

Ref<GDKUser> GDKUser::find_user_by_device(const PackedByteArray &device_id) {
	ERR_FAIL_COND_V_MSG(device_id.size() != APP_LOCAL_DEVICE_ID_SIZE, Ref<GDKUser>(), "Invalid device id size");

    APP_LOCAL_DEVICE_ID id;
    memcpy(&id, device_id.ptr(), sizeof(APP_LOCAL_DEVICE_ID));

    XUserHandle user = nullptr;
    HRESULT hr = XUserFindForDevice(&id, &user);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserFindForDevice Error: 0x%08ux", (uint64_t)hr));
    return GDKUser::create(user);
}

Ref<GDKUser> GDKUser::duplicate() const {
    XUserHandle handle = nullptr;
    HRESULT hr = XUserDuplicateHandle(_user, &handle);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserDuplicateHandle Error: 0x%08ux", (uint64_t)hr));
    return GDKUser::create(handle);
}

void GDKUser::close_handle() {
    XUserCloseHandle(_user);
}

bool GDKUser::compare(const Ref<GDKUser> &other) const {
	return XUserCompare(_user, other->get_user()) == 0;
}

Ref<GDKAsyncBlock> GDKUser::sign_out() const {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);

        HRESULT hr = XUserSignOutResult(async);
        Dictionary return_data;
        return_data["hresult"] = hr;
        wrapper->emit(return_data); 
    });
    HRESULT hr = XUserSignOutAsync(_user, asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserSignOutAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

int64_t GDKUser::get_id() const {
    uint64_t id;
	HRESULT hr = XUserGetId(_user, &id);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XUserGetId Error: 0x%08ux", (uint64_t)hr));
    return id;
}

int64_t GDKUser::get_local_id() const {
	XUserLocalId id;
    HRESULT hr = XUserGetLocalId(_user, &id);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XUserGetLocalId Error: 0x%08ux", (uint64_t)hr));
    return id.value;
}

bool GDKUser::is_guest() const {
	bool ret = false;
    HRESULT hr = XUserGetIsGuest(_user, &ret);
    ERR_FAIL_COND_V_MSG(FAILED(hr), false, vformat("XUserIsGuest Error: 0x%08ux", (uint64_t)hr));
    return ret;
}

GDKXUserState::Enum GDKUser::get_state() const {
    XUserState state = XUserState::SignedIn;
    HRESULT hr = XUserGetState(_user, &state);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKXUserState::Enum::SignedIn, vformat("XUserGetState Error: 0x%08ux", (uint64_t)hr));
	return (GDKXUserState::Enum)state;
}

String GDKUser::get_gamer_tag(GDKXUserGamertagComponent::Enum component) const {
	XUserGamertagComponent tag = (XUserGamertagComponent)component;

    char gamer_tag[128];
    size_t len = 0;
    HRESULT hr = XUserGetGamertag(_user, tag, sizeof(gamer_tag), gamer_tag, &len);
    ERR_FAIL_COND_V_MSG(FAILED(hr), String(), vformat("XUserGetGamertag Error: 0x%08ux", (uint64_t)hr));

    if (len > 0) {
        return String(gamer_tag);
    }
    return String();
}

Ref<GDKAsyncBlock> GDKUser::get_gamer_picture_async(GDKXUserGamerPictureSize::Enum size) const {
    XUserGamerPictureSize psize = (XUserGamerPictureSize)size;
    XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        PackedByteArray byte_array;
        size_t bufferSize = 0;
        HRESULT hr = XUserGetGamerPictureResultSize(async, &bufferSize);
        byte_array.resize(bufferSize);
        Dictionary return_data;
        return_data["hresult"] = hr;

        if (SUCCEEDED(hr)) {
            size_t bufferUsed = 0;
            hr = XUserGetGamerPictureResult(async, bufferSize, byte_array.ptrw(), &bufferUsed);
            return_data["hresult"] = hr;

            if (SUCCEEDED(hr)) {
                return_data["hresult"] = hr;

                Ref<Image> image = memnew(Image);
                image->load_png_from_buffer(byte_array);
                return_data["image"] = image;
            }
        }

        wrapper->emit(return_data);
    });

    HRESULT hr = XUserGetGamerPictureAsync(_user, psize, asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserGetGamerPictureAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

GDKXUserAgeGroup::Enum GDKUser::get_user_age_group() const {
    XUserAgeGroup age_group = XUserAgeGroup::Unknown;
    HRESULT hr = XUserGetAgeGroup(_user, &age_group);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKXUserAgeGroup::Enum::Unknown, vformat("XUserGetAgeGroup Error: 0x%08ux", (uint64_t)hr));
	return (GDKXUserAgeGroup::Enum)age_group;
}

GDKXUserPrivilegeDenyReason::Enum GDKUser::check_privilege(BitField<GDKXUserPrivilegeOptions::Enum> options, GDKXUserPrivilege::Enum privilege) const {
	XUserPrivilegeOptions opt = (XUserPrivilegeOptions)((int64_t) options);
    XUserPrivilegeDenyReason deny_reason = XUserPrivilegeDenyReason::None;

    bool has_privilege = false;
    HRESULT hr = XUserCheckPrivilege(_user, opt, (XUserPrivilege)privilege, &has_privilege, &deny_reason);
    ERR_FAIL_COND_V_MSG(FAILED(hr), (GDKXUserPrivilegeDenyReason::Enum)deny_reason, vformat("XUserCheckPrivilege Error: 0x%08ux", (uint64_t)hr));
    return (GDKXUserPrivilegeDenyReason::Enum)deny_reason;
}

Ref<GDKAsyncBlock> GDKUser::resolve_privilege_with_ui_async(BitField<GDKXUserPrivilegeOptions::Enum> options, GDKXUserPrivilege::Enum privilege) const {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        HRESULT hr = XUserResolvePrivilegeWithUiResult(async);
        Dictionary return_data;
        return_data["hresult"] = hr;
        wrapper->emit(return_data);
    });
    XUserPrivilegeOptions opt = (XUserPrivilegeOptions)((int64_t) options);
    HRESULT hr = XUserResolvePrivilegeWithUiAsync(_user, opt, (XUserPrivilege)privilege, asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserResolvePrivilegeWithUiAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

Ref<GDKAsyncBlock> GDKUser::get_token_and_signature_async(BitField<GDKXUserGetTokenAndSignatureOptions::Enum> options, const String &method, const String &url, 
    const TypedDictionary<String, String> &headers, const PackedByteArray &body) const {
        XUserGetTokenAndSignatureOptions opt = (XUserGetTokenAndSignatureOptions)((int64_t) options);

        LocalVector<XUserGetTokenAndSignatureHttpHeader> header_arr;
        header_arr.resize(headers.size());
        TypedArray<String> keys = headers.keys();
        for (int i = 0; i < keys.size(); i++) {
            const char* key = static_cast<String>(keys[i]).utf8().get_data();
            const char* value = static_cast<String>(headers[keys[i]]).utf8().get_data();

            header_arr.push_back({key, value});
        }
        XTaskQueueHandle queue = GDKHelpers::get_async_queue();
        Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);
        asyncBlock->set_callback([](XAsyncBlock* async) {
            GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
            Dictionary return_data;
            size_t bufferSize = 0;
            HRESULT hr = XUserGetTokenAndSignatureResultSize(async, &bufferSize);
            return_data["hresult"] = hr;

            if (SUCCEEDED(hr)) {
                void* buffer = nullptr;
                XUserGetTokenAndSignatureData* data = nullptr;
                hr = XUserGetTokenAndSignatureResult(async, bufferSize, &buffer, &data, nullptr);
                return_data["hresult"] = hr;

                if (SUCCEEDED(hr)) {
                    return_data["data"] = GDKXUserGetTokenAndSignatureData::create(data);
                }
            }
            wrapper->emit(return_data);
        });
        HRESULT hr = XUserGetTokenAndSignatureAsync(_user, opt, method.utf8(), url.utf8(), headers.size(),
            header_arr.ptr(), body.size(), body.ptr(), asyncBlock->get_block());
        
        ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserGetTokenAndSignatureAsync Error: 0x%08ux", (uint64_t)hr));
        return asyncBlock;
}

Ref<GDKAsyncBlock> GDKUser::resolve_issue_with_ui_async(const String &url) const {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        HRESULT hr = XUserResolveIssueWithUiResult(async);
        Dictionary return_data;
        return_data["hresult"] = hr;
        wrapper->emit(return_data);
    });

    HRESULT hr = XUserResolveIssueWithUiAsync(_user, url.utf8().get_data(), asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserResolveIssueWithUiAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

Ref<GDKXUserSignOutDeferralHandle> GDKUser::get_sign_out_deferral() const {
    XUserSignOutDeferralHandle deferral = nullptr;
    HRESULT hr = XUserGetSignOutDeferral(&deferral);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKXUserSignOutDeferralHandle::create(nullptr), vformat("XUserGetSignOutDeferral Error: 0x%08ux", (uint64_t)hr));
    return GDKXUserSignOutDeferralHandle::create(deferral);
}

void GDKUser::close_sign_out_deferral_handle(Ref<GDKXUserSignOutDeferralHandle> handle) {
    XUserSignOutDeferralHandle deferral = handle->get_data();
    XUserCloseSignOutDeferralHandle(deferral);
}

Ref<GDKAsyncBlock> GDKUser::find_controller_for_user_with_ui_async() const {
    Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(GDKHelpers::get_async_queue());
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        Dictionary return_data;
        APP_LOCAL_DEVICE_ID id;
        HRESULT hr = XUserFindControllerForUserWithUiResult(async, &id);
        return_data["hresult"] = hr;
        if (SUCCEEDED(hr)) {
            PackedByteArray byte_array;
            byte_array.resize(sizeof(APP_LOCAL_DEVICE_ID));
            memcpy(byte_array.ptrw(), &id, sizeof(APP_LOCAL_DEVICE_ID));
            return_data["device_id"] = byte_array;
        }
        wrapper->emit(return_data);
    });
    HRESULT hr = XUserFindControllerForUserWithUiAsync(_user, asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XUserFindControllerForUserWithUiAsync Error: 0x%08ux", (uint64_t)hr));
	return asyncBlock;
}

bool GDKUser::is_store_user() const {
	return XUserIsStoreUser(_user);
}
