#pragma once

#include <Windows.h>
#include <winapifamily.h>
#include <objbase.h>
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <XAsync.h>
#include <XTaskQueue.h>
#include <XUser.h>
#include <xsapi-c/types_c.h>

#include <string>

using namespace godot;

class godot_gdk : public RefCounted {
	GDCLASS(godot_gdk, RefCounted)

	godot::Callable stored;

private:
	static HRESULT Identity_TrySignInDefaultUserSilently(XTaskQueueHandle asyncQueue);
	static void Identity_TrySignInDefaultUserSilently_Callback(XAsyncBlock *asyncBlock);

protected:
	static void _bind_methods();

public:
	godot_gdk() = default;
	~godot_gdk() override = default;

	void print_type(const Variant &p_variant) const;
	int InitializeGDK(godot::Callable);
	static bool CreateContextHandle(XblContextHandle* handle);
	static XTaskQueueHandle GetQueueHandle();

	static XUserLocalId GetUserId();
	static XUserHandle GetUserHandle();
	static XAsyncBlock* godot_gdk::CreateAsyncBlock();
	static bool CheckResult(HRESULT result, std::string succeedMessage, std::string errorMessage);
};
