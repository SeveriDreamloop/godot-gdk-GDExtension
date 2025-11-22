#include "gdk_achievements.h"

#include "gdk_achievement.h"
#include "godot_gdk.h"
#include "../godot-cpp/gen/include/godot_cpp/variant/callable.hpp"

#include <Windows.h>
#include <winapifamily.h>
#include <objbase.h>
#include <XGameRuntimeInit.h>
#include <XTaskQueue.h>
#include <xsapi-c/services_c.h>
#include <XUser.h>
#include <XGame.h>

#include <iomanip>
#include <list>
#include <map>

void gdk_achievements::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("GetAchievements", "callback"), &gdk_achievements::GetAchievements);
	godot::ClassDB::bind_method(D_METHOD("UnlockAchievement", "achievementId"), &gdk_achievements::UnlockAchievement);
	godot::ClassDB::bind_method(D_METHOD("SetAchievementPercentage", "achievementId", "percentage"), &gdk_achievements::SetAchievementPercentage);
}

void gdk_achievements::GetAchievements(godot::Callable callback) {
	XAsyncBlock* asyncBlock = godot_gdk::CreateAsyncBlock();

	AchievementGatherer* gatherer = new AchievementGatherer();
	gatherer->Callback = callback;

	asyncBlock->context = gatherer;
	asyncBlock->callback = GetAchievementsCallback;

	XblContextHandle handle;

	if (!godot_gdk::CreateContextHandle(&handle)) {
		return;
	}

	uint32_t titleID;
	if (!godot_gdk::CheckResult(!XGameGetXboxTitleId(&titleID), "Successfully retrieved titleID", "Failed to retrieve titleID")) {
		return;
	}

	HRESULT hr = XblAchievementsGetAchievementsForTitleIdAsync(handle, godot_gdk::GetUserId().value, titleID, XblAchievementType::All, false, XblAchievementOrderBy::DefaultOrder, 0, 0, asyncBlock);

	godot_gdk::CheckResult(hr, "Successfully started retrieving achievements", "Failed to start retrieving achievements");
}

void gdk_achievements::GetAchievementsCallback(XAsyncBlock *asyncBlock) {
	AchievementGatherer* gatherer = static_cast<AchievementGatherer*>(asyncBlock->context);

	XblAchievementsResultHandle resultHandle;
	HRESULT hr = XblAchievementsGetAchievementsForTitleIdResult(asyncBlock, &resultHandle);

	if (!godot_gdk::CheckResult(hr, "", "Failed to retrieve achievements")) {
		return;
	}

	const XblAchievement *achievements;
	size_t size;
	hr = XblAchievementsResultGetAchievements(resultHandle, &achievements, &size);

	if (!godot_gdk::CheckResult(hr, "", "Failed to retrieve achievements")) {
		return;
	}

	for (int i = 0; i < size; i++) {
		gatherer->RetrievedAchievements.push_back(memnew(gdk_achievement(&achievements[i])));
	}

	bool hasMoreAchievements = false;

	//GDK can only retrieve 32 achievements at once. Check if there is more, and retrieve until we have all.
	XblAchievementsResultHasNext(resultHandle, &hasMoreAchievements);

	if (hasMoreAchievements) {
		XblAchievementsResultGetNextAsync(resultHandle, 0, asyncBlock);
	} else {
		FinishGetAchievements(gatherer);
		delete asyncBlock;
	}

	XblAchievementsResultCloseHandle(resultHandle);
}

void gdk_achievements::FinishGetAchievements(AchievementGatherer* gatherer) {
	if (gatherer->Callback.is_valid()) {
		gatherer->Callback.call_deferred(gatherer->RetrievedAchievements);
	}

	delete gatherer;
}

void gdk_achievements::UnlockAchievement(godot::String achievementId) {
	InternalSetAchievementPercentage(achievementId, 100, "Successfully unlocked achievement", "Failed to unlock achievement");
}


void gdk_achievements::SetAchievementPercentage(godot::String achievementId, uint32_t percentage) {
	InternalSetAchievementPercentage(achievementId, percentage, "Successfully set achievement progress", "Failed to set achievement progress");
}

void gdk_achievements::InternalSetAchievementPercentage(godot::String achievementId, uint32_t percentage, const std::string &successMessage, const std::string &failMessage) {

	XblContextHandle handle;

	if (!godot_gdk::CreateContextHandle(&handle)) {
		return;
	}

	XAsyncBlock* asyncBlock = godot_gdk::CreateAsyncBlock();

	HRESULT result = XblAchievementsUpdateAchievementAsync(handle, godot_gdk::GetUserId().value, achievementId.utf8(), percentage, asyncBlock);

	godot_gdk::CheckResult(result, successMessage, failMessage);
}
