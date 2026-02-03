#pragma once

#include <classes/ref_counted.hpp>
#include <windows.h>
#include <XUser.h>
#include <XTaskQueue.h>


namespace godot {
class GDKHelpers : public RefCounted {
    GDCLASS(GDKHelpers, RefCounted)

protected:
    static void _bind_methods();

public:
    static XTaskQueueHandle get_async_queue();
};
}