#pragma once

#include <classes/ref_counted.hpp>
#include "gdk_user.h"

namespace godot {
class GDKEvents: public RefCounted {
    GDCLASS(GDKEvents, RefCounted);

protected:
    static void _bind_methods();

public:
    static int write_in_game_event(Ref<GDKUser> user, const String& event_name, const Dictionary& dimensions, const Dictionary& measurements);
    static int set_max_file_size(int64_t max_file_size);
    static int set_storage_allotment(int64_t storage_allotment);
};
}