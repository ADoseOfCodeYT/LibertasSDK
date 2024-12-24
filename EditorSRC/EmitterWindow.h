#pragma once
class EditorComponent;

class MaterialWindow;

class EmitterWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	void UpdateData();

	lb::EmittedParticleSystem* GetEmitter();

	lb::gui::Button restartButton;
	lb::gui::Button burstButton;
	lb::gui::TextInputField burstCountInput;
	lb::gui::ComboBox meshComboBox;
	lb::gui::ComboBox shaderTypeComboBox;
	lb::gui::Label infoLabel;
	lb::gui::Slider maxParticlesSlider;
	lb::gui::CheckBox sortCheckBox;
	lb::gui::CheckBox depthCollisionsCheckBox;
	lb::gui::CheckBox sphCheckBox;
	lb::gui::CheckBox pauseCheckBox;
	lb::gui::CheckBox debugCheckBox;
	lb::gui::CheckBox volumeCheckBox;
	lb::gui::CheckBox frameBlendingCheckBox;
	lb::gui::CheckBox collidersDisabledCheckBox;
	lb::gui::CheckBox takeColorCheckBox;
	lb::gui::Slider emitCountSlider;
	lb::gui::Slider emitSizeSlider;
	lb::gui::Slider emitRotationSlider;
	lb::gui::Slider emitNormalSlider;
	lb::gui::Slider emitScalingSlider;
	lb::gui::Slider emitLifeSlider;
	lb::gui::Slider emitOpacityCurveStartSlider;
	lb::gui::Slider emitOpacityCurveEndSlider;
	lb::gui::Slider emitRandomnessSlider;
	lb::gui::Slider emitLifeRandomnessSlider;
	lb::gui::Slider emitColorRandomnessSlider;
	lb::gui::Slider emitMotionBlurSlider;
	lb::gui::Slider emitMassSlider;
	lb::gui::Slider timestepSlider;
	lb::gui::Slider dragSlider;
	lb::gui::Slider restitutionSlider;
	lb::gui::TextInputField VelocityXInput;
	lb::gui::TextInputField VelocityYInput;
	lb::gui::TextInputField VelocityZInput;
	lb::gui::TextInputField GravityXInput;
	lb::gui::TextInputField GravityYInput;
	lb::gui::TextInputField GravityZInput;

	lb::gui::Slider sph_h_Slider;
	lb::gui::Slider sph_K_Slider;
	lb::gui::Slider sph_p0_Slider;
	lb::gui::Slider sph_e_Slider;

	lb::gui::TextInputField frameRateInput;
	lb::gui::TextInputField framesXInput;
	lb::gui::TextInputField framesYInput;
	lb::gui::TextInputField frameCountInput;
	lb::gui::TextInputField frameStartInput;

	void ResizeLayout() override;
};

