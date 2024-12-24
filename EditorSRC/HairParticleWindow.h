#pragma once
#include "SpriteRectWindow.h"

class EditorComponent;

class MaterialWindow;

class HairParticleWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity;
	void SetEntity(lb::ecs::Entity entity);

	void UpdateData();

	lb::HairParticleSystem* GetHair();

	lb::gui::Label infoLabel;
	lb::gui::ComboBox meshComboBox;
	lb::gui::Slider lengthSlider;
	lb::gui::Slider widthSlider;
	lb::gui::Slider stiffnessSlider;
	lb::gui::Slider randomnessSlider;
	lb::gui::Slider countSlider;
	lb::gui::Slider segmentcountSlider;
	lb::gui::Slider randomSeedSlider;
	lb::gui::Slider viewDistanceSlider;
	lb::gui::Slider uniformitySlider;

	lb::gui::Button addSpriteButton;
	lb::vector<lb::gui::Button> sprites;
	lb::vector<lb::gui::Button> spriteRemoveButtons;
	lb::vector<lb::gui::Slider> spriteSizeSliders;

	SpriteRectWindow spriterectwnd;

	void RefreshSprites();

	void ResizeLayout() override;
};

