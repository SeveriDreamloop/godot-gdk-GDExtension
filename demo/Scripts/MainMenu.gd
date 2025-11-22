extends Node
	
func get_infos():
	var node = get_node("/root/Node")
	return [
		BaseScript.buttonInfo("Achievements", true, Callable(node, "_button_pressed").bind("Achievements")),
		BaseScript.buttonInfo("Saves", true, Callable(node, "_button_pressed").bind("Saves"))
	]
