#pragma once
class EditorComponent;

class SpringWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Button resetAllButton;
	lb::gui::CheckBox disabledCheckBox;
	lb::gui::CheckBox gravityCheckBox;
	lb::gui::Slider stiffnessSlider;
	lb::gui::Slider dragSlider;
	lb::gui::Slider windSlider;
	lb::gui::Slider gravitySlider;
	lb::gui::Slider hitradiusSlider;

	void ResizeLayout() override;
};

