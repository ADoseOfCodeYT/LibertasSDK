#pragma once
class EditorComponent;

class LightWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	void SetLightType(lb::scene::LightComponent::LightType type);

	lb::gui::Slider intensitySlider;
	lb::gui::Slider rangeSlider;
	lb::gui::Slider radiusSlider;
	lb::gui::Slider lengthSlider;
	lb::gui::Slider outerConeAngleSlider;
	lb::gui::Slider innerConeAngleSlider;
	lb::gui::Slider volumetricBoostSlider;
	lb::gui::CheckBox shadowCheckBox;
	lb::gui::CheckBox haloCheckBox;
	lb::gui::CheckBox volumetricsCheckBox;
	lb::gui::CheckBox staticCheckBox;
	lb::gui::CheckBox volumetricCloudsCheckBox;
	lb::gui::ColorPicker colorPicker;
	lb::gui::ComboBox typeSelectorComboBox;
	lb::gui::ComboBox shadowResolutionComboBox;

	lb::gui::Label lensflare_Label;
	lb::gui::Button lensflare_Button[7];

	struct CascadeConfig
	{
		lb::gui::Slider distanceSlider;
		lb::gui::Button removeButton;
	};
	lb::vector<CascadeConfig> cascades;
	lb::gui::Button addCascadeButton;
	void RefreshCascades();

	void ResizeLayout() override;
};

