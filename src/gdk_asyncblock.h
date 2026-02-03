#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <Windows.h>
#include <XAsync.h>

namespace godot {

class GDKAsyncBlock : public RefCounted {
    GDCLASS(GDKAsyncBlock, RefCounted);

private:
    XAsyncBlock* _block = nullptr;

protected:
    static void _bind_methods();
    virtual void _notification(int p_what);

public:
    GDKAsyncBlock();
    static Ref<GDKAsyncBlock> create(XTaskQueueHandle queue);

    inline XAsyncBlock* get_block() { return _block; }
    void set_callback(XAsyncCompletionRoutine* callback);
    void emit(Dictionary data);
};

}
