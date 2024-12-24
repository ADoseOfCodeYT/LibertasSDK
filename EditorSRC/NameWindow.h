#pragma once
class EditorComponent;

class NameWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::TextInputField nameInput;

	void ResizeLayout() override;
};

