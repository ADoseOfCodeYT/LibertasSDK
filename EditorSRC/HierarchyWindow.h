#pragma once
class EditorComponent;

class HierarchyWindow : public lb::gui::Window
{
private:
	lb::unordered_set<lb::ecs::Entity> entities;
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::ComboBox parentCombo;

	void ResizeLayout() override;
};

