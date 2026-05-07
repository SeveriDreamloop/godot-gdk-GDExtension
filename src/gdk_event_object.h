#pragma once

#include <classes/ref_counted.hpp>
#include <Windows.h>
#include <classes/engine.hpp>

extern "C" {
    #include <XTaskQueue.h>
}

namespace godot {
class GDKEventObject: public RefCounted {
    GDCLASS(GDKEventObject, RefCounted);

protected:
    static void _bind_methods();

public:
    virtual HRESULT initialize();
    virtual StringName get_singleton_name() const = 0;
};
}