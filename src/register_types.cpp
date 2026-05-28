#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <classes/engine.hpp>

#include "godot_gdk.h"
#include "gdk_asyncblock.h"
#include "gdk_achievement.h"
#include "gdk_achievements.h"
#include "gdk_game_save.h"
#include "gdk_game_save_blob.h"
#include "gdk_user.h"
#include "gdk_presence.h"

using namespace godot;

static GodotGDK* gdk = nullptr;

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(GodotGDK);
	GDREGISTER_CLASS(GDKAsyncBlock);
	GDREGISTER_CLASS(GDKAchievement);
	GDREGISTER_CLASS(GDKAchievements);
	GDREGISTER_CLASS(GDKAchievementsResultHandle);
	GDREGISTER_CLASS(GDKGameSave);
	GDREGISTER_CLASS(GDKGameSaveBlob);
	GDREGISTER_CLASS(GDKGameSaveBlobInfo);
	GDREGISTER_CLASS(GDKXUserAddOptions);
	GDREGISTER_CLASS(GDKXUserState);
	GDREGISTER_CLASS(GDKXUserGamertagComponent);
	GDREGISTER_CLASS(GDKXUserGamerPictureSize);
	GDREGISTER_CLASS(GDKXUserAgeGroup);
	GDREGISTER_CLASS(GDKXUserPrivilege);
	GDREGISTER_CLASS(GDKXUserPrivilegeOptions);
	GDREGISTER_CLASS(GDKXUserPrivilegeDenyReason);
	GDREGISTER_CLASS(GDKXUserGetTokenAndSignatureOptions);
	GDREGISTER_CLASS(GDKXUserDefaultAudioEndpointKind);
	GDREGISTER_CLASS(GDKXUserGetTokenAndSignatureData);
	GDREGISTER_CLASS(GDKXUserSignOutDeferralHandle);
	GDREGISTER_CLASS(GDKUser);
	GDREGISTER_CLASS(GDKPresence);
	GDREGISTER_CLASS(GDKPresenceRecord);
	GDREGISTER_CLASS(GDKPresenceDeviceRecord);
	GDREGISTER_CLASS(GDKPresenceTitleRecord);
	GDREGISTER_CLASS(GDKXblPresenceUserState);
	GDREGISTER_CLASS(GDKXblPresenceDeviceType);
	GDREGISTER_CLASS(GDKXblPresenceTitleState);
	GDREGISTER_CLASS(GDKXblPresenceFilter);
	GDREGISTER_CLASS(GDKXblPresenceDetailLevel);

	gdk = GodotGDK::get_singleton();
	Engine::get_singleton()->register_singleton("GDK", gdk);
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	Engine::get_singleton()->unregister_singleton("GDK");
	memdelete(gdk);
	gdk = nullptr;
}

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT init_gdk(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_gdextension_types);
		init_obj.register_terminator(uninitialize_gdextension_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}
