#pragma once
class EditorComponent;

class RigidBodyWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	void RefreshShapeType();

	lb::gui::ComboBox collisionShapeComboBox;
	lb::gui::Slider XSlider;
	lb::gui::Slider YSlider;
	lb::gui::Slider ZSlider;
	lb::gui::Slider massSlider;
	lb::gui::Slider frictionSlider;
	lb::gui::Slider restitutionSlider;
	lb::gui::Slider lineardampingSlider;
	lb::gui::Slider angulardampingSlider;
	lb::gui::Slider buoyancySlider;
	lb::gui::Slider physicsMeshLODSlider;
	lb::gui::CheckBox startDeactivatedCheckBox;
	lb::gui::CheckBox disabledeactivationCheckBox;
	lb::gui::CheckBox kinematicCheckBox;
	lb::gui::Slider offsetXSlider;
	lb::gui::Slider offsetYSlider;
	lb::gui::Slider offsetZSlider;

	void ResizeLayout() override;
};

