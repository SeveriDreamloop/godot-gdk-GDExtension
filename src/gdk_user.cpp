#include "gdk_user.h"
#include "gdk_helpers.h"
#include "gdk_asyncblock.h"
#include <classes/image.hpp>

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

void GDKUser::_bind_methods() {
    ClassDB::bind_static_method("GDKUser", D_METHOD("add_user_async", "options"), &GDKUser::add_user_async);
    ClassDB::bind_static_method("GDKUser", D_METHOD("add_user_by_id_with_ui_async", "user_id"), &GDKUser::add_user_by_id_with_ui_async);
    ClassDB::bind_static_method("GDKUser", D_METHOD("get_max_users"), &GDKUser::get_max_users);
    ClassDB::bind_static_method("GDKUser", D_METHOD("is_sign_out_present"), &GDKUser::is_sign_out_present);
    ClassDB::bind_static_method("GDKUser", D_METHOD("find_user_by_local_id", "local_id"), &GDKUser::find_user_by_local_id);
    ClassDB::bind_static_method("GDKUser", D_METHOD("find_user_for_device", "device_id"), &GDKUser::find_user_by_device);
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
}

Ref<GDKUser> GDKUser::create(XUserHandle user) {
    Ref<GDKUser> wrapper;
    if (user) {
        wrapper.instantiate();
        wrapper->_user = user;
    }
	return wrapper;
}

Ref<GDKAsyncBlock> GDKUser::add_user_async(BitField<GDKXUserAddOptions::Enum> options) {
    XUserAddOptions opts = (XUserAddOptions)((int64_t) options);
    XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(queue);
    b->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);

        XUserHandle user = nullptr;
        HRESULT hr = XUserAddResult(async, &user);
        Dictionary d;
        d["code"] = hr;
        if (SUCCEEDED(hr)) {
            d["user"] = GDKUser::create(user);
        }

        wrapper->emit(d);
    });

    HRESULT hr = XUserAddAsync(opts, b->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserAddAsync Error: %08X", hr));
	return b;
}

Ref<GDKAsyncBlock> GDKUser::add_user_by_id_with_ui_async(int64_t user_id) {
	/* 
        https://github.com/godotengine/godot-proposals/issues/9740
        Suggested to use int64 instead of uint64 cuz they are the same size
    */
    
    XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(queue);
    b->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);

        XUserHandle user = nullptr;
        HRESULT hr = XUserAddByIdWithUiResult(async, &user);
        Dictionary d;
        d["code"] = hr;
        if (SUCCEEDED(hr)) {
            d["user"] = GDKUser::create(user);
        }

        wrapper->emit(d);
    });
    HRESULT hr = XUserAddByIdWithUiAsync(user_id, b->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserAddByIdWithUiAsync Error: %08X", hr));
    return b;
}

int GDKUser::get_max_users() {
    uint32_t max_users = 0;
    HRESULT hr = XUserGetMaxUsers(&max_users);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XUserGetMaxUsers Error: %08X", hr));
	return max_users;
}

Ref<GDKUser> godot::GDKUser::find_user_by_id(int64_t user_id) {
    XUserHandle handle;
    HRESULT hr = XUserFindUserById(user_id, &handle);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserFindUserById Error: %08X", hr));
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
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserFindUserByLocalId Error: %08X", hr));
    return GDKUser::create(user);
}

Ref<GDKUser> GDKUser::find_user_by_device(const PackedByteArray &device_id) {
	ERR_FAIL_COND_V_MSG(device_id.size() != APP_LOCAL_DEVICE_ID_SIZE, Ref<GDKUser>(), "Invalid device id size");

    APP_LOCAL_DEVICE_ID id;
    memcpy(&id, device_id.ptr(), sizeof(APP_LOCAL_DEVICE_ID));

    XUserHandle user = nullptr;
    HRESULT hr = XUserFindForDevice(&id, &user);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserFindForDevice Error: %08X", hr));
    return GDKUser::create(user);
}

Ref<GDKUser> GDKUser::duplicate() const {
    XUserHandle handle = nullptr;
    HRESULT hr = XUserDuplicateHandle(_user, &handle);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKUser::create(nullptr), vformat("XUserDuplicateHandle Error: %08X", hr));
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
    Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(queue);
    b->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);

        HRESULT hr = XUserSignOutResult(async);
        Dictionary d;
        d["code"] = hr;
        wrapper->emit(d); 
    });
    HRESULT hr = XUserSignOutAsync(_user, b->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserSignOutAsync Error: %08X", hr));
    return b;
}

int64_t GDKUser::get_id() const {
    uint64_t id;
	HRESULT hr = XUserGetId(_user, &id);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XUserGetId Error: %08X", hr));
    return id;
}

int64_t GDKUser::get_local_id() const {
	XUserLocalId id;
    HRESULT hr = XUserGetLocalId(_user, &id);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XUserGetLocalId Error: %08X", hr));
    return id.value;
}

bool GDKUser::is_guest() const {
	bool ret = false;
    HRESULT hr = XUserGetIsGuest(_user, &ret);
    ERR_FAIL_COND_V_MSG(FAILED(hr), false, vformat("XUserIsGuest Error: %08X", hr));
    return ret;
}

GDKXUserState::Enum godot::GDKUser::get_state() const {
    XUserState state = XUserState::SignedIn;
    HRESULT hr = XUserGetState(_user, &state);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKXUserState::Enum::SignedIn, vformat("XUserGetState Error: %08X", hr));
	return (GDKXUserState::Enum)state;
}

String GDKUser::get_gamer_tag(GDKXUserGamertagComponent::Enum component) const {
	XUserGamertagComponent tag = (XUserGamertagComponent)component;

    char* gamer_tag = nullptr;
    size_t len = 0;
    HRESULT hr = XUserGetGamertag(_user, tag, 128, gamer_tag, &len);
    ERR_FAIL_COND_V_MSG(FAILED(hr), String(), vformat("XUserGetGamertag Error: %08X", hr));

    if (len > 0) {
        return String(gamer_tag);
    }
    return String();
}

Ref<GDKAsyncBlock> GDKUser::get_gamer_picture_async(GDKXUserGamerPictureSize::Enum size) const {
    XUserGamerPictureSize psize = (XUserGamerPictureSize)size;
    XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(queue);
    b->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        PackedByteArray byte_array;
        size_t bufferSize = 0;
        HRESULT hr = XUserGetGamerPictureResultSize(async, &bufferSize);
        byte_array.resize(bufferSize);
        Dictionary d;
        d["code"] = hr;

        if (SUCCEEDED(hr)) {
            size_t bufferUsed = 0;
            hr = XUserGetGamerPictureResult(async, bufferSize, byte_array.ptrw(), &bufferUsed);
            d["code"] = hr;

            if (SUCCEEDED(hr)) {
                size_t sqrtB = sqrt(bufferUsed);
                Ref<Image> image = Image::create_from_data(sqrtB, sqrtB, false, Image::FORMAT_RGBA8, byte_array);
                d["image"] = image;
            }
        }

        wrapper->emit(d);
    });

    HRESULT hr = XUserGetGamerPictureAsync(_user, psize, b->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserGetGamerPictureAsync Error: %08X", hr));
    return b;
}

GDKXUserAgeGroup::Enum GDKUser::get_user_age_group() const {
    XUserAgeGroup age_group = XUserAgeGroup::Unknown;
    HRESULT hr = XUserGetAgeGroup(_user, &age_group);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKXUserAgeGroup::Enum::Unknown, vformat("XUserGetAgeGroup Error: %08X", hr));
	return (GDKXUserAgeGroup::Enum)age_group;
}

GDKXUserPrivilegeDenyReason::Enum GDKUser::check_privilege(BitField<GDKXUserPrivilegeOptions::Enum> options, GDKXUserPrivilege::Enum privilege) const {
	XUserPrivilegeOptions opt = (XUserPrivilegeOptions)((int64_t) options);
    XUserPrivilegeDenyReason deny_reason = XUserPrivilegeDenyReason::None;

    bool has_privilege = false;
    HRESULT hr = XUserCheckPrivilege(_user, opt, (XUserPrivilege)privilege, &has_privilege, &deny_reason);
    ERR_FAIL_COND_V_MSG(FAILED(hr), (GDKXUserPrivilegeDenyReason::Enum)deny_reason, vformat("XUserCheckPrivilege Error: %08X", hr));
    return GDKXUserPrivilegeDenyReason::Enum::None;
}

Ref<GDKAsyncBlock> GDKUser::resolve_privilege_with_ui_async(BitField<GDKXUserPrivilegeOptions::Enum> options, GDKXUserPrivilege::Enum privilege) const {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(queue);
    b->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        HRESULT hr = XUserResolvePrivilegeWithUiResult(async);
        Dictionary d;
        d["code"] = hr;
        wrapper->emit(d);
    });
    XUserPrivilegeOptions opt = (XUserPrivilegeOptions)((int64_t) options);
    HRESULT hr = XUserResolvePrivilegeWithUiAsync(_user, opt, (XUserPrivilege)privilege, b->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserResolvePrivilegeWithUiAsync Error: %08X", hr));
    return b;
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
        Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(queue);
        b->set_callback([](XAsyncBlock* async) {
            GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
            Dictionary d;
            size_t bufferSize = 0;
            HRESULT hr = XUserGetTokenAndSignatureResultSize(async, &bufferSize);
            d["code"] = hr;

            if (SUCCEEDED(hr)) {
                void* buffer = nullptr;
                XUserGetTokenAndSignatureData* data = nullptr;
                hr = XUserGetTokenAndSignatureResult(async, bufferSize, &buffer, &data, nullptr);
                d["code"] = hr;

                if (SUCCEEDED(hr)) {
                    d["data"] = GDKXUserGetTokenAndSignatureData::create(data);
                }
            }
            wrapper->emit(d);
        });
        HRESULT hr = XUserGetTokenAndSignatureAsync(_user, opt, method.utf8(), url.utf8(), headers.size(),
            header_arr.ptr(), body.size(), body.ptr(), b->get_block());
        
        ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserGetTokenAndSignatureAsync Error: %08X", hr));
        return b;
}

Ref<GDKAsyncBlock> GDKUser::resolve_issue_with_ui_async(const String &url) const {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
    Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(queue);
    b->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        HRESULT hr = XUserResolveIssueWithUiResult(async);
        Dictionary d;
        d["code"] = hr;
        wrapper->emit(d);
    });

    HRESULT hr = XUserResolveIssueWithUiAsync(_user, url.utf8().get_data(), b->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserResolveIssueWithUiAsync Error: %08X", hr));
    return b;
}

Ref<GDKXUserSignOutDeferralHandle> GDKUser::get_sign_out_deferral() const {
    XUserSignOutDeferralHandle deferral = nullptr;
    HRESULT hr = XUserGetSignOutDeferral(&deferral);
    ERR_FAIL_COND_V_MSG(FAILED(hr), GDKXUserSignOutDeferralHandle::create(nullptr), vformat("XUserGetSignOutDeferral Error: %08X", hr));
    return GDKXUserSignOutDeferralHandle::create(deferral);
}

void GDKUser::close_sign_out_deferral_handle(Ref<GDKXUserSignOutDeferralHandle> handle) {
    XUserSignOutDeferralHandle deferral = handle->get_data();
    XUserCloseSignOutDeferralHandle(deferral);
}

Ref<GDKAsyncBlock> GDKUser::find_controller_for_user_with_ui_async() const {
    Ref<GDKAsyncBlock> b = GDKAsyncBlock::create(GDKHelpers::get_async_queue());
    b->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        Dictionary d;
        APP_LOCAL_DEVICE_ID id;
        HRESULT hr = XUserFindControllerForUserWithUiResult(async, &id);
        d["code"] = hr;
        if (SUCCEEDED(hr)) {
            PackedByteArray byte_array;
            byte_array.resize(sizeof(APP_LOCAL_DEVICE_ID));
            memcpy(byte_array.ptrw(), &id, sizeof(APP_LOCAL_DEVICE_ID));
            d["device_id"] = byte_array;
        }
        wrapper->emit(d);
    });
    HRESULT hr = XUserFindControllerForUserWithUiAsync(_user, b->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), b, vformat("XUserFindControllerForUserWithUiAsync Error: %08X", hr));
	return b;
}

bool GDKUser::is_store_user() const {
	return XUserIsStoreUser(_user);
}
