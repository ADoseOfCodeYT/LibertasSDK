#include "stdafx.h"
#include "NameWindow.h"

using namespace lb::ecs;
using namespace lb::scene;

void NameWindow::Create(EditorComponent* _editor)
{
	editor = _editor;
	lb::gui::Window::Create(ICON_NAME " Name", lb::gui::Window::WindowControls::COLLAPSE | lb::gui::Window::WindowControls::CLOSE);
	SetSize(XMFLOAT2(360, 60));

	closeButton.SetTooltip("Delete NameComponent");
	OnClose([=](lb::gui::EventArgs args) {

		lb::Archive& archive = editor->AdvanceHistory();
		archive << EditorComponent::HISTORYOP_COMPONENT_DATA;
		editor->RecordEntity(archive, entity);

		editor->GetCurrentScene().names.Remove(entity);

		editor->RecordEntity(archive, entity);

		editor->componentsWnd.RefreshEntityTree();
		});

	float x = 60;
	float y = 0;
	float step = 25;
	float siz = 250;
	float hei = 20;

	nameInput.Create("");
	nameInput.SetDescription("Name: ");
	nameInput.SetPos(XMFLOAT2(x, y));
	nameInput.SetSize(XMFLOAT2(siz, hei));
	nameInput.OnInputAccepted([=](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		for (auto& x : editor->translator.selected)
		{
			NameComponent* name = scene.names.GetComponent(x.entity);
			if (name == nullptr)
			{
				name = &editor->GetCurrentScene().names.Create(x.entity);
			}
			name->name = args.sValue;
		}
		editor->componentsWnd.RefreshEntityTree();
	});
	AddWidget(&nameInput);

	SetMinimized(true);
	SetVisible(false);

	SetEntity(INVALID_ENTITY);
}

void NameWindow::SetEntity(Entity entity)
{
	this->entity = entity;

	if (entity != INVALID_ENTITY)
	{
		SetEnabled(true);

		NameComponent* name = editor->GetCurrentScene().names.GetComponent(entity);
		if (name != nullptr)
		{
			nameInput.SetValue(name->name);
		}
	}
	else
	{
		SetEnabled(false);
		nameInput.SetValue("Select entity to modify name...");
	}
}

void NameWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();

	nameInput.SetPos(XMFLOAT2(60, 4));
	nameInput.SetSize(XMFLOAT2(GetSize().x - 65, nameInput.GetSize().y));
}
