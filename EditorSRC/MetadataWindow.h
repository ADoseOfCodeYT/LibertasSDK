#pragma once
class EditorComponent;

class MetadataWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::ComboBox presetCombo;
	lb::gui::ComboBox addCombo;

	struct Entry
	{
		lb::gui::Button remove;
		lb::gui::TextInputField name;
		lb::gui::TextInputField value;
		lb::gui::CheckBox check;
		bool is_bool = false;
	};
	lb::vector<Entry> entries;

	void RefreshEntries();

	void ResizeLayout() override;
};

