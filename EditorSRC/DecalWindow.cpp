#include "stdafx.h"
#include "DecalWindow.h"

using namespace lb::ecs;
using namespace lb::scene;


void DecalWindow::Create(EditorComponent* _editor)
{
	editor = _editor;
	lb::gui::Window::Create(ICON_DECAL " Decal", lb::gui::Window::WindowControls::COLLAPSE | lb::gui::Window::WindowControls::CLOSE);
	SetSize(XMFLOAT2(300, 200));

	closeButton.SetTooltip("Delete DecalComponent");
	OnClose([=](lb::gui::EventArgs args) {

		lb::Archive& archive = editor->AdvanceHistory();
		archive << EditorComponent::HISTORYOP_COMPONENT_DATA;
		editor->RecordEntity(archive, entity);

		editor->GetCurrentScene().decals.Remove(entity);

		editor->RecordEntity(archive, entity);

		editor->componentsWnd.RefreshEntityTree();
	});

	float x = 200;
	float y = 0;
	float hei = 18;
	float step = hei + 2;

	placementCheckBox.Create("Decal Placement Enabled: ");
	placementCheckBox.SetPos(XMFLOAT2(x, y));
	placementCheckBox.SetSize(XMFLOAT2(hei, hei));
	placementCheckBox.SetCheck(false);
	placementCheckBox.SetTooltip("Enable decal placement. Use the left mouse button to place decals to the scene.");
	AddWidget(&placementCheckBox);

	y += step;

	onlyalphaCheckBox.Create("Alpha only basecolor: ");
	onlyalphaCheckBox.SetSize(XMFLOAT2(hei, hei));
	onlyalphaCheckBox.SetTooltip("You can enable this to only use alpha channel from basecolor map. Useful for blending normalmap-only decals.");
	onlyalphaCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		for (auto& x : editor->translator.selected)
		{
			DecalComponent* decal = scene.decals.GetComponent(x.entity);
			if (decal == nullptr)
				continue;
			decal->SetBaseColorOnlyAlpha(args.bValue);
		}
	});
	AddWidget(&onlyalphaCheckBox);

	slopeBlendPowerSlider.Create(0, 8, 0, 1000, "Slope Blend: ");
	slopeBlendPowerSlider.SetSize(XMFLOAT2(100, hei));
	slopeBlendPowerSlider.SetTooltip("Set a power factor for blending on surface slopes. 0 = no slope blend, increasing = more slope blend");
	slopeBlendPowerSlider.OnSlide([=](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		for (auto& x : editor->translator.selected)
		{
			DecalComponent* decal = scene.decals.GetComponent(x.entity);
			if (decal == nullptr)
				continue;
			decal->slopeBlendPower = args.fValue;
		}
	});
	AddWidget(&slopeBlendPowerSlider);

	y += step;

	infoLabel.Create("");
	infoLabel.SetText("Set decal properties in the Material component. Decals support the following material properties:\n - Base color\n - Base color texture\n - Emissive strength\n - Normalmap texture\n - Normalmap strength\n - Surfacemap texture\n - Texture tiling (TexMulAdd)");
	infoLabel.SetSize(XMFLOAT2(300, 100));
	infoLabel.SetPos(XMFLOAT2(10, y));
	infoLabel.SetColor(lb::Color::Transparent());
	AddWidget(&infoLabel);
	y += infoLabel.GetScale().y - step + 5;

	SetMinimized(true);
	SetVisible(false);

	SetEntity(INVALID_ENTITY);
}

void DecalWindow::SetEntity(Entity entity)
{
	this->entity = entity;

	Scene& scene = editor->GetCurrentScene();
	const DecalComponent* decal = scene.decals.GetComponent(entity);

	if (decal != nullptr)
	{
		SetEnabled(true);
		onlyalphaCheckBox.SetCheck(decal->IsBaseColorOnlyAlpha());
		slopeBlendPowerSlider.SetValue(decal->slopeBlendPower);
	}
	else
	{
		SetEnabled(false);
	}
}

void DecalWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();
	const float padding = 4;
	const float width = GetWidgetAreaSize().x;
	float y = padding;
	float jump = 20;

	auto add = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_left = 100;
		const float margin_right = 40;
		widget.SetPos(XMFLOAT2(margin_left, y));
		widget.SetSize(XMFLOAT2(width - margin_left - margin_right, widget.GetScale().y));
		y += widget.GetSize().y;
		y += padding;
	};
	auto add_right = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_right = padding;
		widget.SetPos(XMFLOAT2(width - margin_right - widget.GetSize().x, y));
		y += widget.GetSize().y;
		y += padding;
	};
	auto add_fullwidth = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_left = padding;
		const float margin_right = padding;
		widget.SetPos(XMFLOAT2(margin_left, y));
		widget.SetSize(XMFLOAT2(width - margin_left - margin_right, widget.GetScale().y));
		y += widget.GetSize().y;
		y += padding;
	};

	add_fullwidth(infoLabel);
	add_right(placementCheckBox);
	add_right(onlyalphaCheckBox);
	add(slopeBlendPowerSlider);

}
