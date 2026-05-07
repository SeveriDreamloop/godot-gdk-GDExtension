#include "gdk_events.h"

extern "C" {
    #include <xsapi-c/events_c.h>
}

#include <classes/json.hpp>

using namespace godot;

void GDKEvents::_bind_methods() {
    ClassDB::bind_static_method(get_class_static(), D_METHOD("write_in_game_event", "user", "event_name", "dimensions", "measurements"), &GDKEvents::write_in_game_event);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("set_max_file_size", "max_file_size"), &GDKEvents::set_max_file_size);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("set_storage_allotment", "storage_allotment"), &GDKEvents::set_storage_allotment);
}

int GDKEvents::write_in_game_event(Ref<GDKUser> user, const String &event_name, const Dictionary& dimensions, const Dictionary& measurements) {
    HRESULT hr = S_OK;
#ifdef XSAPI_EVENTS_SERVICE
    const char* dimension_json_string = JSON::stringify(dimensions).utf8().get_data();
    const char* measurements_json_string = JSON::stringify(measurements).utf8().get_data();
	hr = XblEventsWriteInGameEvent(user->get_xbox_live_context(), event_name.utf8().get_data(), dimension_json_string, measurements_json_string);
    ERR_FAIL_COND_V_MSG(FAILED(hr), (int64_t)hr, vformat("XblEventsWriteInGameEvent Error: 0x%08ux", (uint64_t)hr));
#endif
    return (int64_t)hr;
}

int GDKEvents::set_max_file_size(int64_t max_file_size) {
    HRESULT hr = S_OK;
#ifdef XSAPI_INTERNAL_EVENTS_SERVICE
	hr = XblEventsSetMaxFileSize((uint64_t)max_file_size);
    ERR_FAIL_COND_V_MSG(FAILED(hr), (int64_t)hr, vformat("XblEventsSetMaxFileSize Error: 0x%08ux", (uint64_t)hr));

#else
    hr = E_NOTIMPL;
#endif
    return (int64_t)hr;
}

int GDKEvents::set_storage_allotment(int64_t storage_allotment) {
    HRESULT hr = S_OK;
#ifdef XSAPI_INTERNAL_EVENTS_SERVICE
	hr = XblEventsSetStorageAllotment((uint64_t)storage_allotment);
    ERR_FAIL_COND_V_MSG(FAILED(hr), (int64_t)hr, vformat("XblEventsSetStorageAllotment Error: 0x%08ux", (uint64_t)hr));
#else
    hr = E_NOTIMPL;
#endif
    return (int64_t)hr;
}
