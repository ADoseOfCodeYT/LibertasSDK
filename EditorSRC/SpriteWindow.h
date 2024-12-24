#pragma once
class EditorComponent;

class SpriteWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Button textureButton;
	lb::gui::Button maskButton;
	lb::gui::Slider pivotXSlider;
	lb::gui::Slider pivotYSlider;
	lb::gui::Slider intensitySlider;
	lb::gui::Slider rotationSlider;
	lb::gui::Slider saturationSlider;
	lb::gui::Slider alphaStartSlider;
	lb::gui::Slider alphaEndSlider;
	lb::gui::Slider borderSoftenSlider;
	lb::gui::Slider cornerRounding0Slider;
	lb::gui::Slider cornerRounding1Slider;
	lb::gui::Slider cornerRounding2Slider;
	lb::gui::Slider cornerRounding3Slider;
	lb::gui::ComboBox qualityCombo;
	lb::gui::ComboBox samplemodeCombo;
	lb::gui::ComboBox blendModeCombo;
	lb::gui::CheckBox mirrorCheckBox;
	lb::gui::CheckBox hiddenCheckBox;
	lb::gui::CheckBox cameraFacingCheckBox;
	lb::gui::CheckBox cameraScalingCheckBox;
	lb::gui::CheckBox depthTestCheckBox;
	lb::gui::CheckBox distortionCheckBox;
	lb::gui::ColorPicker colorPicker;

	lb::gui::Slider movingTexXSlider;
	lb::gui::Slider movingTexYSlider;

	lb::gui::Slider drawrectFrameRateSlider;
	lb::gui::TextInputField drawrectFrameCountInput;
	lb::gui::TextInputField drawrectHorizontalFrameCountInput;

	lb::gui::Slider wobbleXSlider;
	lb::gui::Slider wobbleYSlider;
	lb::gui::Slider wobbleSpeedSlider;

	void ResizeLayout() override;

	void UpdateSpriteDrawRectParams(lb::Sprite* sprite);
};
