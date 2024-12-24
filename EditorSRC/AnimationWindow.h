#pragma once
class EditorComponent;

class AnimationWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label infoLabel;
	lb::gui::ComboBox modeComboBox;
	lb::gui::Button loopTypeButton;
	lb::gui::Button	playButton;
	lb::gui::Button	playFromStartButton;
	lb::gui::Button	backwardsButton;
	lb::gui::Button	backwardsFromEndButton;
	lb::gui::Button	stopButton;
	lb::gui::Slider	timerSlider;
	lb::gui::Slider	amountSlider;
	lb::gui::Slider	speedSlider;
	lb::gui::TextInputField startInput;
	lb::gui::TextInputField endInput;

	lb::gui::ComboBox recordCombo;
	lb::gui::TreeList keyframesList;

	lb::gui::ComboBox retargetCombo;

	lb::gui::CheckBox rootMotionCheckBox;
	lb::gui::ComboBox rootBoneComboBox;

	void Update();

	void RefreshKeyframesList();

	void ResizeLayout() override;
};
