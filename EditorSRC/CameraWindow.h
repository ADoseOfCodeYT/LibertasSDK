#pragma once
class EditorComponent;

class CameraWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	void ResetCam();

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);
	void Update();


	lb::gui::Slider farPlaneSlider;
	lb::gui::Slider nearPlaneSlider;
	lb::gui::Slider fovSlider;
	lb::gui::Slider focalLengthSlider;
	lb::gui::Slider apertureSizeSlider;
	lb::gui::Slider apertureShapeXSlider;
	lb::gui::Slider apertureShapeYSlider;
	lb::gui::Slider movespeedSlider;
	lb::gui::Slider rotationspeedSlider;
	lb::gui::Slider accelerationSlider;
	lb::gui::Button resetButton;
	lb::gui::CheckBox fpsCheckBox;
	lb::gui::CheckBox orthoCheckBox;

	lb::gui::Button proxyButton;
	lb::gui::CheckBox followCheckBox;
	lb::gui::Slider followSlider;

	void ResizeLayout() override;
};

