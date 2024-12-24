#pragma once
#include "Translator.h"
#include "wiScene_BindLua.h"
#include "ComponentsWindow.h"
#include "ProfilerWindow.h"
#include "ContentBrowserWindow.h"
#include "GraphicsWindow.h"
#include "CameraWindow.h"
#include "MaterialPickerWindow.h"
#include "PaintToolWindow.h"
#include "GeneralWindow.h"
#include "IconDefinitions.h"

class Editor;
class EditorComponent : public lb::RenderPath2D
{
public:
	Editor* main = nullptr;

	lb::gui::Button newSceneButton;

	lb::gui::ComboBox newEntityCombo;

	lb::gui::Button translateButton;
	lb::gui::Button rotateButton;
	lb::gui::Button scaleButton;

	lb::gui::Button physicsButton;

	lb::gui::Button dummyButton;
	bool dummy_enabled = false;
	bool dummy_male = false;
	XMFLOAT3 dummy_pos = XMFLOAT3(0, 0, 0);

	lb::gui::Button navtestButton;
	bool navtest_enabled = false;
	lb::scene::PickResult navtest_start_pick;
	lb::scene::PickResult navtest_goal_pick;
	lb::PathQuery navtest_pathquery;

	lb::gui::Button playButton;
	lb::gui::Button stopButton;

	lb::gui::Button saveButton;
	lb::gui::Button openButton;
	lb::gui::Button contentBrowserButton;
	lb::gui::Button logButton;
	lb::gui::Button profilerButton;
	lb::gui::Button cinemaButton;
	lb::gui::Button fullscreenButton;
	lb::gui::Button bugButton;
	lb::gui::Button aboutButton;
	lb::gui::Button exitButton;
	lb::gui::Window aboutWindow;
	lb::gui::Label aboutLabel;

	ComponentsWindow componentsWnd;
	ProfilerWindow profilerWnd;
	ContentBrowserWindow contentBrowserWnd;
	lb::gui::Window topmenuWnd;

	lb::gui::Button generalButton;
	lb::gui::Button graphicsButton;
	lb::gui::Button cameraButton;
	lb::gui::Button materialsButton;
	lb::gui::Button paintToolButton;

	GeneralWindow generalWnd;
	GraphicsWindow graphicsWnd;
	CameraWindow cameraWnd;
	MaterialPickerWindow materialPickerWnd;
	PaintToolWindow paintToolWnd;

	lb::primitive::Ray pickRay;
	lb::physics::PickDragOperation physicsDragOp;

	std::unique_ptr<lb::RenderPath3D> renderPath;
	const lb::graphics::Texture* GetGUIBlurredBackground() const override { return renderPath->GetGUIBlurredBackground(); }

	void ResizeBuffers() override;
	void ResizeLayout() override;
	void Load() override;
	void Start() override;
	void PreUpdate() override;
	void FixedUpdate() override;
	void Update(float dt) override;
	void PostUpdate() override;
	void Render() const override;
	void Compose(lb::graphics::CommandList cmd) const override;

	lb::graphics::Viewport viewport3D;
	lb::primitive::Hitbox2D viewport3D_hitbox;
	void ResizeViewport3D();

	bool camControlStart = true;
	XMFLOAT4 originalMouse = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 currentMouse = XMFLOAT4(0, 0, 0, 0);

	enum EDITORSTENCILREF
	{
		EDITORSTENCILREF_CLEAR = 0x00,
		EDITORSTENCILREF_HIGHLIGHT_OBJECT = 0x01,
		EDITORSTENCILREF_HIGHLIGHT_MATERIAL = 0x02,
		EDITORSTENCILREF_LAST = 0x0F,
	};
	lb::graphics::Texture rt_selectionOutline_MSAA;
	lb::graphics::Texture rt_selectionOutline[2];
	lb::graphics::Texture rt_dummyOutline;
	float outlineTimer = 0;
	const XMFLOAT4 selectionColor = XMFLOAT4(1, 0.6f, 0, 1);
	const XMFLOAT4 selectionColor2 = XMFLOAT4(0, 1, 0.6f, 0.35f);
	lb::Color inactiveEntityColor = lb::Color::fromFloat4(XMFLOAT4(1, 1, 1, 0.5f));
	lb::Color hoveredEntityColor = lb::Color::fromFloat4(XMFLOAT4(1, 1, 1, 1));
	lb::Color backgroundEntityColor = lb::Color::Black();
	lb::Color dummyColor = lb::Color::White();

	lb::graphics::Texture rt_metadataDummies_MSAA;
	lb::graphics::Texture rt_metadataDummies;

	const lb::Color springDebugColor = lb::Color(255, 70, 165, 255);
	const lb::Color ikDebugColor = lb::Color(49, 190, 103, 255);

	lb::graphics::Texture editor_depthbuffer;
	lb::graphics::Texture editor_rendertarget;

	Translator translator;
	lb::scene::PickResult hovered;
	bool inspector_mode = false;
	lb::ecs::Entity grass_interaction_entity = lb::ecs::INVALID_ENTITY;

	void ClearSelected();
	void AddSelected(lb::ecs::Entity entity, bool allow_refocus = true);
	void AddSelected(const lb::scene::PickResult& picked, bool allow_refocus = true);
	bool IsSelected(lb::ecs::Entity entity) const;
	bool selectAll = false;
	lb::unordered_set<lb::ecs::Entity> selectAllStorage;

	bool bone_picking = false;
	void CheckBonePickingEnabled();

	void UpdateDynamicWidgets();

	lb::Archive clipboard;

	enum HistoryOperationType
	{
		HISTORYOP_TRANSLATOR,		// translator interaction
		HISTORYOP_SELECTION,		// selection changed
		HISTORYOP_ADD,				// entity added
		HISTORYOP_DELETE,			// entity removed
		HISTORYOP_COMPONENT_DATA,	// generic component data changed
		HISTORYOP_PAINTTOOL,		// paint tool interaction
		HISTORYOP_NONE
	};

	void RecordSelection(lb::Archive& archive) const;
	void RecordEntity(lb::Archive& archive, lb::ecs::Entity entity);
	void RecordEntity(lb::Archive& archive, const lb::vector<lb::ecs::Entity>& entities);

	void ResetHistory();
	lb::Archive& AdvanceHistory();
	void ConsumeHistoryOperation(bool undo);

	lb::vector<std::string> recentFilenames;
	size_t maxRecentFilenames = 10;
	lb::vector<std::string> recentFolders;
	size_t maxRecentFolders = 8;
	void RegisterRecentlyUsed(const std::string& filename);

	void Open(std::string filename);
	void Save(const std::string& filename);
	void SaveAs();
	bool deleting = false;

	lb::graphics::Texture CreateThumbnailScreenshot() const;

	std::string save_text_message = "";
	std::string save_text_filename = "";
	float save_text_alpha = 0; // seconds until save text disappears
	lb::Color save_text_color = lb::Color::White();
	void PostSaveText(const std::string& message, const std::string& filename = "", float time_seconds = 4);

	std::string last_script_path;

	struct EditorScene
	{
		std::string path;
		lb::scene::Scene scene;
		XMFLOAT3 cam_move = {};
		lb::scene::CameraComponent camera;
		lb::scene::TransformComponent camera_transform;
		lb::scene::TransformComponent camera_target;
		lb::vector<lb::Archive> history;
		int historyPos = -1;
		lb::gui::Button tabSelectButton;
		lb::gui::Button tabCloseButton;
	};
	lb::vector<std::unique_ptr<EditorScene>> scenes;
	int current_scene = 0;
	EditorScene& GetCurrentEditorScene() { return *scenes[current_scene].get(); }
	const EditorScene& GetCurrentEditorScene() const { return *scenes[current_scene].get(); }
	lb::scene::Scene& GetCurrentScene() { return scenes[current_scene].get()->scene; }
	const lb::scene::Scene& GetCurrentScene() const { return scenes[current_scene].get()->scene; }
	void SetCurrentScene(int index);
	void RefreshSceneList();
	void NewScene();

	void FocusCameraOnSelected();

	lb::Localization default_localization;
	lb::Localization current_localization;
	void SetDefaultLocalization();
	void SetLocalization(lb::Localization& loc);
	void ReloadLanguage();

	struct FontData
	{
		std::string name;
		lb::vector<uint8_t> filedata;
	};
	lb::vector<FontData> font_datas;

	lb::jobsystem::context loadmodel_workload;
	lb::SpriteFont loadmodel_font;
};

class Editor : public lb::Application
{
public:
	EditorComponent renderComponent;
	lb::config::File config;

	void Initialize() override;

	void HotReload();

	~Editor() override
	{
		config.Commit();
	}
};

// Additional localizations that are outside the GUI can be defined here:
enum class EditorLocalization
{
	// Top menu:
	Save,
	Open,
	Backlog,
	Profiler,
	__REMOVED_Cinema,
	FullScreen,
	Windowed,
	BugReport,
	About,
	Exit,
	UntitledScene,
	ContentBrowser,

	Count
};
static const char* EditorLocalizationStrings[] = {
	// Top menu:
	"Save",
	"Open",
	"Backlog",
	"Profiler",
	"Cinema",
	"Full screen",
	"Windowed",
	"Bug report",
	"About",
	"Exit",
	"Untitled scene",
	"Content",
};
static_assert(arraysize(EditorLocalizationStrings) == size_t(EditorLocalization::Count));
