#pragma once
class EditorComponent;

class IKWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::ComboBox targetCombo;
	lb::gui::CheckBox disabledCheckBox;
	lb::gui::Slider chainLengthSlider;
	lb::gui::Slider iterationCountSlider;

	void ResizeLayout() override;
};

