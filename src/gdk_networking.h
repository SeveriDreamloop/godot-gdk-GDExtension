#pragma once

#include <classes/ref_counted.hpp>
#include <variant/typed_array.hpp>
#include "gdk_asyncblock.h"
#include "gdk_event_object.h"
#include <XNetworking.h>

namespace godot {
class GDKXNetworkingThumbprint: public RefCounted {
    GDCLASS(GDKXNetworkingThumbprint, RefCounted);

public:
    enum ThumbprintType: uint32_t {
        Leaf = XNetworkingThumbprintType::Leaf,
        Issuer = XNetworkingThumbprintType::Issuer,
        Root = XNetworkingThumbprintType::Root
    };

private:
    ThumbprintType _type;
    PackedByteArray _buffer;

protected:
    static void _bind_methods();

public:
    static Ref<GDKXNetworkingThumbprint> create(const XNetworkingThumbprint* thumbprint);

    inline ThumbprintType get_type() const { return _type; }
    inline PackedByteArray get_buffer() const { return _buffer; }
};

class GDKXNetworkingSecurityInformation: public RefCounted {
    GDCLASS(GDKXNetworkingSecurityInformation, RefCounted);

private:
    PackedByteArray _buffer;
    XNetworkingSecurityInformation* _info;

protected:
    static void _bind_methods();

public:
    static Ref<GDKXNetworkingSecurityInformation> create(size_t bufferSize);
    inline uint8_t* buffer_ptrw() { return _buffer.ptrw(); }
    inline XNetworkingSecurityInformation** info_ptrw() { return &_info; }
    inline XNetworkingSecurityInformation* info_ptr() const { return _info; }

    inline int64_t get_enabled_http_security_protocol_flags() const { return _info->enabledHttpSecurityProtocolFlags; }
    inline int64_t get_thumbprints_count() const { return _info->thumbprintCount; }
    TypedArray<Ref<GDKXNetworkingThumbprint>> get_thumbprints() const;
};

class GDKXNetworkingConnectivityLevelHint: public Object {
    GDCLASS(GDKXNetworkingConnectivityLevelHint, Object);

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        Unknown = XNetworkingConnectivityLevelHint::Unknown,
        None = XNetworkingConnectivityLevelHint::None,
        LocalAccess = XNetworkingConnectivityLevelHint::LocalAccess,
        InternetAccess = XNetworkingConnectivityLevelHint::InternetAccess,
        ConstrainedInternetAccess = XNetworkingConnectivityLevelHint::ConstrainedInternetAccess
    };
};

class GDKXNetworkingConnectivityCostHint: public Object {
    GDCLASS(GDKXNetworkingConnectivityCostHint, Object);

protected:
    static void _bind_methods();

public:
    enum Enum: uint32_t {
        Unknown = XNetworkingConnectivityCostHint::Unknown,
        Unrestricted = XNetworkingConnectivityCostHint::Unrestricted,
        Fixed = XNetworkingConnectivityCostHint::Fixed,
        Variable = XNetworkingConnectivityCostHint::Variable
    };
};

class GDKXNetworkingConfigurationSetting: public Object {
    GDCLASS(GDKXNetworkingConfigurationSetting, Object);

protected:
    static void _bind_methods();

public:
    enum Enum : uint32_t {
        MaxTitleTcpQueuedReceiveBufferSize = XNetworkingConfigurationSetting::MaxTitleTcpQueuedReceiveBufferSize,
        MaxSystemTcpQueuedReceiveBufferSize = XNetworkingConfigurationSetting::MaxSystemTcpQueuedReceiveBufferSize,
        MaxToolsTcpQueuedReceiveBufferSize = XNetworkingConfigurationSetting::MaxToolsTcpQueuedReceiveBufferSize
    };
};

class GDKXNetworkingStatisticsType: public Object {
    GDCLASS(GDKXNetworkingStatisticsType, Object);

protected:
    static void _bind_methods();

public:
    enum Enum : uint32_t {
        TitleTcpQueuedReceivedBufferUsage = XNetworkingStatisticsType::TitleTcpQueuedReceivedBufferUsage,
        SystemTcpQueuedReceivedBufferUsage = XNetworkingStatisticsType::SystemTcpQueuedReceivedBufferUsage,
        ToolsTcpQueuedReceivedBufferUsage = XNetworkingStatisticsType::ToolsTcpQueuedReceivedBufferUsage
    };
};

class GDKNetworking : public RefCounted {
    GDCLASS(GDKNetworking, RefCounted);

protected:
    static void _bind_methods();

public:
    static Dictionary get_connectivity_hint();
    static String query_configuration_setting(GDKXNetworkingConfigurationSetting::Enum setting);
    static int64_t query_preferred_local_udp_multiplayer_port();
    static Ref<GDKAsyncBlock> query_preferred_local_udp_multiplayer_port_async();
    static Ref<GDKAsyncBlock> query_security_information_for_url_async(const String& url);
    static Ref<GDKAsyncBlock> query_security_information_for_url_utf16_async(const String& url);
    static Dictionary query_statistics(GDKXNetworkingStatisticsType::Enum type);
    static int set_configuration_setting(GDKXNetworkingConfigurationSetting::Enum setting, const String& int64_string);
};

class GDKNetworkingEvents: public GDKEventObject {
    GDCLASS(GDKNetworkingEvents, RefCounted);

private:
    static GDKNetworkingEvents* _instance;
    XTaskQueueHandle _async_queue = nullptr;
    XTaskQueueRegistrationToken _hint_change_token = {};
    XTaskQueueRegistrationToken _port_change_token = {};

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    GDKNetworkingEvents();
    static GDKNetworkingEvents* get_singleton();
    inline virtual StringName get_singleton_name() const override { return "GDKNetworkingEvents"; }
};
}

VARIANT_ENUM_CAST(GDKXNetworkingStatisticsType::Enum);
VARIANT_ENUM_CAST(GDKXNetworkingThumbprint::ThumbprintType);
VARIANT_ENUM_CAST(GDKXNetworkingConnectivityLevelHint::Enum);
VARIANT_ENUM_CAST(GDKXNetworkingConnectivityCostHint::Enum);
VARIANT_ENUM_CAST(GDKXNetworkingConfigurationSetting::Enum);