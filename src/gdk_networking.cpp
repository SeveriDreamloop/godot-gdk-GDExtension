#include "gdk_networking.h"
#include "gdk_helpers.h"
#include <Windows.h>

using namespace godot;

void GDKXNetworkingThumbprint::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_type"), &GDKXNetworkingThumbprint::get_type);
    ClassDB::bind_method(D_METHOD("get_buffer"), &GDKXNetworkingThumbprint::get_buffer);
}

Ref<GDKXNetworkingThumbprint> GDKXNetworkingThumbprint::create(const XNetworkingThumbprint *thumbprint) {
	Ref<GDKXNetworkingThumbprint> wrapper;
    if (thumbprint != nullptr) {
        wrapper.instantiate();
        wrapper->_type = (GDKXNetworkingThumbprint::ThumbprintType)thumbprint->thumbprintType;

        wrapper->_buffer.resize(thumbprint->thumbprintBufferByteCount);
        memcpy(wrapper->_buffer.ptrw(), thumbprint->thumbprintBuffer, thumbprint->thumbprintBufferByteCount);
    }
    return wrapper;
}

void GDKXNetworkingSecurityInformation::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_enabled_http_security_protocol_flags"), &GDKXNetworkingSecurityInformation::get_enabled_http_security_protocol_flags);
    ClassDB::bind_method(D_METHOD("get_thumbprints_count"), &GDKXNetworkingSecurityInformation::get_thumbprints_count);
    ClassDB::bind_method(D_METHOD("get_thumbprints"), &GDKXNetworkingSecurityInformation::get_thumbprints);
}

Ref<GDKXNetworkingSecurityInformation> GDKXNetworkingSecurityInformation::create(size_t bufferSize) {
	Ref<GDKXNetworkingSecurityInformation> wrapper;
    wrapper.instantiate();
    wrapper->_buffer.resize(bufferSize);
    return wrapper;
}

TypedArray<Ref<GDKXNetworkingThumbprint>> GDKXNetworkingSecurityInformation::get_thumbprints() const {
	TypedArray<Ref<GDKXNetworkingThumbprint>> thumbprints;
    for (size_t i = 0; i < _info->thumbprintCount; i++) {
        Ref<GDKXNetworkingThumbprint> thumbprint = GDKXNetworkingThumbprint::create(&_info->thumbprints[i]);
        thumbprints.push_back(thumbprint);
    }
    return thumbprints;
}

void GDKXNetworkingConnectivityLevelHint::_bind_methods() {
    BIND_ENUM_CONSTANT(Unknown);
    BIND_ENUM_CONSTANT(None);
    BIND_ENUM_CONSTANT(LocalAccess);
    BIND_ENUM_CONSTANT(InternetAccess);
    BIND_ENUM_CONSTANT(ConstrainedInternetAccess);
}

void GDKXNetworkingConnectivityCostHint::_bind_methods() {
    BIND_ENUM_CONSTANT(Unknown);
    BIND_ENUM_CONSTANT(Unrestricted);
    BIND_ENUM_CONSTANT(Fixed);
    BIND_ENUM_CONSTANT(Variable);
}

void GDKXNetworkingConfigurationSetting::_bind_methods() {
    BIND_ENUM_CONSTANT(MaxTitleTcpQueuedReceiveBufferSize);
    BIND_ENUM_CONSTANT(MaxSystemTcpQueuedReceiveBufferSize);
    BIND_ENUM_CONSTANT(MaxToolsTcpQueuedReceiveBufferSize);
}

void GDKXNetworkingStatisticsType::_bind_methods() {
    BIND_ENUM_CONSTANT(TitleTcpQueuedReceivedBufferUsage);
    BIND_ENUM_CONSTANT(SystemTcpQueuedReceivedBufferUsage);
    BIND_ENUM_CONSTANT(ToolsTcpQueuedReceivedBufferUsage);
}

void GDKNetworking::_bind_methods() {
    ClassDB::bind_static_method(get_class_static(), D_METHOD("get_connectivity_hint"), &GDKNetworking::get_connectivity_hint);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("query_configuration_setting", "setting"), &GDKNetworking::query_configuration_setting);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("query_preferred_local_udp_multiplayer_port"), &GDKNetworking::query_preferred_local_udp_multiplayer_port);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("query_preferred_local_udp_multiplayer_port_async"), &GDKNetworking::query_preferred_local_udp_multiplayer_port_async);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("query_security_information_for_url_async", "url"), &GDKNetworking::query_security_information_for_url_async);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("query_security_information_for_url_utf16_async", "url"), &GDKNetworking::query_security_information_for_url_utf16_async);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("query_statistics", "type"), &GDKNetworking::query_statistics);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("set_configuration_setting", "setting", "value"), &GDKNetworking::set_configuration_setting);
}

Dictionary GDKNetworking::get_connectivity_hint() {
    XNetworkingConnectivityHint hint {};
    HRESULT hr = XNetworkingGetConnectivityHint(&hint);
    ERR_FAIL_COND_V_MSG(FAILED(hr), Dictionary(), vformat("XNetworkingGetConnectivityHint Error: 0x%08ux", (uint64_t)hr));

    Dictionary return_data;
    return_data["level_hint"] = (GDKXNetworkingConnectivityLevelHint::Enum)hint.connectivityLevel;
    return_data["cost_hint"] = (GDKXNetworkingConnectivityCostHint::Enum)hint.connectivityCost;
    return_data["iana_interface_type"] = (int64_t)hint.ianaInterfaceType;
    return_data["networking_initialized"] = hint.networkInitialized;
    return_data["approaching_data_limit"] = hint.approachingDataLimit;
    return_data["over_data_limit"] = hint.overDataLimit;
    return_data["roaming"] = hint.roaming;
	return return_data;
}

String GDKNetworking::query_configuration_setting(GDKXNetworkingConfigurationSetting::Enum setting) {
    uint64_t value = 0;
    HRESULT hr = XNetworkingQueryConfigurationSetting((XNetworkingConfigurationSetting)setting, &value);
    ERR_FAIL_COND_V_MSG(FAILED(hr), String(), vformat("XNetworkingQueryConfigurationSetting Error: 0x%08ux", (uint64_t)hr));
	return String::num_uint64(value);
}

int64_t GDKNetworking::query_preferred_local_udp_multiplayer_port() {
    uint16_t value = 0;
    HRESULT hr = XNetworkingQueryPreferredLocalUdpMultiplayerPort(&value);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XNetworkingQueryPreferredLocalUdpMultiplayerPort Error: 0x%08ux", (uint64_t)hr));
    return (int64_t)value;
}

Ref<GDKAsyncBlock> GDKNetworking::query_preferred_local_udp_multiplayer_port_async() {
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(GDKHelpers::get_async_queue());
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        Dictionary return_data;
        uint16_t value = 0;
        HRESULT hr = XNetworkingQueryPreferredLocalUdpMultiplayerPortAsyncResult(async, &value);
        return_data["hresult"] = hr;

        if (SUCCEEDED(hr)) {
            return_data["port"] = (int64_t)value;
        }
        wrapper->emit(return_data);
    });

    HRESULT hr = XNetworkingQueryPreferredLocalUdpMultiplayerPortAsync(asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XNetworkingQueryPreferredLocalUdpMultiplayerPortAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

Ref<GDKAsyncBlock> GDKNetworking::query_security_information_for_url_async(const String &url) {
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(GDKHelpers::get_async_queue());
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        Dictionary return_data;

        size_t bufferSize;
        HRESULT hr = XNetworkingQuerySecurityInformationForUrlAsyncResultSize(async, &bufferSize);
        return_data["hresult"] = hr;
        if (FAILED(hr)) {
            wrapper->emit(return_data);
            return;
        }

        Ref<GDKXNetworkingSecurityInformation> info = GDKXNetworkingSecurityInformation::create(bufferSize);
        hr = XNetworkingQuerySecurityInformationForUrlAsyncResult(async, bufferSize, nullptr, info->buffer_ptrw(), info->info_ptrw());
        if (SUCCEEDED(hr)) {
            return_data["security_info"] = info; 
        }

        wrapper->emit(return_data);
    });

    HRESULT hr = XNetworkingQuerySecurityInformationForUrlAsync(url.utf8().get_data(), asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XNetworkingQuerySecurityInformationForUrlAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

Ref<GDKAsyncBlock> GDKNetworking::query_security_information_for_url_utf16_async(const String &url) {
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(GDKHelpers::get_async_queue());
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        Dictionary return_data;

        size_t bufferSize;
        HRESULT hr = XNetworkingQuerySecurityInformationForUrlUtf16AsyncResultSize(async, &bufferSize);
        return_data["hresult"] = hr;
        if (FAILED(hr)) {
            wrapper->emit(return_data);
            return;
        }

        Ref<GDKXNetworkingSecurityInformation> info = GDKXNetworkingSecurityInformation::create(bufferSize);
        hr = XNetworkingQuerySecurityInformationForUrlUtf16AsyncResult(async, bufferSize, nullptr, info->buffer_ptrw(), info->info_ptrw());
        if (SUCCEEDED(hr)) {
            return_data["security_info"] = info; 
        }

        wrapper->emit(return_data);
    });

    HRESULT hr = XNetworkingQuerySecurityInformationForUrlUtf16Async(url.wide_string().get_data(), asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XNetworkingQuerySecurityInformationForUrlUtf16Async Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

Dictionary GDKNetworking::query_statistics(GDKXNetworkingStatisticsType::Enum type) {
	XNetworkingStatisticsBuffer buffer {};
    Dictionary return_data;
    HRESULT hr = XNetworkingQueryStatistics((XNetworkingStatisticsType)type, &buffer);
    return_data["hresult"] = hr;
    ERR_FAIL_COND_V_MSG(FAILED(hr), return_data, vformat("XNetworkingQueryStatistics Error: 0x%08ux", (uint64_t)hr));
    
    Dictionary stat_buffer;
    stat_buffer["num_bytes_currently_queued"] = String::num_uint64(buffer.tcpQueuedReceiveBufferUsage.numBytesCurrentlyQueued);
    stat_buffer["peak_num_bytes_ever_queued"] = String::num_uint64(buffer.tcpQueuedReceiveBufferUsage.peakNumBytesEverQueued);
    stat_buffer["total_num_bytes_queued"] = String::num_uint64(buffer.tcpQueuedReceiveBufferUsage.totalNumBytesQueued);
    stat_buffer["num_bytes_dropped_for_exceeding_configured_max"] = String::num_uint64(buffer.tcpQueuedReceiveBufferUsage.numBytesDroppedForExceedingConfiguredMax);
    stat_buffer["num_bytes_dropped_due_to_any_failure"] = String::num_uint64(buffer.tcpQueuedReceiveBufferUsage.numBytesDroppedDueToAnyFailure);

    return_data["statistics_buffer"] = stat_buffer;
    return return_data;
}

int GDKNetworking::set_configuration_setting(GDKXNetworkingConfigurationSetting::Enum setting, const String& int64_string) {
    uint64_t value = (uint64_t)int64_string.to_int();
	HRESULT hr = XNetworkingSetConfigurationSetting((XNetworkingConfigurationSetting)setting, value);
    ERR_FAIL_COND_V_MSG(FAILED(hr), 0, vformat("XNetworkingSetConfigurationSetting Error: 0x%08ux", (uint64_t)hr));
    return (int64_t)hr;
}

GDKNetworkingEvents* GDKNetworkingEvents::_instance = nullptr;

void GDKNetworkingEvents::_bind_methods() {
    ADD_SIGNAL(MethodInfo("connectivity_hint_changed", PropertyInfo(Variant::DICTIONARY, "hint")));
    ADD_SIGNAL(MethodInfo("preferred_multiplayer_port_changed", PropertyInfo(Variant::INT, "port")));
}

void GDKNetworkingEvents::_notification(int p_what) {
    if (p_what == NOTIFICATION_PREDELETE) {
        if (_hint_change_token.token != 0) {
            XNetworkingUnregisterConnectivityHintChanged(_hint_change_token, false);
        }

        if (_port_change_token.token != 0) {
            XNetworkingUnregisterPreferredLocalUdpMultiplayerPortChanged(_port_change_token, false);
        }

        if (_async_queue != nullptr) {
            XTaskQueueCloseHandle(_async_queue);
        }
    }
}

GDKNetworkingEvents::GDKNetworkingEvents() {
	HRESULT hr = XTaskQueueCreate(XTaskQueueDispatchMode::Immediate, XTaskQueueDispatchMode::Immediate, &_async_queue);
    ERR_FAIL_COND_MSG(FAILED(hr), vformat("GDKNetworkingEvents::XTaskQueueCreate Error: 0x%08ux", (uint64_t)hr));
    
    hr = XNetworkingRegisterConnectivityHintChanged(_async_queue, this, 
            [](void* context, const XNetworkingConnectivityHint* hint) {
                GDKNetworkingEvents* self = static_cast<GDKNetworkingEvents*>(context);
                Dictionary return_data;
                return_data["level_hint"] = (GDKXNetworkingConnectivityLevelHint::Enum)hint->connectivityLevel;
                return_data["cost_hint"] = (GDKXNetworkingConnectivityCostHint::Enum)hint->connectivityCost;
                return_data["iana_interface_type"] = (int64_t)hint->ianaInterfaceType;
                return_data["networking_initialized"] = hint->networkInitialized;
                return_data["approaching_data_limit"] = hint->approachingDataLimit;
                return_data["over_data_limit"] = hint->overDataLimit;
                return_data["roaming"] = hint->roaming;
    
                self->call_deferred("emit_signal", "connectivity_hint_changed", return_data);
            }, &_hint_change_token);
    
    if (FAILED(hr)) {
        ERR_PRINT(vformat("GDKNetworkingEvents::XNetworkingRegisterConnectivityHintChanged Error: 0x%08ux", (uint64_t)hr));
    }
    
    hr = XNetworkingRegisterPreferredLocalUdpMultiplayerPortChanged(_async_queue, this,
            [](void* context, uint16_t port){
                GDKNetworkingEvents* self = static_cast<GDKNetworkingEvents*>(context);
                self->call_deferred("emit_signal", "preferred_multiplayer_port_changed", (int64_t)port);
            }, &_port_change_token);
    
    if (FAILED(hr)) {
        ERR_PRINT(vformat("GDKNetworkingEvents::XNetworkingRegisterPreferredLocalUdpMultiplayerPortChanged Error: 0x%08ux", (uint64_t)hr));
    }
}

GDKNetworkingEvents* GDKNetworkingEvents::get_singleton() {
	if (_instance) {
        return _instance;
    }

    _instance = memnew(GDKNetworkingEvents);
    return _instance;
}
