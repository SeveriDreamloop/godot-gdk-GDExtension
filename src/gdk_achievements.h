#pragma once

#include "gdk_achievement.h"

#include <Windows.h>
#include <winapifamily.h>
#include <objbase.h>
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <XAsync.h>
#include <XTaskQueue.h>
#include <list>
#include <xsapi-c/services_c.h>

using namespace godot;

struct AchievementGatherer {
	Array RetrievedAchievements;
	Callable Callback;
};

class gdk_achievements : public RefCounted {
	GDCLASS(gdk_achievements, RefCounted)

	static void CALLBACK GetAchievementsCallback(_Inout_ XAsyncBlock* asyncBlock);
	static void FinishGetAchievements(AchievementGatherer* gatherer);
	static void InternalSetAchievementPercentage(String achievementId, uint32_t percentage, const std::string &successMessage, const std::string &failMessage);
protected:
	static void _bind_methods();

public:
	gdk_achievements() = default;
	~gdk_achievements() override = default;

	void GetAchievements(Callable callback);
	void UnlockAchievement(godot::String achievementId);
	void SetAchievementPercentage(godot::String achievementId, uint32_t percentage);
};
