#pragma once
class EditorComponent;

class VoxelGridWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label infoLabel;
	lb::gui::TextInputField dimXInput;
	lb::gui::TextInputField dimYInput;
	lb::gui::TextInputField dimZInput;
	lb::gui::Button	clearButton;
	lb::gui::Button	voxelizeObjectsButton;
	lb::gui::Button	voxelizeNavigationButton;
	lb::gui::Button	voxelizeCollidersButton;
	lb::gui::Button	floodfillButton;
	lb::gui::Button	fitToSceneButton;
	lb::gui::Button	generateMeshButton;
	lb::gui::Button	generateSimplifiedMeshButton;
	lb::gui::CheckBox subtractCheckBox;
	lb::gui::CheckBox debugAllCheckBox;

	void ResizeLayout() override;
};

