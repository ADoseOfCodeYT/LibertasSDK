#pragma once
class EditorComponent;

class GraphicsWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;

	lb::gui::CheckBox vsyncCheckBox;
	lb::gui::ComboBox swapchainComboBox;
	lb::gui::Slider hdrcalibrationSlider;
	lb::gui::ComboBox renderPathComboBox;
	lb::gui::Slider pathTraceTargetSlider;
	lb::gui::Label pathTraceStatisticsLabel;
	lb::gui::CheckBox occlusionCullingCheckBox;
	lb::gui::CheckBox visibilityComputeShadingCheckBox;
	lb::gui::CheckBox meshShaderCheckBox;
	lb::gui::CheckBox meshletOcclusionCullingCheckBox;
	lb::gui::Slider resolutionScaleSlider;
	lb::gui::Slider streamingSlider;
	lb::gui::Slider GIBoostSlider;
	lb::gui::CheckBox ddgiCheckBox;
	lb::gui::CheckBox ddgiDebugCheckBox;
	lb::gui::TextInputField ddgiX;
	lb::gui::TextInputField ddgiY;
	lb::gui::TextInputField ddgiZ;
	lb::gui::Slider ddgiRayCountSlider;
	lb::gui::Slider ddgiBlendSpeedSlider;
	lb::gui::Slider ddgiSmoothBackfaceSlider;
	lb::gui::CheckBox vxgiCheckBox;
	lb::gui::ComboBox vxgiDebugCombo;
	lb::gui::CheckBox vxgiReflectionsCheckBox;
	lb::gui::Slider vxgiVoxelSizeSlider;
	lb::gui::Slider vxgiRayStepSizeSlider;
	lb::gui::Slider vxgiMaxDistanceSlider;
	lb::gui::Slider speedMultiplierSlider;
	lb::gui::ComboBox shadowTypeComboBox;
	lb::gui::ComboBox shadowProps2DComboBox;
	lb::gui::ComboBox shadowPropsCubeComboBox;
	lb::gui::ComboBox MSAAComboBox;
	lb::gui::CheckBox temporalAACheckBox;
	lb::gui::CheckBox temporalAADebugCheckBox;
	lb::gui::ComboBox textureQualityComboBox;
	lb::gui::Slider mipLodBiasSlider;
	lb::gui::Slider raytraceBounceCountSlider;

	lb::gui::ComboBox tonemapCombo;
	lb::gui::Slider exposureSlider;
	lb::gui::Slider brightnessSlider;
	lb::gui::Slider contrastSlider;
	lb::gui::Slider saturationSlider;
	lb::gui::CheckBox lensFlareCheckBox;
	lb::gui::CheckBox lightShaftsCheckBox;
	lb::gui::Slider lightShaftsStrengthStrengthSlider;
	lb::gui::ComboBox aoComboBox;
	lb::gui::Slider aoPowerSlider;
	lb::gui::Slider aoRangeSlider;
	lb::gui::Slider aoSampleCountSlider;
	lb::gui::CheckBox ssrCheckBox;
	lb::gui::CheckBox raytracedReflectionsCheckBox;
	lb::gui::Slider reflectionsRoughnessCutoffSlider;
	lb::gui::Slider raytracedReflectionsRangeSlider;
	lb::gui::CheckBox screenSpaceShadowsCheckBox;
	lb::gui::Slider screenSpaceShadowsStepCountSlider;
	lb::gui::Slider screenSpaceShadowsRangeSlider;
	lb::gui::CheckBox eyeAdaptionCheckBox;
	lb::gui::Slider eyeAdaptionKeySlider;
	lb::gui::Slider eyeAdaptionRateSlider;
	lb::gui::CheckBox motionBlurCheckBox;
	lb::gui::Slider motionBlurStrengthSlider;
	lb::gui::CheckBox depthOfFieldCheckBox;
	lb::gui::Slider depthOfFieldScaleSlider;
	lb::gui::CheckBox bloomCheckBox;
	lb::gui::Slider bloomStrengthSlider;
	lb::gui::CheckBox fxaaCheckBox;
	lb::gui::CheckBox colorGradingCheckBox;
	lb::gui::CheckBox ditherCheckBox;
	lb::gui::CheckBox sharpenFilterCheckBox;
	lb::gui::Slider sharpenFilterAmountSlider;
	lb::gui::CheckBox outlineCheckBox;
	lb::gui::Slider outlineThresholdSlider;
	lb::gui::Slider outlineThicknessSlider;
	lb::gui::CheckBox chromaticaberrationCheckBox;
	lb::gui::Slider chromaticaberrationSlider;
	lb::gui::CheckBox variableRateShadingClassificationCheckBox;
	lb::gui::CheckBox variableRateShadingClassificationDebugCheckBox;
	lb::gui::CheckBox advancedLightCullingCheckBox;
	lb::gui::CheckBox debugLightCullingCheckBox;
	lb::gui::CheckBox tessellationCheckBox;

	enum RENDERPATH
	{
		RENDERPATH_DEFAULT,
		RENDERPATH_PATHTRACING,
	};
	void ChangeRenderPath(RENDERPATH path);

	void UpdateSwapChainFormats(lb::graphics::SwapChain* swapChain);
	void Update();

	void ResizeLayout() override;

	void ApplySamplerSettings();
};

