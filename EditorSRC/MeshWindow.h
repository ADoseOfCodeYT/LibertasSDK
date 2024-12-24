#pragma once
class EditorComponent;

class MeshWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	int subset = -1;
	void SetEntity(lb::ecs::Entity entity, int subset);

	lb::gui::Label meshInfoLabel;
	lb::gui::Button subsetRemoveButton;
	lb::gui::ComboBox subsetComboBox;
	lb::gui::ComboBox subsetMaterialComboBox;
	lb::gui::CheckBox doubleSidedCheckBox;
	lb::gui::CheckBox doubleSidedShadowCheckBox;
	lb::gui::CheckBox bvhCheckBox;
	lb::gui::CheckBox quantizeCheckBox;
	lb::gui::Button impostorCreateButton;
	lb::gui::Slider impostorDistanceSlider;
	lb::gui::Slider tessellationFactorSlider;
	lb::gui::Button flipCullingButton;
	lb::gui::Button flipNormalsButton;
	lb::gui::Button computeNormalsSmoothButton;
	lb::gui::Button computeNormalsHardButton;
	lb::gui::Button recenterButton;
	lb::gui::Button recenterToBottomButton;
	lb::gui::Button mergeButton;
	lb::gui::Button optimizeButton;
	lb::gui::Button exportHeaderButton;

	lb::gui::ComboBox morphTargetCombo;
	lb::gui::Slider morphTargetSlider;

	lb::gui::Button lodgenButton;
	lb::gui::Slider lodCountSlider;
	lb::gui::Slider lodQualitySlider;
	lb::gui::Slider lodErrorSlider;
	lb::gui::CheckBox lodSloppyCheckBox;

	void ResizeLayout() override;
};

