#pragma once

#include <Windows.h>
#include <winapifamily.h>
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <XAsync.h>
#include <XTaskQueue.h>
#include <XUser.h>
#include <XGame.h>
#include <XGameInvite.h>

#include <functional>
#include <string>
#include <vector>

namespace godot {
	class GDKXUserChangeEvent: public Object {
		GDCLASS(GDKXUserChangeEvent, Object)

	protected:
		static void _bind_methods();
	
	public:
		enum Enum: uint32_t {
			SignedInAgain = XUserChangeEvent::SignedInAgain,
			SigningOut = XUserChangeEvent::SigningOut,
			SignedOut = XUserChangeEvent::SignedOut,
			Gamertag = XUserChangeEvent::Gamertag,
			GamerPicture = XUserChangeEvent::GamerPicture,
			Privileges = XUserChangeEvent::Privileges
		};
	};

	class GDKUser;
	

	class GodotGDK : public RefCounted {
		GDCLASS(GodotGDK, RefCounted)

	private:
		static GodotGDK* _instance;
		bool _initialized = false;
		XTaskQueueHandle _async_queue = nullptr;
		static XTaskQueueRegistrationToken _invite_token;

		bool _invite_registered = false;

		static HRESULT Identity_TrySignInDefaultUserSilently(XTaskQueueHandle asyncQueue, Callable cb);
		static void Identity_TrySignInDefaultUserSilently_Callback(XAsyncBlock *asyncBlock);
		static void xgame_invite_register_for_event(Object* node);
		static void xgame_invite_unregister_for_event();

	protected:
		static void _bind_methods();
		void _notification(int p_what);

	public:
		static GodotGDK* get_singleton();
		static std::vector<std::function<void(Object*)>> initialize_listeners;
		static std::vector<std::function<void()>> deinitialize_listeners;
		GodotGDK() = default;
		~GodotGDK() override = default;

		inline XTaskQueueHandle get_async_queue() { return _async_queue; }

		int InitializeGDK(Callable, String);
		static XTaskQueueHandle GetQueueHandle();

		static XUserLocalId GetUserId();
		static XUserHandle GetUserHandle();
		static XAsyncBlock* GodotGDK::CreateAsyncBlock();
		static const char* GetSCID();
		static bool CheckResult(HRESULT result, const String& succeedMessage, const String& errorMessage);
		static char* CopyStringToChar(String string);

		// XGame
		int64_t get_xbox_title_id();
		void launch_new_game(const String &exe_path, const String &args, Ref<GDKUser> default_user);
		int64_t launch_restart_on_crash(const String &args);


	};
}

VARIANT_ENUM_CAST(GDKXUserChangeEvent::Enum);
