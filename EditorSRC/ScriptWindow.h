#pragma once
class EditorComponent;

class ScriptWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Button fileButton;
	lb::gui::CheckBox playonceCheckBox;
	lb::gui::Button playstopButton;

	void Update(const lb::Canvas& canvas, float dt) override;
	void ResizeLayout() override;
};

