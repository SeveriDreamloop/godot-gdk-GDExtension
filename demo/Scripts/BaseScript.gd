extends Node
class_name BaseScript

var currentNode;
var gdkObject;

static func buttonInfo(text, requiresSubMenu, function, hasOutput = false, inputs = []):
	return {
		"name" = text,
		"submenu" = requiresSubMenu,
		"function" = function,
		"output" = hasOutput,
		"inputs" = inputs
	}
	

func _ready():
	gdkObject = godot_gdk.new()
	gdkObject.InitializeGDK(on_init_done)
	
	create_menu("MainMenu")
	currentNode = $Control/MainMenu
		
func create_menu(menuName, previousMenu = ""):
	#Create the control as base for new menu
	var control = Control.new()
	control.name = menuName
	
	#Load script for the new menu if there is one
	if ResourceLoader.exists("res://Scripts/"+menuName+".gd", "Script"):
		var script := load("res://Scripts/"+menuName+".gd")
		control.set_script(script)
		
	$Control.add_child(control)
	control.visible=false
	
	#Create grid container for all the buttons
	var gridContainer = GridContainer.new()
	gridContainer.name = "Buttons"
	control.add_child(gridContainer)
	
	#Load script for the new menu if there is one
	if control.has_method("get_infos"):
		for info in control.get_infos():
			var parentNode = get_node("Control/"+menuName+"/Buttons")
			
			var horizontalContainer = GridContainer.new();
			var columns = 1;
			parentNode.add_child(horizontalContainer)
			
			#Create button to the new menu
			var button = Button.new()
			button.text = info["name"]
			horizontalContainer.add_child(button)
			
			for input in info["inputs"]:
				var inputField = LineEdit.new()
				inputField.expand_to_text_length = true
				inputField.placeholder_text = input;
				horizontalContainer.add_child(inputField)
				info["function"] = info["function"].bind(inputField)
				columns += 1
			
			if info["output"]:
				var label = Label.new()
				label.text = "Output"
				horizontalContainer.add_child(label)
				info["function"] = info["function"].bind(label)
				columns += 1
				
			horizontalContainer.columns = columns
			button.pressed.connect(info["function"])
			
			#If the menu should have submenus, start same function with new info
			if info["submenu"]:
				create_menu(info["name"], menuName)
	
	if previousMenu != "":
		#Add back button as last button to go back to previous menus
		var returnButton = Button.new()
		returnButton.text = "Back"
		returnButton.pressed.connect(Callable(self, "_button_pressed").bind(previousMenu))

func _button_pressed(menu):
	currentNode.visible = false;
	currentNode = get_node("Control/" + menu)
	currentNode.visible = true;

func on_init_done() -> void:
	print("GDK Initialization finished")
	currentNode.visible = true;
