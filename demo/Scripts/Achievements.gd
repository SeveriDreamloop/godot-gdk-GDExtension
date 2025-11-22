extends Node

var achievement;
var achievements;

func get_infos():
	return [
		BaseScript.buttonInfo("Get Achievements", false, Callable(self,"get_achievements"), true),
		BaseScript.buttonInfo("Unlock Achievement With Index", false, Callable(self, "unlock_achievement_with_index"), false, ["Achievement Index"]),
		BaseScript.buttonInfo("Unlock Achievement With ID", false, Callable(self, "unlock_achievement_with_id"), false, ["Achievement ID"])
	]

func _ready() -> void:
	achievement = gdk_achievements.new()
	print("Achievements initialized")
	
func get_achievements(output) -> void:
	print("Start retrieveing achievements")
	var callable = Callable(self, "receive_achievements").bind(output)
	achievement.GetAchievements(callable)
	
func receive_achievements(received_achievements, output):
	print("Received achievements");
	self.achievements = received_achievements
	
	var achievementNames = ""
	
	for a in received_achievements:
		print(a.name);
		achievementNames += "\""+a.name+"\", "
	
	output.text = achievementNames
		
func unlock_achievement_with_index(indexInput):
	if achievements == null:
		printerr("Achievement list is null. Retrieve achievements first")
		return
		
	if !indexInput.text.is_valid_int():
		printerr("The given index isn't valid int. Either give a number or use unlock_achievement_with_id")
		return
	
	achievement.UnlockAchievement(achievements[int(indexInput.text)].id)
	
	
func unlock_achievement_with_id(idInput):
	achievement.UnlockAchievement(idInput.text)
