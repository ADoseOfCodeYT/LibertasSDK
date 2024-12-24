#pragma once
class EditorComponent;

class MaterialPickerWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;

	lb::vector<lb::gui::Button> buttons;

	lb::gui::Slider zoomSlider;

	void ResizeLayout() override;

	// Recreating buttons shouldn't be done every frame because interaction states need multi-frame execution
	void RecreateButtons();
};

