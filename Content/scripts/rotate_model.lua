-- This script will load a teapot model with lights and rotate the whole model
killProcesses()  -- stops all running lua coroutine processes

console_post("---> START SCRIPT: rotate_model.lua")

scene = GetScene()
scene.Clear()
model_entity = LoadModel(script_dir() .. "../models/teapot.wiscene")
transform_component = scene.Component_GetTransform(model_entity)

runProcess(function()
	while true do
		transform_component.Rotate(Vector(0, 0.1)) -- rotate around y axis
		update()
	end
end)

console_post("---> END SCRIPT: rotate_model.lua")
