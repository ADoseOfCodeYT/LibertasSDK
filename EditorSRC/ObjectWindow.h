#pragma once
class EditorComponent;

class ObjectWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::ComboBox meshCombo;
	lb::gui::CheckBox renderableCheckBox;
	lb::gui::CheckBox shadowCheckBox;
	lb::gui::CheckBox navmeshCheckBox;
	lb::gui::CheckBox foregroundCheckBox;
	lb::gui::CheckBox notVisibleInMainCameraCheckBox;
	lb::gui::CheckBox notVisibleInReflectionsCheckBox;
	lb::gui::CheckBox wetmapCheckBox;
	lb::gui::Slider ditherSlider;
	lb::gui::Slider alphaRefSlider;
	lb::gui::Slider rimHighlightIntesitySlider;
	lb::gui::Slider rimHighlightFalloffSlider;
	lb::gui::Slider cascadeMaskSlider;
	lb::gui::Slider lodSlider;
	lb::gui::Slider drawdistanceSlider;
	lb::gui::Slider sortPrioritySlider;

	lb::gui::ComboBox colorComboBox;
	lb::gui::ColorPicker colorPicker;

	lb::gui::Slider lightmapResolutionSlider;
	lb::gui::CheckBox lightmapBlockCompressionCheckBox;
	lb::gui::ComboBox lightmapSourceUVSetComboBox;
	lb::gui::Button generateLightmapButton;
	lb::gui::Button stopLightmapGenButton;
	lb::gui::Button clearLightmapButton;
	lb::gui::Button lightmapPreviewButton;

	lb::gui::Button vertexAOButton;
	bool deleteAOMode = false;
	lb::gui::Slider vertexAORayCountSlider;
	lb::gui::Slider vertexAORayLengthSlider;

	void ResizeLayout() override;
};

