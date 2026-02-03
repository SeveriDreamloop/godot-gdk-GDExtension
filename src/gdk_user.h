#pragma once

#include <classes/ref_counted.hpp>
#include <windows.h>
#include <XUser.h>
#include <variant/typed_dictionary.hpp>
#include "gdk_macros.h"

namespace godot {
    class GDKAsyncBlock;

class GDKXUserAddOptions : public Object {
    GDCLASS(GDKXUserAddOptions, Object);

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        None = 0x00,
        AddDefaultUserSilently = 0x01,
        AllowGuests = 0x02,
        AddDefaultUserAllowingUI = 0x04,
    };
};

class GDKXUserState: public Object {
    GDCLASS(GDKXUserState, Object)

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        SignedIn = 0,
        SigningOut = 1,
        SignedOut = 2
    };
};
class GDKXUserGamertagComponent: public Object {
    GDCLASS(GDKXUserGamertagComponent, Object)

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        Classic = 0,
        Modern = 1,
        ModernSuffix = 2,
        UniqueModern = 3
    };
};

class GDKXUserGamerPictureSize : public Object {
    GDCLASS(GDKXUserGamerPictureSize, Object)
    
protected:
    static void _bind_methods();
    
public:
    enum Enum: uint32_t {
        Small = 0, // 64x64
        Medium = 1, // 208x208
        Large = 2, // 424x424
        ExtraLarge = 3 // 1080x1080
    };
};

class GDKXUserAgeGroup: public Object {
    GDCLASS(GDKXUserAgeGroup, Object)
    
protected:
    static void _bind_methods();
    
public:
    enum Enum: uint32_t {
        Unknown = 0,
        Child = 1,
        Teen = 2,
        Adult = 3
    };
};

class GDKXUserPrivilege: public Object {
    GDCLASS(GDKXUserPrivilege, Object)
    
protected:
    static void _bind_methods();
    
public:
    enum Enum: uint32_t {
        CrossPlay = 185,
        Clubs = 188,
        Sessions = 189,
        Broadcast = 190,
        ManageProfilePrivacy = 196,
        GameDvr = 198,
        MultiplayerParties = 203,
        CloudManageSession = 207,
        CloudJoinSession = 208,
        CloudSavedGames = 209,
        SocialNetworkSharing = 220,
        UserGeneratedContent = 247,
        Communications = 252,
        Multiplayer = 254,
        AddFriends = 255
    };
};

class GDKXUserPrivilegeDenyReason : public Object {
    GDCLASS(GDKXUserPrivilegeDenyReason, Object)
    
    protected:
    static void _bind_methods();
    
    public:
    enum Enum: uint32_t {
        None = 0,
        PurchaseRequired = 1,
        Restricted = 2,
        Banned = 3,
        Unknown = 0xFFFFFFFF
    };
};


class GDKXUserPrivilegeOptions: public Object {
    GDCLASS(GDKXUserPrivilegeOptions, Object)

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        None = 0x00,
        AllUsers = 0x01
    };
};

class GDKXUserGetTokenAndSignatureOptions: public Object {
    GDCLASS(GDKXUserGetTokenAndSignatureOptions, Object)

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        None = 0x00,
        ForceRefresh = 0x01,
        AllUsers = 0x02
    };
};

class GDKXUserDefaultAudioEndpointKind: public Object {
    GDCLASS(GDKXUserDefaultAudioEndpointKind, Object)

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        CommunicationRender = 0,
        CommunicationCapture = 1
    };
};

GDKOBJECT(GDKXUserGetTokenAndSignatureData, XUserGetTokenAndSignatureData*, RefCounted);
GDKOBJECT(GDKXUserSignOutDeferralHandle, XUserSignOutDeferralHandle, RefCounted);

class Image;
class GDKUser : public RefCounted {
    GDCLASS(GDKUser, RefCounted);

private:
    XUserHandle _user = nullptr;

protected:
    static void _bind_methods();

public:
    inline XUserHandle get_user() const { return _user; }
    static Ref<GDKUser> create(XUserHandle user);
    static Ref<GDKAsyncBlock> add_user_async(BitField<GDKXUserAddOptions::Enum> options);
    static Ref<GDKAsyncBlock> add_user_by_id_with_ui_async(int64_t user_id);
    static int get_max_users();
    static Ref<GDKUser> find_user_by_id(int64_t user_id);
    static bool is_sign_out_present();
    static Ref<GDKUser> find_user_by_local_id(int64_t local_id);
    static Ref<GDKUser> find_user_by_device(const PackedByteArray& device_id);

    Ref<GDKUser> duplicate() const;
    void close_handle();
    bool compare(const Ref<GDKUser>& other) const;
    Ref<GDKAsyncBlock> sign_out() const;
    int64_t get_id() const;
    int64_t get_local_id() const;
    bool is_guest() const;
    GDKXUserState::Enum get_state() const;
    String get_gamer_tag(GDKXUserGamertagComponent::Enum component) const;
    Ref<GDKAsyncBlock> get_gamer_picture_async(GDKXUserGamerPictureSize::Enum size) const;
    GDKXUserAgeGroup::Enum get_user_age_group() const;
    GDKXUserPrivilegeDenyReason::Enum check_privilege(BitField<GDKXUserPrivilegeOptions::Enum> options, GDKXUserPrivilege::Enum privilege) const;
    Ref<GDKAsyncBlock> resolve_privilege_with_ui_async(BitField<GDKXUserPrivilegeOptions::Enum> options, GDKXUserPrivilege::Enum privilege) const;
    Ref<GDKAsyncBlock> get_token_and_signature_async(BitField<GDKXUserGetTokenAndSignatureOptions::Enum> options, const String& method,
        const String& url, const TypedDictionary<String, String>& headers, const PackedByteArray& body) const;
    Ref<GDKAsyncBlock> resolve_issue_with_ui_async(const String& url = "") const;
    Ref<GDKXUserSignOutDeferralHandle> get_sign_out_deferral() const;
    void close_sign_out_deferral_handle(Ref<GDKXUserSignOutDeferralHandle> handle);
    Ref<GDKAsyncBlock> find_controller_for_user_with_ui_async() const;
    bool is_store_user() const;
};
}

VARIANT_BITFIELD_CAST(GDKXUserAddOptions::Enum);
VARIANT_ENUM_CAST(GDKXUserState::Enum);
VARIANT_ENUM_CAST(GDKXUserGamertagComponent::Enum);
VARIANT_ENUM_CAST(GDKXUserGamerPictureSize::Enum);
VARIANT_ENUM_CAST(GDKXUserAgeGroup::Enum);
VARIANT_ENUM_CAST(GDKXUserPrivilege::Enum);
VARIANT_BITFIELD_CAST(GDKXUserPrivilegeOptions::Enum);
VARIANT_ENUM_CAST(GDKXUserPrivilegeDenyReason::Enum);
VARIANT_BITFIELD_CAST(GDKXUserGetTokenAndSignatureOptions::Enum);
VARIANT_ENUM_CAST(GDKXUserDefaultAudioEndpointKind::Enum);