#pragma once
class EditorComponent;

class LayerWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label label;
	lb::gui::CheckBox layers[32];
	lb::gui::Button enableAllButton;
	lb::gui::Button enableNoneButton;

	void ResizeLayout() override;
};

