#pragma once
class EditorComponent;

class VideoPreview : public lb::gui::Widget
{
public:
	lb::scene::VideoComponent* video = nullptr;

	void Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const override;
};

class VideoWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Button openButton;
	lb::gui::Label filenameLabel;
	lb::gui::Button playpauseButton;
	lb::gui::Button stopButton;
	lb::gui::CheckBox loopedCheckbox;
	VideoPreview preview;
	lb::gui::Slider timerSlider;
	lb::gui::Label infoLabel;

	void ResizeLayout() override;
};
