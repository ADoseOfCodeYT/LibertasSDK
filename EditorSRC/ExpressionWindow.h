#pragma once
class EditorComponent;

class ExpressionWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label infoLabel;
	lb::gui::CheckBox talkCheckBox;
	lb::gui::Slider blinkFrequencySlider;
	lb::gui::Slider blinkLengthSlider;
	lb::gui::Slider blinkCountSlider;
	lb::gui::Slider lookFrequencySlider;
	lb::gui::Slider lookLengthSlider;
	lb::gui::TreeList expressionList;
	lb::gui::Slider weightSlider;
	lb::gui::CheckBox binaryCheckBox;
	lb::gui::ComboBox overrideMouthCombo;
	lb::gui::ComboBox overrideBlinkCombo;
	lb::gui::ComboBox overrideLookCombo;

	void ResizeLayout() override;
};

