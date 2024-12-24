#pragma once
class EditorComponent;

class ContentBrowserWindow : public lb::gui::Window
{
public:
	void Create(EditorComponent* editor);

	EditorComponent* editor = nullptr;

	std::string content_folder;
	enum SELECTION
	{
		SELECTION_RECENT,
		SELECTION_MODELS,
		SELECTION_SCRIPTS,

		SELECTION_RECENTFOLDER_BEGIN,
		SELECTION_RECENTFOLDER_END = SELECTION_RECENTFOLDER_BEGIN + 10,

		SELECTION_COUNT
	};
	SELECTION current_selection = SELECTION_COUNT;
	lb::gui::Button folderButtons[SELECTION_COUNT];
	lb::vector<lb::gui::Button> itemButtons;

	lb::gui::Button openFolderButton;

	void RefreshContent();

	void SetSelection(SELECTION selection);
	void AddItems(const std::string& folder, const std::string& extension, const std::string& icon);
	void AddItem(const std::string& filename, const std::string& icon);

	void Update(const lb::Canvas& canvas, float dt);
	void Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const override;
	void ResizeLayout() override;
};

