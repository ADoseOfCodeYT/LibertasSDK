#pragma once
class EditorComponent;

class HumanoidWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);
	void RefreshBoneList();

	lb::gui::Label infoLabel;
	lb::gui::CheckBox lookatMouseCheckBox;
	lb::gui::CheckBox lookatCheckBox;
	lb::gui::CheckBox ragdollCheckBox;
	lb::gui::Slider headRotMaxXSlider;
	lb::gui::Slider headRotMaxYSlider;
	lb::gui::Slider headRotSpeedSlider;
	lb::gui::Slider eyeRotMaxXSlider;
	lb::gui::Slider eyeRotMaxYSlider;
	lb::gui::Slider eyeRotSpeedSlider;
	lb::gui::Slider headSizeSlider;
	lb::gui::Slider ragdollFatnessSlider;
	lb::gui::Slider ragdollHeadSizeSlider;
	lb::gui::TreeList boneList;

	void ResizeLayout() override;

	void UpdateHumanoids();
};

