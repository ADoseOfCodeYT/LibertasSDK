#pragma once
class EditorComponent;

struct ModifierWindow : public lb::gui::Window
{
	lb::terrain::Modifier* modifier = nullptr;
	std::function<void()> generation_callback;
	lb::gui::ComboBox blendCombo;
	lb::gui::Slider weightSlider;
	lb::gui::Slider frequencySlider;

	virtual ~ModifierWindow() = default;

	ModifierWindow(const std::string& name);
	void Bind(lb::terrain::Modifier* ptr);
	void From(lb::terrain::Modifier* ptr);
};
struct PerlinModifierWindow : public ModifierWindow
{
	lb::gui::Slider octavesSlider;

	PerlinModifierWindow();
	void ResizeLayout() override;
	void Bind(lb::terrain::PerlinModifier* ptr);
	void From(lb::terrain::PerlinModifier* ptr);
};
struct VoronoiModifierWindow : public ModifierWindow
{
	lb::gui::Slider fadeSlider;
	lb::gui::Slider shapeSlider;
	lb::gui::Slider falloffSlider;
	lb::gui::Slider perturbationSlider;

	VoronoiModifierWindow();
	void ResizeLayout() override;
	void Bind(lb::terrain::VoronoiModifier* ptr);
	void From(lb::terrain::VoronoiModifier* ptr);
};
struct HeightmapModifierWindow : public ModifierWindow
{
	lb::gui::Slider scaleSlider;
	lb::gui::Button loadButton;

	HeightmapModifierWindow();
	void ResizeLayout() override;
	void Bind(lb::terrain::HeightmapModifier* ptr);
	void From(lb::terrain::HeightmapModifier* ptr);
};

struct PropWindow : public lb::gui::Window
{
	lb::gui::ComboBox meshCombo;
	lb::gui::TextInputField minCountPerChunkInput;
	lb::gui::TextInputField maxCountPerChunkInput;
	lb::gui::ComboBox regionCombo;
	lb::gui::Slider regionPowerSlider;
	lb::gui::Slider noiseFrequencySlider;
	lb::gui::Slider noisePowerSlider;
	lb::gui::Slider thresholdSlider;
	lb::gui::Slider minSizeSlider;
	lb::gui::Slider maxSizeSlider;
	lb::gui::TextInputField minYOffsetInput;
	lb::gui::TextInputField maxYOffsetInput;

	PropWindow(lb::terrain::Prop* prop, lb::scene::Scene* scene);
	void ResizeLayout() override;

	lb::terrain::Prop* prop = nullptr;
	lb::scene::Scene* scene = nullptr;

	std::function<void()> generation_callback;
};

struct PropsWindow : public lb::gui::Window
{
	lb::gui::Button addButton;

	PropsWindow(EditorComponent* editor);

	void Rebuild();
	void AddWindow(lb::terrain::Prop& prop);

	void Update(const lb::Canvas& canvas, float dt) override;

	void ResizeLayout() override;

	EditorComponent* editor = nullptr;
	lb::terrain::Terrain* terrain = nullptr;

	lb::vector<std::unique_ptr<PropWindow>> windows;
	lb::vector<PropWindow*> windows_to_remove;

	std::function<void()> generation_callback;
};

class TerrainWindow : public lb::gui::Window
{
public:
	lb::gui::CheckBox centerToCamCheckBox;
	lb::gui::CheckBox removalCheckBox;
	lb::gui::CheckBox grassCheckBox;
	lb::gui::CheckBox physicsCheckBox;
	lb::gui::CheckBox tessellationCheckBox;
	lb::gui::Slider lodSlider;
	lb::gui::Slider generationSlider;
	lb::gui::Slider propGenerationSlider;
	lb::gui::Slider physicsGenerationSlider;
	lb::gui::Slider propDensitySlider;
	lb::gui::Slider grassDensitySlider;
	lb::gui::Slider grassLengthSlider;
	lb::gui::Slider grassDistanceSlider;
	lb::gui::ComboBox presetCombo;
	lb::gui::Slider scaleSlider;
	lb::gui::Slider seedSlider;
	lb::gui::Slider bottomLevelSlider;
	lb::gui::Slider topLevelSlider;
	lb::gui::Button saveHeightmapButton;
	lb::gui::Button saveRegionButton;
	lb::gui::ComboBox addModifierCombo;

	lb::gui::Slider region1Slider;
	lb::gui::Slider region2Slider;
	lb::gui::Slider region3Slider;

	lb::gui::ComboBox materialCombos[lb::terrain::MATERIAL_COUNT];
	lb::gui::ComboBox materialCombo_GrassParticle;

	std::unique_ptr<PropsWindow> propsWindow;

	enum PRESET
	{
		PRESET_HILLS,
		PRESET_ISLANDS,
		PRESET_MOUNTAINS,
		PRESET_ARCTIC,
		PRESET_DESERT,
	};

	lb::vector<std::unique_ptr<ModifierWindow>> modifiers;
	lb::vector<ModifierWindow*> modifiers_to_remove;

	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;
	lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
	lb::terrain::Terrain* terrain = nullptr;
	void SetEntity(lb::ecs::Entity entity);
	void AddModifier(ModifierWindow* modifier_window);
	void SetupAssets();

	void Update(const lb::Canvas& canvas, float dt) override;
	void ResizeLayout() override;
};

