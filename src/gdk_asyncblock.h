#ifndef GDK_ASYNCBLOCK_H
#define GDK_ASYNCBLOCK_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <Windows.h>
#include <XAsync.h>

namespace godot {
class gdk_asyncBlock : public RefCounted {
    GDCLASS(gdk_asyncBlock, RefCounted);

private:
    XAsyncBlock* _block = nullptr;

protected:
    static void _bind_methods();
    virtual void _notification(int p_what);

public:
    gdk_asyncBlock();
    static Ref<gdk_asyncBlock> create(XTaskQueueHandle queue);

    inline XAsyncBlock* get_block() { return _block; }
    void set_callback(XAsyncCompletionRoutine* callback);
    void emit(Dictionary data);
};
}



#endif