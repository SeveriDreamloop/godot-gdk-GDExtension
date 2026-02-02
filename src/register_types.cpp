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
	GDREGISTER_CLASS(GDKGameSave);
	GDREGISTER_CLASS(GDKGameSaveBlob);
	GDREGISTER_CLASS(GDKGameSaveBlobInfo);

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
