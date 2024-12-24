#pragma once
class EditorComponent;

class DecalWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label infoLabel;
	lb::gui::CheckBox placementCheckBox;
	lb::gui::CheckBox onlyalphaCheckBox;
	lb::gui::Slider slopeBlendPowerSlider;

	void ResizeLayout() override;
};

