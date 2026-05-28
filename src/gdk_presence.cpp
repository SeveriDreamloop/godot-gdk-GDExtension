#include "gdk_presence.h"

#include "gdk_helpers.h"
#include "godot_gdk.h"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include <Windows.h>
#include <winapifamily.h>
#include <objbase.h>
#include <XGameRuntimeInit.h>
#include <XTaskQueue.h>
#include <xsapi-c/services_c.h>

#include <vector>

using namespace godot;

// --- Enum Bindings ---

void GDKXblPresenceUserState::_bind_methods() {
	BIND_ENUM_CONSTANT(Unknown);
	BIND_ENUM_CONSTANT(Online);
	BIND_ENUM_CONSTANT(Away);
	BIND_ENUM_CONSTANT(Offline);
}

void GDKXblPresenceDeviceType::_bind_methods() {
	BIND_ENUM_CONSTANT(Unknown);
	BIND_ENUM_CONSTANT(WindowsPhone);
	BIND_ENUM_CONSTANT(WindowsPhone7);
	BIND_ENUM_CONSTANT(Web);
	BIND_ENUM_CONSTANT(Xbox360);
	BIND_ENUM_CONSTANT(PC);
	BIND_ENUM_CONSTANT(Windows8);
	BIND_ENUM_CONSTANT(XboxOne);
	BIND_ENUM_CONSTANT(WindowsOneCore);
	BIND_ENUM_CONSTANT(WindowsOneCoreMobile);
	BIND_ENUM_CONSTANT(iOS);
	BIND_ENUM_CONSTANT(Android);
	BIND_ENUM_CONSTANT(AppleTV);
	BIND_ENUM_CONSTANT(Nintendo);
	BIND_ENUM_CONSTANT(PlayStation);
	BIND_ENUM_CONSTANT(Win32);
	BIND_ENUM_CONSTANT(Scarlett);
}

void GDKXblPresenceTitleState::_bind_methods() {
	BIND_ENUM_CONSTANT(Unknown);
	BIND_ENUM_CONSTANT(Started);
	BIND_ENUM_CONSTANT(Ended);
}

void GDKXblPresenceDetailLevel::_bind_methods() {
	BIND_ENUM_CONSTANT(Default);
	BIND_ENUM_CONSTANT(User);
	BIND_ENUM_CONSTANT(Device);
	BIND_ENUM_CONSTANT(Title);
	BIND_ENUM_CONSTANT(All);
}

void GDKXblPresenceFilter::_bind_methods() {
	BIND_ENUM_CONSTANT(Unknown);
	BIND_ENUM_CONSTANT(TitleOnline);
	BIND_ENUM_CONSTANT(TitleOffline);
	BIND_ENUM_CONSTANT(TitleOnlineOutsideTitle);
	BIND_ENUM_CONSTANT(AllOnline);
	BIND_ENUM_CONSTANT(AllOffline);
	BIND_ENUM_CONSTANT(AllTitle);
	BIND_ENUM_CONSTANT(All);
}

// --- GDKPresence ---

void GDKPresence::_bind_methods() {
	ClassDB::bind_static_method("GDKPresence", D_METHOD("create", "user"), &GDKPresence::create);
	ClassDB::bind_method(D_METHOD("set_presence_async", "is_active", "rich_presence_id"), &GDKPresence::set_presence_async);
	ClassDB::bind_method(D_METHOD("get_presence_async", "xuid"), &GDKPresence::get_presence_async);
	ClassDB::bind_method(D_METHOD("get_presence_for_multiple_users_async", "xuids", "filter", "detail_level"), &GDKPresence::get_presence_for_multiple_users_async);
	ClassDB::bind_method(D_METHOD("get_presence_for_social_group_async", "social_group", "filter", "detail_level"), &GDKPresence::get_presence_for_social_group_async);

	ADD_SIGNAL(MethodInfo("device_presence_changed",
		PropertyInfo(Variant::INT, "xuid"),
		PropertyInfo(Variant::INT, "device_type"),
		PropertyInfo(Variant::BOOL, "is_user_logged_on")));

	ADD_SIGNAL(MethodInfo("title_presence_changed",
		PropertyInfo(Variant::INT, "xuid"),
		PropertyInfo(Variant::INT, "title_id"),
		PropertyInfo(Variant::INT, "title_state")));
}

Ref<GDKPresence> GDKPresence::create(Ref<GDKUser> user) {
	Ref<GDKPresence> wrapper;
	if (user != nullptr) {
		wrapper.instantiate();
		wrapper->_user = user;

		XblContextHandle handle;
		if (FAILED(XblContextCreateHandle(GodotGDK::GetUserHandle(), &handle))) {
			return wrapper;
		}

		wrapper->_device_presence_change_context = XblPresenceAddDevicePresenceChangedHandler(handle,
			[](void* context, uint64_t xuid, XblPresenceDeviceType deviceType, bool isUserLoggedOnDevice) {
				GDKPresence* presence = static_cast<GDKPresence*>(context);
				presence->call_deferred("emit_signal", "device_presence_changed",
					(int64_t)xuid, (int)deviceType, isUserLoggedOnDevice);
			}, wrapper.ptr());

		wrapper->_title_presence_change_context = XblPresenceAddTitlePresenceChangedHandler(handle,
			[](void* context, uint64_t xuid, uint32_t titleId, XblPresenceTitleState titleState) {
				GDKPresence* presence = static_cast<GDKPresence*>(context);
				presence->call_deferred("emit_signal", "title_presence_changed",
					(int64_t)xuid, (int)titleId, (int)titleState);
			}, wrapper.ptr());
	}
	return wrapper;
}

void GDKPresence::_notification(int p_what) {
	if (p_what == NOTIFICATION_PREDELETE) {
		XblContextHandle handle;
		if (FAILED(XblContextCreateHandle(GodotGDK::GetUserHandle(), &handle))) {
			return;
		}

		if (_device_presence_change_context) {
			XblPresenceRemoveDevicePresenceChangedHandler(handle, _device_presence_change_context);
		}
		if (_title_presence_change_context) {
			XblPresenceRemoveTitlePresenceChangedHandler(handle, _title_presence_change_context);
		}
	}
}

Ref<GDKAsyncBlock> GDKPresence::set_presence_async(bool is_active, String rich_presence_id) {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);

	asyncBlock->set_callback([](XAsyncBlock* asyncBlock) {
		GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(asyncBlock->context);

		HRESULT hr = XAsyncGetStatus(asyncBlock, false);
		Dictionary return_data;
		return_data["hresult"] = hr;
		wrapper->emit(return_data);
	});

	XblContextHandle handle;
	if (FAILED(XblContextCreateHandle(GodotGDK::GetUserHandle(), &handle))) {
		return asyncBlock;
	}

	XblPresenceRichPresenceIds richPresenceIds{};
	strncpy(richPresenceIds.scid, GodotGDK::GetSCID(), sizeof(richPresenceIds.scid) - 1);
	richPresenceIds.scid[sizeof(richPresenceIds.scid) - 1] = '\0';

	CharString presence_utf8 = rich_presence_id.utf8();
	richPresenceIds.presenceId = presence_utf8.get_data();

	HRESULT hr = XblPresenceSetPresenceAsync(handle, is_active, &richPresenceIds, asyncBlock->get_block());
	GodotGDK::CheckResult(hr, "Successfully started setting presence", "Failed to start setting presence");

	return asyncBlock;
}

Ref<GDKAsyncBlock> GDKPresence::get_presence_async(int64_t xuid) {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);

	asyncBlock->set_callback([](XAsyncBlock* asyncBlock) {
		GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(asyncBlock->context);

		XblPresenceRecordHandle recordHandle;
		HRESULT hr = XblPresenceGetPresenceResult(asyncBlock, &recordHandle);

		Dictionary return_data;
		if (GodotGDK::CheckResult(hr, "", "Failed to retrieve presence")) {
			return_data["result"] = Ref<GDKPresenceRecord>(memnew(GDKPresenceRecord(recordHandle)));
			XblPresenceRecordCloseHandle(recordHandle);
		}

		return_data["hresult"] = hr;
		wrapper->emit(return_data);
	});

	XblContextHandle handle;
	if (FAILED(XblContextCreateHandle(GodotGDK::GetUserHandle(), &handle))) {
		return asyncBlock;
	}

	HRESULT hr = XblPresenceGetPresenceAsync(handle, static_cast<uint64_t>(xuid), asyncBlock->get_block());
	GodotGDK::CheckResult(hr, "Successfully started retrieving presence", "Failed to start retrieving presence");

	return asyncBlock;
}

Ref<GDKAsyncBlock> GDKPresence::get_presence_for_multiple_users_async(PackedInt64Array xuids, GDKXblPresenceFilter::Enum filter, GDKXblPresenceDetailLevel::Enum detail_level) {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);

	asyncBlock->set_callback([](XAsyncBlock* asyncBlock) {
		GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(asyncBlock->context);

		XblPresenceRecordHandle* recordHandles = nullptr;
		size_t recordsCount = 0;
		HRESULT hr = XblPresenceGetPresenceForMultipleUsersResultCount(asyncBlock, &recordsCount);

		Dictionary return_data;
		if (GodotGDK::CheckResult(hr, "", "Failed to get presence result count")) {
			recordHandles = new XblPresenceRecordHandle[recordsCount];
			hr = XblPresenceGetPresenceForMultipleUsersResult(asyncBlock, recordHandles, recordsCount);

			if (GodotGDK::CheckResult(hr, "", "Failed to retrieve presence for multiple users")) {
				TypedArray<GDKPresenceRecord> records;
				for (size_t i = 0; i < recordsCount; i++) {
					records.push_back(Ref<GDKPresenceRecord>(memnew(GDKPresenceRecord(recordHandles[i]))));
					XblPresenceRecordCloseHandle(recordHandles[i]);
				}
				return_data["result"] = records;
			}
			delete[] recordHandles;
		}

		return_data["hresult"] = hr;
		wrapper->emit(return_data);
	});

	XblContextHandle handle;
	if (FAILED(XblContextCreateHandle(GodotGDK::GetUserHandle(), &handle))) {
		return asyncBlock;
	}

	std::vector<uint64_t> xuid_array;
	for (int i = 0; i < xuids.size(); i++) {
		xuid_array.push_back(static_cast<uint64_t>(xuids[i]));
	}

	(void)filter;
	XblPresenceQueryFilters filters{};
	filters.detailLevel = static_cast<XblPresenceDetailLevel>(detail_level);

	HRESULT hr = XblPresenceGetPresenceForMultipleUsersAsync(handle, xuid_array.data(), xuid_array.size(), &filters, asyncBlock->get_block());
	GodotGDK::CheckResult(hr, "Successfully started retrieving presence for multiple users", "Failed to start retrieving presence for multiple users");

	return asyncBlock;
}

Ref<GDKAsyncBlock> GDKPresence::get_presence_for_social_group_async(String social_group, GDKXblPresenceFilter::Enum filter, GDKXblPresenceDetailLevel::Enum detail_level) {
	XTaskQueueHandle queue = GDKHelpers::get_async_queue();
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(queue);

	asyncBlock->set_callback([](XAsyncBlock* asyncBlock) {
		GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(asyncBlock->context);

		XblPresenceRecordHandle* recordHandles = nullptr;
		size_t recordsCount = 0;
		HRESULT hr = XblPresenceGetPresenceForSocialGroupResultCount(asyncBlock, &recordsCount);

		Dictionary return_data;
		if (GodotGDK::CheckResult(hr, "", "Failed to get social group presence result count")) {
			recordHandles = new XblPresenceRecordHandle[recordsCount];
			hr = XblPresenceGetPresenceForSocialGroupResult(asyncBlock, recordHandles, recordsCount);

			if (GodotGDK::CheckResult(hr, "", "Failed to retrieve presence for social group")) {
				TypedArray<GDKPresenceRecord> records;
				for (size_t i = 0; i < recordsCount; i++) {
					records.push_back(Ref<GDKPresenceRecord>(memnew(GDKPresenceRecord(recordHandles[i]))));
					XblPresenceRecordCloseHandle(recordHandles[i]);
				}
				return_data["result"] = records;
			}
			delete[] recordHandles;
		}

		return_data["hresult"] = hr;
		wrapper->emit(return_data);
	});

	XblContextHandle handle;
	if (FAILED(XblContextCreateHandle(GodotGDK::GetUserHandle(), &handle))) {
		return asyncBlock;
	}

	(void)filter;
	XblPresenceQueryFilters filters{};
	filters.detailLevel = static_cast<XblPresenceDetailLevel>(detail_level);

	CharString social_group_utf8 = social_group.utf8();
	HRESULT hr = XblPresenceGetPresenceForSocialGroupAsync(handle, social_group_utf8.get_data(), nullptr, &filters, asyncBlock->get_block());
	GodotGDK::CheckResult(hr, "Successfully started retrieving presence for social group", "Failed to start retrieving presence for social group");

	return asyncBlock;
}
