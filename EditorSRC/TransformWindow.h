#pragma once
class EditorComponent;

class TransformWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Button clearButton;

	lb::gui::TextInputField txInput;
	lb::gui::TextInputField tyInput;
	lb::gui::TextInputField tzInput;

	lb::gui::TextInputField rollInput;
	lb::gui::TextInputField pitchInput;
	lb::gui::TextInputField yawInput;
					 
	lb::gui::TextInputField rxInput;
	lb::gui::TextInputField ryInput;
	lb::gui::TextInputField rzInput;
	lb::gui::TextInputField rwInput;
					 
	lb::gui::TextInputField sxInput;
	lb::gui::TextInputField syInput;
	lb::gui::TextInputField szInput;
	lb::gui::TextInputField suInput;

	lb::gui::TextInputField snapScaleInput;
	lb::gui::TextInputField snapRotateInput;
	lb::gui::TextInputField snapTranslateInput;

	lb::gui::Button resetTranslationButton;
	lb::gui::Button resetScaleButton;
	lb::gui::Button resetScaleUniformButton;
	lb::gui::Button resetRotationButton;

	void ResizeLayout() override;
};

