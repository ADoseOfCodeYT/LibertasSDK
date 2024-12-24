#pragma once
class EditorComponent;

class EnvProbeWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::Label infoLabel;
	lb::gui::CheckBox realTimeCheckBox;
	lb::gui::CheckBox msaaCheckBox;
	lb::gui::Button refreshButton;
	lb::gui::Button refreshAllButton;
	lb::gui::Button importButton;
	lb::gui::Button exportButton;
	lb::gui::ComboBox resolutionCombo;

	void ResizeLayout() override;
};

