#pragma once
class EditorComponent;

class WeatherWindow : public lb::gui::Window
{
	void UpdateWind();
public:
	void Create(EditorComponent* editor);

	void Update();

	EditorComponent* editor = nullptr;

	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	void SetEntity(lb::ecs::Entity entity);
	lb::scene::WeatherComponent& GetWeather() const;
	void InvalidateProbes() const;

	XMFLOAT3 default_sky_horizon = XMFLOAT3(0, 0, 0);
	XMFLOAT3 default_sky_zenith = XMFLOAT3(0, 0, 0);

	lb::gui::Button primaryButton;
	lb::gui::CheckBox overrideFogColorCheckBox;
	lb::gui::CheckBox heightFogCheckBox;
	lb::gui::Slider fogStartSlider;
	lb::gui::Slider fogDensitySlider;
	lb::gui::Slider fogHeightStartSlider;
	lb::gui::Slider fogHeightEndSlider;
	lb::gui::Slider gravitySlider;
	lb::gui::Slider windSpeedSlider;
	lb::gui::Slider windMagnitudeSlider;
	lb::gui::Slider windDirectionSlider;
	lb::gui::Slider windWaveSizeSlider;
	lb::gui::Slider windRandomnessSlider;
	lb::gui::Slider skyExposureSlider;
	lb::gui::Slider starsSlider;
	lb::gui::Slider skyRotationSlider;
	lb::gui::Slider rainAmountSlider;
	lb::gui::Slider rainLengthSlider;
	lb::gui::Slider rainSpeedSlider;
	lb::gui::Slider rainScaleSlider;
	lb::gui::Slider rainSplashScaleSlider;
	lb::gui::CheckBox realisticskyCheckBox;
	lb::gui::CheckBox aerialperspectiveCheckBox;
	lb::gui::CheckBox realisticskyHighQualityCheckBox;
	lb::gui::CheckBox realisticskyReceiveShadowCheckBox;
	lb::gui::Button skyButton;
	lb::gui::Button colorgradingButton;

	// ocean params:
	lb::gui::CheckBox ocean_enabledCheckBox;
	lb::gui::Slider ocean_patchSizeSlider;
	lb::gui::Slider ocean_waveAmplitudeSlider;
	lb::gui::Slider ocean_choppyScaleSlider;
	lb::gui::Slider ocean_windDependencySlider;
	lb::gui::Slider ocean_timeScaleSlider;
	lb::gui::Slider ocean_heightSlider;
	lb::gui::Slider ocean_detailSlider;
	lb::gui::Slider ocean_toleranceSlider;
	lb::gui::Button ocean_resetButton;

	lb::gui::ComboBox colorComboBox;
	lb::gui::ColorPicker colorPicker;

	// volumetric clouds:
	lb::gui::CheckBox volumetricCloudsCheckBox;
	lb::gui::CheckBox volumetricCloudsReceiveShadowCheckBox;
	lb::gui::CheckBox volumetricCloudsCastShadowCheckBox;
	lb::gui::Slider cloudStartHeightSlider;
	lb::gui::Slider cloudThicknessSlider;
	lb::gui::Slider skewAlongWindDirectionFirstSlider;
	lb::gui::Slider totalNoiseScaleFirstSlider;
	lb::gui::Slider curlScaleFirstSlider;
	lb::gui::Slider curlNoiseHeightFractionFirstSlider;
	lb::gui::Slider curlNoiseModifierFirstSlider;
	lb::gui::Slider detailScaleFirstSlider;
	lb::gui::Slider detailNoiseHeightFractionFirstSlider;
	lb::gui::Slider detailNoiseModifierFirstSlider;
	lb::gui::Slider skewAlongCoverageWindDirectionFirstSlider;
	lb::gui::Slider weatherScaleFirstSlider;
	lb::gui::Slider coverageAmountFirstSlider;
	lb::gui::Slider coverageMinimumFirstSlider;
	lb::gui::Slider typeAmountFirstSlider;
	lb::gui::Slider typeMinimumFirstSlider;
	lb::gui::Slider rainAmountFirstSlider;
	lb::gui::Slider rainMinimumFirstSlider;
	lb::gui::TextInputField gradientSmallFirstTextFields[4];
	lb::gui::TextInputField gradientMediumFirstTextFields[4];
	lb::gui::TextInputField gradientLargeFirstTextFields[4];
	lb::gui::TextInputField anvilDeformationSmallFirstTextFields[4];
	lb::gui::TextInputField anvilDeformationMediumFirstTextFields[4];
	lb::gui::TextInputField anvilDeformationLargeFirstTextFields[4];
	lb::gui::Slider windSpeedFirstSlider;
	lb::gui::Slider windAngleFirstSlider;
	lb::gui::Slider windUpAmountFirstSlider;
	lb::gui::Slider coverageWindSpeedFirstSlider;
	lb::gui::Slider coverageWindAngleFirstSlider;
	lb::gui::Slider coverageAmountSecondSlider;
	lb::gui::Slider coverageMinimumSecondSlider;
	lb::gui::Button volumetricCloudsWeatherMapFirstButton;
	lb::gui::Button volumetricCloudsWeatherMapSecondButton;

	lb::gui::Button preset0Button;
	lb::gui::Button preset1Button;
	lb::gui::Button preset2Button;
	lb::gui::Button preset3Button;
	lb::gui::Button preset4Button;
	lb::gui::Button preset5Button;

	void ResizeLayout() override;
};

