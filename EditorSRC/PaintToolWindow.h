#pragma once
class EditorComponent;

class PaintToolWindow : public lb::gui::Window
{
	float rot = 0;
	float stroke_dist = 0;
	size_t history_textureIndex = 0;
	struct TextureSlot
	{
		lb::graphics::Texture texture;
		int srgb_subresource = -1;
	};
	lb::vector<TextureSlot> history_textures;
	TextureSlot GetEditTextureSlot(const lb::scene::MaterialComponent& material, int* uvset = nullptr);
	void ReplaceEditTextureSlot(lb::scene::MaterialComponent& material, const TextureSlot& textureslot);

	lb::unordered_map<lb::ecs::Entity, lb::Archive> historyStartDatas;

	struct SculptingIndex
	{
		size_t ind;
		float affection;
	};
	lb::vector<SculptingIndex> sculpting_indices;
	XMFLOAT3 sculpting_normal = XMFLOAT3(0, 0, 0);

	lb::Resource brushTex;
	lb::Resource revealTex;

	struct Stroke
	{
		XMFLOAT2 position;
		float pressure;
	};
	std::deque<Stroke> strokes;

public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;

	lb::gui::ComboBox modeComboBox;
	lb::gui::Label infoLabel;
	lb::gui::Slider radiusSlider;
	lb::gui::Slider amountSlider;
	lb::gui::Slider smoothnessSlider;
	lb::gui::Slider spacingSlider;
	lb::gui::Slider rotationSlider;
	lb::gui::Slider stabilizerSlider;
	lb::gui::CheckBox backfaceCheckBox;
	lb::gui::CheckBox wireCheckBox;
	lb::gui::CheckBox pressureCheckBox;
	lb::gui::CheckBox alphaCheckBox;
	lb::gui::CheckBox terrainCheckBox;
	lb::gui::ColorPicker colorPicker;
	lb::gui::ComboBox textureSlotComboBox;
	lb::gui::ComboBox brushShapeComboBox;
	lb::gui::Button saveTextureButton;
	lb::gui::Button brushTextureButton;
	lb::gui::Button revealTextureButton;
	lb::gui::ComboBox axisCombo;

	void Update(float dt);
	void DrawBrush(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const;

	XMFLOAT2 pos = XMFLOAT2(0, 0);
	lb::scene::PickResult brushIntersect;

	enum MODE
	{
		MODE_DISABLED,
		MODE_TEXTURE,
		MODE_VERTEXCOLOR,
		MODE_SCULPTING_ADD,
		MODE_SCULPTING_SUBTRACT,
		MODE_SOFTBODY_PINNING,
		MODE_SOFTBODY_PHYSICS,
		MODE_HAIRPARTICLE_ADD_TRIANGLE,
		MODE_HAIRPARTICLE_REMOVE_TRIANGLE,
		MODE_HAIRPARTICLE_LENGTH,
		MODE_TERRAIN_MATERIAL,
		MODE_WIND,
	};
	MODE GetMode() const;

	enum class AxisLock
	{
		Disabled,
		X,
		Y,
		Z
	};

	lb::vector<lb::gui::Button> terrain_material_buttons;
	size_t terrain_material_layer = 0;

	float texture_paint_radius = 50;
	float vertex_paint_radius = 0.1f;
	float terrain_paint_radius = 5;

	lb::Archive* currentHistory = nullptr;
	void WriteHistoryData(lb::ecs::Entity entity, lb::Archive& archive, lb::graphics::CommandList cmd = lb::graphics::CommandList());
	void RecordHistory(lb::ecs::Entity entity, lb::graphics::CommandList cmd = lb::graphics::CommandList());
	void ConsumeHistoryOperation(lb::Archive& archive, bool undo);

	void ResizeLayout() override;

	void RecreateTerrainMaterialButtons();
};
