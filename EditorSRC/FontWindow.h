#pragma once
class EditorComponent;

class FontWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::TextInputField textInput;
	lb::gui::Button fileButton;
	lb::gui::Button fontStyleButton;
	lb::gui::ComboBox fontSizeCombo;
	lb::gui::ComboBox hAlignCombo;
	lb::gui::ComboBox vAlignCombo;
	lb::gui::Slider rotationSlider;
	lb::gui::Slider spacingSlider;
	lb::gui::Slider softnessSlider;
	lb::gui::Slider boldenSlider;
	lb::gui::Slider shadowSoftnessSlider;
	lb::gui::Slider shadowBoldenSlider;
	lb::gui::Slider shadowOffsetXSlider;
	lb::gui::Slider shadowOffsetYSlider;
	lb::gui::Slider intensitySlider;
	lb::gui::Slider shadowIntensitySlider;
	lb::gui::CheckBox hiddenCheckBox;
	lb::gui::CheckBox cameraFacingCheckBox;
	lb::gui::CheckBox cameraScalingCheckBox;
	lb::gui::CheckBox depthTestCheckBox;
	lb::gui::CheckBox sdfCheckBox;
	lb::gui::ComboBox colorModeCombo;
	lb::gui::ColorPicker colorPicker;

	lb::gui::Label typewriterInfoLabel;
	lb::gui::Slider typewriterTimeSlider;
	lb::gui::CheckBox typewriterLoopedCheckBox;
	lb::gui::TextInputField typewriterStartInput;

	void ResizeLayout() override;
};
