#include "stdafx.h"
#include "ForceFieldWindow.h"

using namespace lb::ecs;
using namespace lb::scene;

void ForceFieldWindow::Create(EditorComponent* _editor)
{
	editor = _editor;
	lb::gui::Window::Create(ICON_FORCE " Force Field", lb::gui::Window::WindowControls::COLLAPSE | lb::gui::Window::WindowControls::CLOSE | lb::gui::Window::WindowControls::FIT_ALL_WIDGETS_VERTICAL);
	SetSize(XMFLOAT2(420, 120));

	closeButton.SetTooltip("Delete ForceFieldComponent");
	OnClose([=](lb::gui::EventArgs args) {

		lb::Archive& archive = editor->AdvanceHistory();
		archive << EditorComponent::HISTORYOP_COMPONENT_DATA;
		editor->RecordEntity(archive, entity);

		editor->GetCurrentScene().forces.Remove(entity);

		editor->RecordEntity(archive, entity);

		editor->componentsWnd.RefreshEntityTree();
	});

	float x = 60;
	float y = 0;
	float hei = 18;
	float step = hei + 2;
	float wid = 200;

	typeComboBox.Create("Type: ");
	typeComboBox.SetPos(XMFLOAT2(x, y));
	typeComboBox.SetSize(XMFLOAT2(wid, hei));
	typeComboBox.OnSelect([&](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		for (auto& x : editor->translator.selected)
		{
			ForceFieldComponent* force = scene.forces.GetComponent(x.entity);
			if (force == nullptr)
				continue;
			switch (args.iValue)
			{
			case 0:
				force->type = ForceFieldComponent::Type::Point;
				break;
			case 1:
				force->type = ForceFieldComponent::Type::Plane;
				break;
			default:
				assert(0); // error
				break;
			}
		}
	});
	typeComboBox.AddItem("Point", (uint64_t)ForceFieldComponent::Type::Point);
	typeComboBox.AddItem("Plane", (uint64_t)ForceFieldComponent::Type::Plane);
	typeComboBox.SetEnabled(false);
	typeComboBox.SetTooltip("Choose the force field type.");
	AddWidget(&typeComboBox);


	gravitySlider.Create(-10, 10, 0, 100000, "Gravity: ");
	gravitySlider.SetSize(XMFLOAT2(wid, hei));
	gravitySlider.SetPos(XMFLOAT2(x, y += step));
	gravitySlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		for (auto& x : editor->translator.selected)
		{
			ForceFieldComponent* force = scene.forces.GetComponent(x.entity);
			if (force == nullptr)
				continue;
			force->gravity = args.fValue;
		}
	});
	gravitySlider.SetEnabled(false);
	gravitySlider.SetTooltip("Set the amount of gravity. Positive values attract, negatives deflect.");
	AddWidget(&gravitySlider);


	rangeSlider.Create(0.0f, 100.0f, 10, 100000, "Range: ");
	rangeSlider.SetSize(XMFLOAT2(wid, hei));
	rangeSlider.SetPos(XMFLOAT2(x, y += step));
	rangeSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		for (auto& x : editor->translator.selected)
		{
			ForceFieldComponent* force = scene.forces.GetComponent(x.entity);
			if (force == nullptr)
				continue;
			force->range = args.fValue;
		}
	});
	rangeSlider.SetEnabled(false);
	rangeSlider.SetTooltip("Set the range of affection.");
	AddWidget(&rangeSlider);



	SetMinimized(true);
	SetVisible(false);

	SetEntity(INVALID_ENTITY);
}

void ForceFieldWindow::SetEntity(Entity entity)
{
	this->entity = entity;

	const ForceFieldComponent* force = editor->GetCurrentScene().forces.GetComponent(entity);

	if (force != nullptr)
	{
		typeComboBox.SetSelectedByUserdataWithoutCallback((uint64_t)force->type);
		gravitySlider.SetValue(force->gravity);
		rangeSlider.SetValue(force->range);

		gravitySlider.SetEnabled(true);
		rangeSlider.SetEnabled(true);

	}
	else
	{
		gravitySlider.SetEnabled(false);
		rangeSlider.SetEnabled(false);
	}

}

void ForceFieldWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();
	const float padding = 4;
	const float width = GetWidgetAreaSize().x;
	float y = padding;
	float jump = 20;

	const float margin_left = 70;
	const float margin_right = 50;

	auto add = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		widget.SetPos(XMFLOAT2(margin_left, y));
		widget.SetSize(XMFLOAT2(width - margin_left - margin_right, widget.GetScale().y));
		y += widget.GetSize().y;
		y += padding;
	};
	auto add_right = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
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

	add(typeComboBox);
	add(gravitySlider);
	add(rangeSlider);


}
