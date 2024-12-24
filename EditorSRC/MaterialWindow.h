#pragma once
class EditorComponent;

class MaterialWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	lb::gui::TextInputField materialNameField;
	lb::gui::CheckBox shadowReceiveCheckBox;
	lb::gui::CheckBox shadowCasterCheckBox;
	lb::gui::CheckBox useVertexColorsCheckBox;
	lb::gui::CheckBox specularGlossinessCheckBox;
	lb::gui::CheckBox occlusionPrimaryCheckBox;
	lb::gui::CheckBox occlusionSecondaryCheckBox;
	lb::gui::CheckBox vertexAOCheckBox;
	lb::gui::CheckBox windCheckBox;
	lb::gui::CheckBox doubleSidedCheckBox;
	lb::gui::CheckBox outlineCheckBox;
	lb::gui::CheckBox preferUncompressedCheckBox;
	lb::gui::CheckBox disableStreamingCheckBox;
	lb::gui::CheckBox coplanarCheckBox;
	lb::gui::ComboBox shaderTypeComboBox;
	lb::gui::ComboBox blendModeComboBox;
	lb::gui::ComboBox shadingRateComboBox;
	lb::gui::Slider normalMapSlider;
	lb::gui::Slider roughnessSlider;
	lb::gui::Slider reflectanceSlider;
	lb::gui::Slider metalnessSlider;
	lb::gui::Slider emissiveSlider;
	lb::gui::Slider saturationSlider;
	lb::gui::Slider cloakSlider;
	lb::gui::Slider chromaticAberrationSlider;
	lb::gui::Slider transmissionSlider;
	lb::gui::Slider refractionSlider;
	lb::gui::Slider pomSlider;
	lb::gui::Slider anisotropyStrengthSlider;
	lb::gui::Slider anisotropyRotationSlider;
	lb::gui::Slider displacementMappingSlider;
	lb::gui::Slider subsurfaceScatteringSlider;
	lb::gui::Slider texAnimFrameRateSlider;
	lb::gui::Slider texAnimDirectionSliderU;
	lb::gui::Slider texAnimDirectionSliderV;
	lb::gui::Slider texMulSliderX;
	lb::gui::Slider texMulSliderY;
	lb::gui::Slider alphaRefSlider;
	lb::gui::Slider sheenRoughnessSlider;
	lb::gui::Slider clearcoatSlider;
	lb::gui::Slider clearcoatRoughnessSlider;
	lb::gui::Slider blendTerrainSlider;

	lb::gui::ComboBox colorComboBox;
	lb::gui::ColorPicker colorPicker;

	lb::gui::ComboBox textureSlotComboBox;
	lb::gui::Button textureSlotButton;
	lb::gui::Label textureSlotLabel;
	lb::gui::TextInputField textureSlotUvsetField;

	void ResizeLayout() override;
};

