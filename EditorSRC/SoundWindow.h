#pragma once
class EditorComponent;

class WaveGraph : public lb::gui::Widget
{
public:
	lb::scene::SoundComponent* sound = nullptr;

	void Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const override;
};

class SoundWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Button openButton;
	lb::gui::ComboBox reverbComboBox;
	lb::gui::Label filenameLabel;
	lb::gui::Button playstopButton;
	lb::gui::CheckBox loopedCheckbox;
	lb::gui::CheckBox reverbCheckbox;
	lb::gui::CheckBox disable3dCheckbox;
	lb::gui::Slider volumeSlider;
	lb::gui::ComboBox submixComboBox;
	lb::gui::TextInputField beginInput;
	lb::gui::TextInputField lengthInput;
	lb::gui::TextInputField loopBeginInput;
	lb::gui::TextInputField loopLengthInput;
	WaveGraph waveGraph;

	void ResizeLayout() override;
};
