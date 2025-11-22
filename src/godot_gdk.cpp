#include "godot_gdk.h"

#include <Windows.h>
#include <winapifamily.h>
#include <objbase.h>
#include <XGameRuntimeInit.h>
#include <XTaskQueue.h>
#include <xsapi-c/services_c.h>
#include <XUser.h>

#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

static XTaskQueueHandle queue;
static XUserLocalId xboxUserId;
static XUserHandle xboxUserHandle;
static godot::Callable* callback;

void godot_gdk::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &godot_gdk::print_type);
	godot::ClassDB::bind_method(D_METHOD("InitializeGDK"), &godot_gdk::InitializeGDK);
}

void godot_gdk::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}

int godot_gdk::InitializeGDK(godot::Callable cb) {
	//Take the callable to keep reference
	stored = cb;
	//Reference pointer in static context so it's callable from GDKs callback
	callback = &stored;

	HRESULT hr = XGameRuntimeInitialize();
	if (!CheckResult(hr,"Successfully initialized GDK", "Failed to initialize Xbox Game Runtime (GDK).")) {
		return hr;
	}


	XblInitArgs xblArgs = {};
	xblArgs.queue = queue;
	xblArgs.scid = "00000000-0000-0000-0000-000000000000";

	hr = XblInitialize(&xblArgs);
	if (!CheckResult(hr, "Successfully initialized xbox services", "Failed to initialize Xbox services"))
	{
		return hr;
	}

	queue = XblGetAsyncQueue();

	hr = Identity_TrySignInDefaultUserSilently(queue);
	CheckResult(hr, "Login successfully started", "Failed to start login");

	return hr;
}


HRESULT godot_gdk::Identity_TrySignInDefaultUserSilently(_In_ XTaskQueueHandle asyncQueue) {
	XAsyncBlock* asyncBlock = new XAsyncBlock();
	asyncBlock->queue = asyncQueue;
	asyncBlock->callback = Identity_TrySignInDefaultUserSilently_Callback;

	// Request to silently sign in the default user.
	HRESULT hr = XUserAddAsync(XUserAddOptions::AddDefaultUserSilently, asyncBlock);

	if (!CheckResult(hr, "Successfully started login", "Failed to start login"))
	{
		delete asyncBlock;
	}

	return hr;
}

void CALLBACK godot_gdk::Identity_TrySignInDefaultUserSilently_Callback(_In_ XAsyncBlock* asyncBlock)
{
	HRESULT hr = XUserAddResult(asyncBlock, &xboxUserHandle);

	if (!CheckResult(hr, "Login successful", "Login failed")) {
		delete asyncBlock;
		return;
	}

	XblContextHandle handle;

	if (!CreateContextHandle(&handle)) {
		delete asyncBlock;
		return;
	}

	hr = XUserGetLocalId(xboxUserHandle, &xboxUserId);

	if (!CheckResult(hr, "Successfully retrieved userid", "Failed to retrieve userid")) {
		delete asyncBlock;
		return;
	}

	if (callback && callback->is_valid()) {
		callback->call();
	}
}

bool godot_gdk::CreateContextHandle(XblContextHandle* handle) {
	HRESULT hr = XblContextCreateHandle(xboxUserHandle, handle);

	return CheckResult(hr, "Successfully created handle", "Creating handle failed");
}

XTaskQueueHandle godot_gdk::GetQueueHandle() {
	return queue;
}

XUserLocalId  godot_gdk::GetUserId() {
	return xboxUserId;
}

XUserHandle godot_gdk::GetUserHandle() {
	return xboxUserHandle;
}

XAsyncBlock* godot_gdk::CreateAsyncBlock() {
	XAsyncBlock* asyncBlock = new XAsyncBlock();
	XTaskQueueHandle queue_handle = godot_gdk::GetQueueHandle();
	asyncBlock->queue = queue_handle;
	return asyncBlock;
}

bool godot_gdk::CheckResult(HRESULT result, std::string succeedMessage, std::string errorMessage) {
	if (FAILED(result)) {
		std::ostringstream oss;
		oss << errorMessage
			<< " HRESULT=0x"
			<< std::hex << std::setw(8) << std::setfill('0') << result;
		ERR_PRINT(oss.str().c_str());
		return false;
	}

	print_line(succeedMessage.c_str());

	return true;
}
