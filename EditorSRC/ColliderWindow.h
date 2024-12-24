#pragma once
class EditorComponent;

class ColliderWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label infoLabel;
	lb::gui::CheckBox cpuCheckBox;
	lb::gui::CheckBox gpuCheckBox;
	lb::gui::ComboBox shapeCombo;
	lb::gui::Slider radiusSlider;
	lb::gui::Slider offsetX;
	lb::gui::Slider offsetY;
	lb::gui::Slider offsetZ;
	lb::gui::Slider tailX;
	lb::gui::Slider tailY;
	lb::gui::Slider tailZ;

	void ResizeLayout() override;
};

