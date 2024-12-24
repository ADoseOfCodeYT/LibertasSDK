#pragma once
class EditorComponent;

class ForceFieldWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::ComboBox typeComboBox;
	lb::gui::Slider gravitySlider;
	lb::gui::Slider rangeSlider;

	void ResizeLayout() override;
};

