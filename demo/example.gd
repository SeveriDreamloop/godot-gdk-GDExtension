extends Node

var example;
var achievement;

func _ready() -> void:
	example = godot_gdk.new()
	example.InitializeGDK(on_init_done)
	
func on_init_done() -> void:
	print("callback invoked")
	
	achievement = gdk_achievements.new()
	achievement.GetAchievements(get_achievements)
	
func get_achievements(achievements):
	print("got achievements");
	
	for a in achievements:
		print(a.name);
	
	achievement.UnlockAchievement(achievements[3].id)
		
	
	
	
