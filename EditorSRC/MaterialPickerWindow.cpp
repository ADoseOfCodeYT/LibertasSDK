#include "stdafx.h"
#include "MaterialWindow.h"

using namespace lb::graphics;
using namespace lb::ecs;
using namespace lb::scene;

void MaterialPickerWindow::Create(EditorComponent* _editor)
{
	editor = _editor;
	lb::gui::Window::Create("Materials", lb::gui::Window::WindowControls::CLOSE | lb::gui::Window::WindowControls::RESIZE_RIGHT);
	SetText("Materials " ICON_MATERIALBROWSER);
	SetSize(XMFLOAT2(300, 400));

	zoomSlider.Create(10, 100, 100, 100 - 10, "Zoom: ");
	AddWidget(&zoomSlider);

	SetVisible(false);
}

void MaterialPickerWindow::RecreateButtons()
{
	if (editor == nullptr)
		return;
	const lb::scene::Scene& scene = editor->GetCurrentScene();
	for (auto& x : buttons)
	{
		RemoveWidget(&x);
	}
	buttons.resize(scene.materials.GetCount());

	for (size_t i = 0; i < scene.materials.GetCount(); ++i)
	{
		Entity entity = scene.materials.GetEntity(i);

		lb::gui::Button& button = buttons[i];
		button.Create("");
		AddWidget(&button);
		button.SetVisible(false);

		button.OnClick([entity, this](lb::gui::EventArgs args) {

			lb::Archive& archive = editor->AdvanceHistory();
			archive << EditorComponent::HISTORYOP_SELECTION;
			// record PREVIOUS selection state...
			editor->RecordSelection(archive);

			if (!editor->translator.selected.empty() && lb::input::Down(lb::input::KEYBOARD_BUTTON_LSHIFT))
			{
				// Union selection:
				lb::vector<lb::scene::PickResult> saved = editor->translator.selected;
				editor->translator.selected.clear();
				for (const lb::scene::PickResult& picked : saved)
				{
					editor->AddSelected(picked);
				}
				editor->AddSelected(entity);
			}
			else
			{
				// Replace selection:
				editor->translator.selected.clear();
				editor->AddSelected(entity);
			}

			// record NEW selection state...
			editor->RecordSelection(archive);
			});
	}

	ResizeLayout();
}

void MaterialPickerWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();

	if (editor == nullptr || IsCollapsed() || !IsVisible())
	{
		return;
	}

	const lb::scene::Scene& scene = editor->GetCurrentScene();
	if (buttons.size() != scene.materials.GetCount())
	{
		RecreateButtons();
		return;
	}

	zoomSlider.SetPos(XMFLOAT2(55, 0));
	zoomSlider.SetSize(XMFLOAT2(GetWidgetAreaSize().x - 100 - 5, 20));

	lb::gui::Theme theme;
	theme.image.CopyFrom(sprites[lb::gui::IDLE].params);
	theme.image.background = false;
	theme.image.blendFlag = lb::enums::BLENDMODE_ALPHA;
	theme.font.CopyFrom(font.params);
	theme.shadow_color = lb::Color::lerp(theme.font.color, lb::Color::Transparent(), 0.25f);
	theme.tooltipFont.CopyFrom(tooltipFont.params);
	theme.tooltipImage.CopyFrom(tooltipSprite.params);

	const float preview_size = zoomSlider.GetValue();
	const float border = 20 * preview_size / 100.0f;
	int cells = std::max(1, int(GetWidgetAreaSize().x / (preview_size + border)));
	float offset_y = border + zoomSlider.GetSize().y;

	for (size_t i = 0; i < scene.materials.GetCount(); ++i)
	{
		const MaterialComponent& material = scene.materials[i];
		Entity entity = scene.materials.GetEntity(i);

		lb::gui::Button& button = buttons[i];
		button.SetVisible(IsVisible() && !IsCollapsed());

		button.SetTheme(theme);
		button.SetColor(lb::Color::White());
		button.SetColor(lb::Color(255, 255, 255, 150), lb::gui::IDLE);
		button.SetShadowRadius(0);

		for (auto& picked : editor->translator.selected)
		{
			if (picked.entity == entity)
			{
				button.SetColor(lb::Color::White());
				button.SetShadowRadius(3);
				break;
			}
		}

		// find first good texture that we can show:
		button.SetImage({});
		for (auto& slot : material.textures)
		{
			if (slot.resource.IsValid())
			{
				button.SetImage(slot.resource);
				break;
			}
		}

		const NameComponent* name = scene.names.GetComponent(entity);
		if (name != nullptr)
		{
			if (preview_size >= 75)
			{
				button.SetText(name->name);
			}
			else
			{
				button.SetText("");
			}
			button.SetTooltip(name->name);
		}
		button.font.params.h_align = lb::font::WIFALIGN_CENTER;
		button.font.params.v_align = lb::font::WIFALIGN_BOTTOM;

		button.SetSize(XMFLOAT2(preview_size, preview_size));
		button.SetPos(XMFLOAT2((i % cells) * (preview_size + border) + border, offset_y));
		if ((i % cells) == (cells - 1))
		{
			offset_y += preview_size + border;
		}
	}
}
