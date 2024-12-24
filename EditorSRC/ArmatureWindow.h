#pragma once
class EditorComponent;

class ArmatureWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);
	void RefreshBoneList();

	lb::gui::Label infoLabel;
	lb::gui::Button resetPoseButton;
	lb::gui::Button createHumanoidButton;
	lb::gui::TreeList boneList;

	void ResizeLayout() override;
};

