#pragma once
class EditorComponent;

class SoftBodyWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label infoLabel;
	lb::gui::Button resetButton;
	lb::gui::Slider detailSlider;
	lb::gui::Slider massSlider;
	lb::gui::Slider frictionSlider;
	lb::gui::Slider pressureSlider;
	lb::gui::Slider restitutionSlider;
	lb::gui::Slider vertexRadiusSlider;
	lb::gui::CheckBox windCheckbox;

	void ResizeLayout() override;
};

