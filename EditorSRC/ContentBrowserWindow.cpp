#include "stdafx.h"
#include "ContentBrowserWindow.h"

using namespace lb::ecs;
using namespace lb::scene;

void ContentBrowserWindow::Create(EditorComponent* _editor)
{
	editor = _editor;
	control_size = 30;

	lb::gui::Window::Create("Content Browser");

	RemoveWidget(&scrollbar_horizontal);

	SetVisible(false);
}

static const float separator = 140;
void ContentBrowserWindow::Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const
{
	lb::gui::Window::Render(canvas, cmd);

	if (IsVisible() && !IsCollapsed())
	{
		ApplyScissor(canvas, scissorRect, cmd);
		lb::image::Params params;
		params.pos = XMFLOAT3(translation.x + separator, translation.y + control_size, 0);
		params.siz = XMFLOAT2(2, scale.y - control_size);
		params.color = shadow_color;
		lb::image::Draw(nullptr, params, cmd);
	}
}

void ContentBrowserWindow::Update(const lb::Canvas& canvas, float dt)
{
	lb::gui::Window::Update(canvas, dt);

	SetShadowRadius(6);

	static const float radius = 15;

	for (auto& x : folderButtons)
	{
		x.font.params.h_align = lb::font::WIFALIGN_LEFT;
		x.SetShadowRadius(0);
	}
	for (auto& x : itemButtons)
	{
		if (x.sprites[lb::gui::IDLE].textureResource.IsValid())
		{
			x.sprites[lb::gui::IDLE].params.color = lb::Color::White();
		}
		x.SetShadowRadius(4);
	}

	openFolderButton.SetShadowRadius(0);

	XMFLOAT4 color_on = sprites[lb::gui::FOCUS].params.color;
	XMFLOAT4 color_off = sprites[lb::gui::IDLE].params.color;

	for (auto& x : folderButtons)
	{
		x.sprites[lb::gui::IDLE].params.color = color_off;
	}
	if (current_selection < SELECTION_COUNT)
	{
		folderButtons[current_selection].sprites[lb::gui::IDLE].params.color = color_on;
	}
}

void ContentBrowserWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();
	const float padding = 4;
	const float width = GetWidgetAreaSize().x;
	float y = padding;

	openFolderButton.Detach();
	openFolderButton.SetPos(XMFLOAT2(translation.x + padding, translation.y + scale.y - openFolderButton.GetSize().y - padding));
	openFolderButton.SetSize(XMFLOAT2(separator - padding * 2, openFolderButton.GetSize().y));
	openFolderButton.AttachTo(this);

	size_t seli = 0;
	for (auto& x : folderButtons)
	{
		x.Detach();
		x.SetPos(XMFLOAT2(translation.x + padding, translation.y + control_size + y));
		x.SetSize(XMFLOAT2(separator - padding * 2, x.GetSize().y));
		y += x.GetSize().y + padding;
		x.AttachTo(this);
		seli++;
		if (seli == SELECTION_RECENTFOLDER_BEGIN)
			y += x.GetSize().y + padding;
	}

	y = padding + 10;

	float sep = separator + 10;
	float hoffset = sep;
	for (auto& x : itemButtons)
	{
		x.SetPos(XMFLOAT2(hoffset, y));
		hoffset += x.GetSize().x + 15;
		if (hoffset + x.GetSize().x >= width)
		{
			hoffset = sep;
			y += x.GetSize().y + 40;
		}
	}
}

void ContentBrowserWindow::RefreshContent()
{
	content_folder = lb::helper::GetCurrentPath() + "/Content/";
	lb::helper::MakePathAbsolute(content_folder);
	if (!lb::helper::FileExists(content_folder))
	{
		content_folder = lb::helper::GetCurrentPath() + "/../Content/";
		lb::helper::MakePathAbsolute(content_folder);
	}

	float hei = 25;
	float wid = 120;

	for (auto& x : folderButtons)
	{
		RemoveWidget(&x);
	}

	if (!editor->recentFilenames.empty())
	{
		lb::gui::Button& button = folderButtons[SELECTION_RECENT];
		button.Create("Recently Used");
		button.SetTooltip("List all recently used files, not grouped by folders.");
		button.SetLocalizationEnabled(false);
		button.SetSize(XMFLOAT2(wid, hei));
		button.OnClick([this](lb::gui::EventArgs args) {
			SetSelection(SELECTION_RECENT);
			});
		AddWidget(&button, lb::gui::Window::AttachmentOptions::NONE);
		if (current_selection == SELECTION_COUNT)
		{
			current_selection = SELECTION_RECENT;
		}
	}
	if (lb::helper::DirectoryExists(content_folder + "models"))
	{
		lb::gui::Button& button = folderButtons[SELECTION_MODELS];
		button.Create("Models");
		button.SetTooltip(content_folder + "models");
		button.SetLocalizationEnabled(false);
		button.SetSize(XMFLOAT2(wid, hei));
		button.OnClick([this](lb::gui::EventArgs args) {
			SetSelection(SELECTION_MODELS);
			});
		AddWidget(&button, lb::gui::Window::AttachmentOptions::NONE);
		if (current_selection == SELECTION_COUNT)
		{
			current_selection = SELECTION_MODELS;
		}
	}
	if (lb::helper::DirectoryExists(content_folder + "scripts"))
	{
		lb::gui::Button& button = folderButtons[SELECTION_SCRIPTS];
		button.Create("Scripts");
		button.SetTooltip(content_folder + "scripts");
		button.SetLocalizationEnabled(false);
		button.SetSize(XMFLOAT2(wid, hei));
		button.OnClick([this](lb::gui::EventArgs args) {
			SetSelection(SELECTION_SCRIPTS);
		});
		AddWidget(&button, lb::gui::Window::AttachmentOptions::NONE);
		if (current_selection == SELECTION_COUNT)
		{
			current_selection = SELECTION_SCRIPTS;
		}
	}

	for (size_t i = 0; i < editor->recentFolders.size(); ++i)
	{
		lb::gui::Button& button = folderButtons[SELECTION_RECENTFOLDER_BEGIN + i];
		std::string folder = editor->recentFolders[editor->recentFolders.size() - 1 - i];
		while (!folder.empty() && (folder.back() == '/' || folder.back() == '\\'))
		{
			folder.pop_back();
		}
		
		auto last_slash = folder.find_last_of("/\\");
		if (last_slash != folder.npos) {
			folder = folder.substr(last_slash);
		}
		button.Create(folder);
		button.SetTooltip(editor->recentFolders[editor->recentFolders.size() - 1 - i]); // full folder name!
		button.SetLocalizationEnabled(false);
		button.SetSize(XMFLOAT2(wid, hei));
		button.OnClick([this, i](lb::gui::EventArgs args) {
			SetSelection((SELECTION)(SELECTION_RECENTFOLDER_BEGIN + i));
		});
		AddWidget(&button, lb::gui::Window::AttachmentOptions::NONE);
		if (current_selection == SELECTION_COUNT)
		{
			current_selection = (SELECTION)(SELECTION_RECENTFOLDER_BEGIN + i);
		}
	}

	if (current_selection != SELECTION_COUNT)
	{
		SetSelection(current_selection);
	}
}
void ContentBrowserWindow::SetSelection(SELECTION selection)
{
	lb::eventhandler::Subscribe_Once(lb::eventhandler::EVENT_THREAD_SAFE_POINT, [=](uint64_t userdata) {
		current_selection = selection;
		for (auto& x : itemButtons)
		{
			RemoveWidget(&x);
		}
		itemButtons.clear();

		RemoveWidget(&openFolderButton);
		openFolderButton.Create(ICON_OPEN " Go to location");
		openFolderButton.SetSize(XMFLOAT2(60, 60));

		switch (selection)
		{
		case SELECTION_SCRIPTS:
			AddItems(content_folder + "scripts/", "lua", ICON_SCRIPT);
			openFolderButton.OnClick([this](lb::gui::EventArgs args) {
				lb::helper::OpenUrl(content_folder + "scripts/");
				});
			AddWidget(&openFolderButton, lb::gui::Window::AttachmentOptions::NONE);
			break;
		case SELECTION_MODELS:
			AddItems(content_folder + "models/", "wiscene", ICON_OBJECT);
			AddItems(content_folder + "models/", "vrm", ICON_HUMANOID);
			AddItems(content_folder + "models/", "gltf", ICON_OBJECT);
			AddItems(content_folder + "models/", "glb", ICON_OBJECT);
			AddItems(content_folder + "models/", "fbx", ICON_OBJECT);
			AddItems(content_folder + "models/", "obj", ICON_OBJECT);
			openFolderButton.OnClick([this](lb::gui::EventArgs args) {
				lb::helper::OpenUrl(content_folder + "models/");
				});
			AddWidget(&openFolderButton, lb::gui::Window::AttachmentOptions::NONE);
			break;
		case SELECTION_RECENT:
			for (size_t i = 0; i < editor->recentFilenames.size(); ++i)
			{
				const std::string& filename = editor->recentFilenames[editor->recentFilenames.size() - 1 - i];
				std::string ext = lb::helper::toUpper(lb::helper::GetExtensionFromFileName(filename));
				if (!ext.compare("LUA"))
				{
					AddItem(filename, ICON_SCRIPT);
				}
				if (!ext.compare("WISCENE"))
				{
					AddItem(filename, ICON_OBJECT);
				}
				if (!ext.compare("VRM"))
				{
					AddItem(filename, ICON_HUMANOID);
				}
				if (!ext.compare("GLTF"))
				{
					AddItem(filename, ICON_OBJECT);
				}
				if (!ext.compare("GLB"))
				{
					AddItem(filename, ICON_OBJECT);
				}
				if (!ext.compare("FBX"))
				{
					AddItem(filename, ICON_OBJECT);
				}
				if (!ext.compare("OBJ"))
				{
					AddItem(filename, ICON_OBJECT);
				}
			}
			break;
		default:
			{
				size_t i = selection - SELECTION_RECENTFOLDER_BEGIN;
				i = std::min(i, editor->recentFolders.size() - 1);
				const std::string& folder = editor->recentFolders[editor->recentFolders.size() - 1 - i];
				AddItems(folder, "wiscene", ICON_OBJECT);
				AddItems(folder, "vrm", ICON_HUMANOID);
				AddItems(folder, "gltf", ICON_OBJECT);
				AddItems(folder, "glb", ICON_OBJECT);
				AddItems(folder, "fbx", ICON_OBJECT);
				AddItems(folder, "obj", ICON_OBJECT);
				AddItems(folder, "lua", ICON_SCRIPT);
				openFolderButton.OnClick([this, folder](lb::gui::EventArgs args) {
					lb::helper::OpenUrl(folder);
				});
				AddWidget(&openFolderButton, lb::gui::Window::AttachmentOptions::NONE);
			}
			break;
		}

		for (auto& x : itemButtons)
		{
			AddWidget(&x);
		}

		// Refresh theme:
		editor->generalWnd.themeCombo.SetSelected(editor->generalWnd.themeCombo.GetSelected());

	});
}
void ContentBrowserWindow::AddItems(const std::string& folder, const std::string& extension, const std::string& icon)
{
	// Folders parse:
	lb::helper::GetFolderNamesInDirectory(folder, [&](std::string foldername) {
		std::string itemname = foldername.substr(folder.size()) + "." + extension;
		std::string filename = foldername + "/" + itemname;
		if (lb::helper::FileExists(filename))
		{
			AddItem(filename, icon);
		}
	});

	// Individual items:
	lb::helper::GetFileNamesInDirectory(folder, [&](std::string filename) {
		AddItem(filename, icon);
		}, extension);
}
void ContentBrowserWindow::AddItem(const std::string& filename, const std::string& icon)
{
	static const XMFLOAT2 siz = XMFLOAT2(240, 120);

	if (!lb::helper::FileExists(filename))
		return;

	std::string itemname = lb::helper::GetFileNameFromPath(filename);
	std::string foldername = lb::helper::GetDirectoryFromPath(filename);
	std::string ext = lb::helper::toUpper(lb::helper::GetExtensionFromFileName(filename));

	lb::gui::Button& button = itemButtons.emplace_back();
	button.Create(icon);
	button.SetSize(siz);
	button.SetLocalizationEnabled(false);
	button.SetDescription(itemname);
	button.SetTooltip(filename);
	button.OnClick([this, filename](lb::gui::EventArgs args) {
		lb::eventhandler::Subscribe_Once(lb::eventhandler::EVENT_THREAD_SAFE_POINT, [=](uint64_t userdata) {
			editor->Open(filename);
			});
		this->SetVisible(false);
		});
	button.font_description.params.h_align = lb::font::WIFALIGN_CENTER;
	button.font_description.params.v_align = lb::font::WIFALIGN_TOP;
	button.font.params.size = 42;
	if (ext.compare("WISCENE") == 0)
	{
		lb::graphics::Texture archiveThumbnail = lb::Archive::PeekThumbnail(filename);
		if (archiveThumbnail.IsValid())
		{
			for (int i = 0; i < arraysize(sprites); ++i)
			{
				button.sprites[i].textureResource.SetTexture(archiveThumbnail);
			}
			button.SetText("");
		}
	}
	else
	{
		std::string thumbnailName = foldername + "/thumbnail.png";
		if (lb::helper::FileExists(thumbnailName))
		{
			lb::Resource thumbnail = lb::resourcemanager::Load(thumbnailName);
			if (thumbnail.IsValid())
			{
				for (int i = 0; i < arraysize(sprites); ++i)
				{
					button.sprites[i].textureResource = thumbnail;
				}
				button.SetText("");
			}
		}
	}
}
