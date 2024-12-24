#pragma once
class EditorComponent;

class GeneralWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::gui::CheckBox versionCheckBox;
	lb::gui::CheckBox fpsCheckBox;
	lb::gui::CheckBox otherinfoCheckBox;
	lb::gui::ComboBox themeCombo;
	lb::gui::ComboBox saveModeComboBox;
	lb::gui::ComboBox languageCombo;

	lb::gui::CheckBox physicsDebugCheckBox;
	lb::gui::CheckBox nameDebugCheckBox;
	lb::gui::CheckBox gridHelperCheckBox;
	lb::gui::CheckBox aabbDebugCheckBox;
	lb::gui::CheckBox boneLinesCheckBox;
	lb::gui::CheckBox debugEmittersCheckBox;
	lb::gui::CheckBox debugForceFieldsCheckBox;
	lb::gui::CheckBox debugRaytraceBVHCheckBox;
	lb::gui::CheckBox wireFrameCheckBox;
	lb::gui::CheckBox envProbesCheckBox;
	lb::gui::CheckBox cameraVisCheckBox;
	lb::gui::CheckBox colliderVisCheckBox;
	lb::gui::CheckBox springVisCheckBox;
	lb::gui::CheckBox freezeCullingCameraCheckBox;
	lb::gui::CheckBox disableAlbedoMapsCheckBox;
	lb::gui::CheckBox forceDiffuseLightingCheckBox;

	lb::gui::Slider transformToolOpacitySlider;
	lb::gui::Slider bonePickerOpacitySlider;
	lb::gui::CheckBox skeletonsVisibleCheckBox;

	lb::gui::Button localizationButton;
	lb::gui::Button eliminateCoarseCascadesButton;
	lb::gui::Button ddsConvButton;
	lb::gui::Button ktxConvButton;

	void ResizeLayout() override;

	void RefreshLanguageSelectionAfterWholeGUIWasInitialized();
};

