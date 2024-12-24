#pragma once
class EditorComponent;

class CameraPreview : public lb::gui::Widget
{
public:
	lb::RenderPath3D renderpath;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;

	void RenderPreview();

	void Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const override;
};

class CameraComponentWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Slider farPlaneSlider;
	lb::gui::Slider nearPlaneSlider;
	lb::gui::Slider fovSlider;
	lb::gui::Slider focalLengthSlider;
	lb::gui::Slider apertureSizeSlider;
	lb::gui::Slider apertureShapeXSlider;
	lb::gui::Slider apertureShapeYSlider;

	CameraPreview preview;

	void ResizeLayout() override;
};

