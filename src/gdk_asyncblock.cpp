#include "gdk_asyncblock.h"

using namespace godot;

gdk_asyncBlock::gdk_asyncBlock() {
    _block = new XAsyncBlock();
    ZeroMemory(_block, sizeof(XAsyncBlock));

    _block->context = this;
}

void gdk_asyncBlock::_bind_methods() {
    ADD_SIGNAL(MethodInfo("completed", PropertyInfo(Variant::DICTIONARY, "result")));
}

void gdk_asyncBlock::_notification(int p_what) {
    if (p_what == NOTIFICATION_PREDELETE && _block) {
        delete _block;
        _block = nullptr;
    }
}

Ref<gdk_asyncBlock> gdk_asyncBlock::create(XTaskQueueHandle queue) {
    Ref<gdk_asyncBlock> wrapper;
    wrapper.instantiate();

    XAsyncBlock* b = wrapper->_block;
    b->queue = queue;
    return wrapper;
}

void gdk_asyncBlock::set_callback(XAsyncCompletionRoutine* callback) {
    _block->callback = callback;
}

void gdk_asyncBlock::emit(Dictionary data) {
    emit_signal("completed", data);
}