#pragma once

#include "gdk_asyncblock.h"
#include "gdk_user.h"

#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include <Windows.h>
#include <winapifamily.h>
#include <objbase.h>
#include <XAsync.h>
#include <xsapi-c/services_c.h>

namespace godot {

// --- Enum Wrappers ---

class GDKXblPresenceUserState : public Object {
	GDCLASS(GDKXblPresenceUserState, Object);

protected:
	static void _bind_methods();

public:
	enum Enum : uint32_t {
		Unknown = 0,
		Online = 1,
		Away = 2,
		Offline = 3
	};
};

class GDKXblPresenceDeviceType : public Object {
	GDCLASS(GDKXblPresenceDeviceType, Object);

protected:
	static void _bind_methods();

public:
	enum Enum : uint32_t {
		Unknown,  
		WindowsPhone,  
		WindowsPhone7,  
		Web,  
		Xbox360,  
		PC,  
		Windows8,  
		XboxOne,  
		WindowsOneCore,  
		WindowsOneCoreMobile,  
		iOS,  
		Android,  
		AppleTV,  
		Nintendo,  
		PlayStation,  
		Win32,  
		Scarlett
	};
};

class GDKXblPresenceTitleState : public Object {
	GDCLASS(GDKXblPresenceTitleState, Object);

protected:
	static void _bind_methods();

public:
	enum Enum : uint32_t {
		Unknown = 0,
		Started = 1,
		Ended = 2
	};
};

class GDKXblPresenceFilter : public Object {
	GDCLASS(GDKXblPresenceFilter, Object);

protected:
	static void _bind_methods();

public:
	enum Enum : uint32_t {
		Unknown = 0,
		TitleOnline = 1,
		TitleOffline = 2,
		TitleOnlineOutsideTitle = 3,
		AllOnline = 4,
		AllOffline = 5,
		AllTitle = 6,
		All = 7
	};
};

class GDKXblPresenceDetailLevel : public Object {
	GDCLASS(GDKXblPresenceDetailLevel, Object);

protected:
	static void _bind_methods();

public:
	enum Enum : uint32_t {
		Default = 0,
		User = 1,
		Device = 2,
		Title = 3,
		All = 4
	};
};

// --- Data Classes ---

class GDKPresenceTitleRecord : public RefCounted {
	GDCLASS(GDKPresenceTitleRecord, RefCounted);

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_title_id"), &GDKPresenceTitleRecord::get_title_id);
		ClassDB::bind_method(D_METHOD("get_title_name"), &GDKPresenceTitleRecord::get_title_name);
		ClassDB::bind_method(D_METHOD("get_rich_presence_string"), &GDKPresenceTitleRecord::get_rich_presence_string);
		ClassDB::bind_method(D_METHOD("get_is_title_active"), &GDKPresenceTitleRecord::get_is_title_active);

		ADD_PROPERTY(PropertyInfo(Variant::INT, "title_id"), "", "get_title_id");
		ADD_PROPERTY(PropertyInfo(Variant::STRING, "title_name"), "", "get_title_name");
		ADD_PROPERTY(PropertyInfo(Variant::STRING, "rich_presence_string"), "", "get_rich_presence_string");
		ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_title_active"), "", "get_is_title_active");
	}

public:
	GDKPresenceTitleRecord() = default;
	GDKPresenceTitleRecord(const XblPresenceTitleRecord& src) {
		title_id = src.titleId;
		title_name = src.titleName;
		rich_presence_string = src.richPresenceString;
		is_title_active = src.titleActive;
	}

	uint32_t title_id = 0;
	String title_name;
	String rich_presence_string;
	bool is_title_active = false;

	uint32_t get_title_id() const { return title_id; }
	String get_title_name() const { return title_name; }
	String get_rich_presence_string() const { return rich_presence_string; }
	bool get_is_title_active() const { return is_title_active; }
};

class GDKPresenceDeviceRecord : public RefCounted {
	GDCLASS(GDKPresenceDeviceRecord, RefCounted);

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_device_type"), &GDKPresenceDeviceRecord::get_device_type);
		ClassDB::bind_method(D_METHOD("get_title_records"), &GDKPresenceDeviceRecord::get_title_records);

		ADD_PROPERTY(PropertyInfo(Variant::INT, "device_type"), "", "get_device_type");
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "title_records"), "", "get_title_records");
	}

public:
	GDKPresenceDeviceRecord() = default;
	GDKPresenceDeviceRecord(const XblPresenceDeviceRecord& src) {
		device_type = static_cast<GDKXblPresenceDeviceType::Enum>(src.deviceType);
		for (uint32_t i = 0; i < src.titleRecordsCount; i++) {
			title_records.push_back(Ref<GDKPresenceTitleRecord>(memnew(GDKPresenceTitleRecord(src.titleRecords[i]))));
		}
	}

	GDKXblPresenceDeviceType::Enum device_type = GDKXblPresenceDeviceType::Unknown;
	TypedArray<GDKPresenceTitleRecord> title_records;

	GDKXblPresenceDeviceType::Enum get_device_type() const { return device_type; }
	TypedArray<GDKPresenceTitleRecord> get_title_records() const { return title_records; }
};

class GDKPresenceRecord : public RefCounted {
	GDCLASS(GDKPresenceRecord, RefCounted);

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_xuid"), &GDKPresenceRecord::get_xuid);
		ClassDB::bind_method(D_METHOD("get_user_state"), &GDKPresenceRecord::get_user_state);
		ClassDB::bind_method(D_METHOD("get_device_records"), &GDKPresenceRecord::get_device_records);

		ADD_PROPERTY(PropertyInfo(Variant::INT, "xuid"), "", "get_xuid");
		ADD_PROPERTY(PropertyInfo(Variant::INT, "user_state"), "", "get_user_state");
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "device_records"), "", "get_device_records");
	}

public:
	GDKPresenceRecord() = default;
	GDKPresenceRecord(XblPresenceRecordHandle handle) {
		uint64_t xbox_user_id = 0;
		if (SUCCEEDED(XblPresenceRecordGetXuid(handle, &xbox_user_id))) {
			xuid = static_cast<int64_t>(xbox_user_id);
		}

		XblPresenceUserState state = XblPresenceUserState::Unknown;
		if (SUCCEEDED(XblPresenceRecordGetUserState(handle, &state))) {
			user_state = static_cast<GDKXblPresenceUserState::Enum>(state);
		}

		const XblPresenceDeviceRecord* deviceRecords = nullptr;
		size_t deviceRecordsCount = 0;
		if (SUCCEEDED(XblPresenceRecordGetDeviceRecords(handle, &deviceRecords, &deviceRecordsCount))) {
			for (size_t i = 0; i < deviceRecordsCount; i++) {
				device_records.push_back(Ref<GDKPresenceDeviceRecord>(memnew(GDKPresenceDeviceRecord(deviceRecords[i]))));
			}
		}
	}

	int64_t xuid = 0;
	GDKXblPresenceUserState::Enum user_state = GDKXblPresenceUserState::Unknown;
	TypedArray<GDKPresenceDeviceRecord> device_records;

	int64_t get_xuid() const { return xuid; }
	GDKXblPresenceUserState::Enum get_user_state() const { return user_state; }
	TypedArray<GDKPresenceDeviceRecord> get_device_records() const { return device_records; }
};

// --- Service Class ---

class GDKPresence : public RefCounted {
	GDCLASS(GDKPresence, RefCounted)

	Ref<GDKUser> _user;
	XblFunctionContext _device_presence_change_context = 0;
	XblFunctionContext _title_presence_change_context = 0;
	void _notification(int p_what);

protected:
	static void _bind_methods();

public:
	GDKPresence() = default;
	~GDKPresence() override = default;

	static Ref<GDKPresence> create(Ref<GDKUser> user);

	Ref<GDKAsyncBlock> set_presence_async(bool is_active, String rich_presence_id);
	Ref<GDKAsyncBlock> get_presence_async(int64_t xuid);
	Ref<GDKAsyncBlock> get_presence_for_multiple_users_async(PackedInt64Array xuids, GDKXblPresenceFilter::Enum filter, GDKXblPresenceDetailLevel::Enum detail_level);
	Ref<GDKAsyncBlock> get_presence_for_social_group_async(String social_group, GDKXblPresenceFilter::Enum filter, GDKXblPresenceDetailLevel::Enum detail_level);
};
}

VARIANT_ENUM_CAST(GDKXblPresenceUserState::Enum);
VARIANT_ENUM_CAST(GDKXblPresenceDeviceType::Enum);
VARIANT_ENUM_CAST(GDKXblPresenceTitleState::Enum);
VARIANT_ENUM_CAST(GDKXblPresenceFilter::Enum);
VARIANT_ENUM_CAST(GDKXblPresenceDetailLevel::Enum);
