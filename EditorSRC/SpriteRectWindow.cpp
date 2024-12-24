#include "stdafx.h"
#include "Editor.h"
#include "SpriteRectWindow.h"

void SpriteRectWindow::Create(EditorComponent* editor)
{
	lb::gui::Window::Create("Select an area within the atlas...");
	SetLocalizationEnabled(false);

	SetSize(XMFLOAT2(300, 300));

	spriteButton.Create("");
	spriteButton.OnDragStart([=](lb::gui::EventArgs args) {
		XMFLOAT2 pos = spriteButton.GetPos();
		XMFLOAT2 size = spriteButton.GetSize();
		dragStartUV = lb::math::InverseLerp(pos, XMFLOAT2(pos.x + size.x, pos.y + size.y), args.clickPos);
		dragStartUV.x = saturate(dragStartUV.x);
		dragStartUV.y = saturate(dragStartUV.y);
		dragEndUV = dragStartUV;
	});
	spriteButton.OnDrag([=](lb::gui::EventArgs args) {
		XMFLOAT2 pos = spriteButton.GetPos();
		XMFLOAT2 size = spriteButton.GetSize();
		dragEndUV = lb::math::InverseLerp(pos, XMFLOAT2(pos.x + size.x, pos.y + size.y), args.clickPos);
		dragEndUV.x = saturate(dragEndUV.x);
		dragEndUV.y = saturate(dragEndUV.y);
	});
	spriteButton.OnDragEnd([=](lb::gui::EventArgs args) {
		XMFLOAT2 pos = spriteButton.GetPos();
		XMFLOAT2 size = spriteButton.GetSize();
		dragEndUV = lb::math::InverseLerp(pos, XMFLOAT2(pos.x + size.x, pos.y + size.y), args.clickPos);
		dragEndUV.x = saturate(dragEndUV.x);
		dragEndUV.y = saturate(dragEndUV.y);

		muladd = XMFLOAT4(
			saturate(std::abs(dragEndUV.x - dragStartUV.x)),
			saturate(std::abs(dragEndUV.y - dragStartUV.y)),
			saturate(std::min(dragStartUV.x, dragEndUV.x)),
			saturate(std::min(dragStartUV.y, dragEndUV.y))
		);
	});
	AddWidget(&spriteButton);

	okButton.Create("OK");
	okButton.OnClick([=](lb::gui::EventArgs args) {

		if (onAccepted != nullptr)
			onAccepted();

		SetVisible(false);
	});
	AddWidget(&okButton);

	SetPos(XMFLOAT2(200, 100));
}

void SpriteRectWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();

	XMFLOAT2 size = GetWidgetAreaSize();

	spriteButton.SetSize(XMFLOAT2(size.x, size.y - okButton.GetSize().y * 2 - 1));
	spriteButton.SetPos(XMFLOAT2(0, 0));

	okButton.SetSize(XMFLOAT2(size.x, okButton.GetSize().y));
	okButton.SetPos(XMFLOAT2(0, spriteButton.GetSize().y));
}
void SpriteRectWindow::Update(const lb::Canvas& canvas, float dt)
{
	lb::gui::Window::Update(canvas, dt);

	if (!IsVisible())
		return;

	if (GetPointerHitbox().intersects(spriteButton.hitBox))
	{
		lb::input::SetCursor(lb::input::CURSOR_CROSS);
	}
}
void SpriteRectWindow::Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const
{
	lb::gui::Window::Render(canvas, cmd);

	if (!IsVisible())
		return;

	ApplyScissor(canvas, this->scissorRect, cmd);

	XMFLOAT2 pos = spriteButton.GetPos();
	XMFLOAT2 size = spriteButton.GetSize();
	XMFLOAT2 end = XMFLOAT2(pos.x + size.x, pos.y + size.y);

	lb::image::Params params;
	params.sampleFlag = lb::image::SAMPLEMODE_WRAP;
	params.quality = lb::image::QUALITY_NEAREST;
	params.pos = XMFLOAT3(pos.x, pos.y, 0);
	params.siz = size;
	params.color = XMFLOAT4(1.7f, 1.7f, 1.7f, 1);
	params.enableDrawRect(XMFLOAT4(0, 0, size.x / 8, size.y / 8));
	lb::image::Draw(lb::texturehelper::getCheckerBoard(), params, cmd);

	params.disableDrawRect();
	params.quality = lb::image::QUALITY_LINEAR;
	params.color = XMFLOAT4(1, 1, 1, 1);
	lb::image::Draw(sprite.getTexture(), params, cmd);

	XMFLOAT2 rectstart = lb::math::Lerp(pos, end, dragStartUV);
	XMFLOAT2 rectend = lb::math::Lerp(pos, end, dragEndUV);

	params.pos = XMFLOAT3(rectstart.x, rectstart.y, 0);
	params.siz = XMFLOAT2(rectend.x - rectstart.x, rectend.y - rectstart.y);
	params.color = shadow_color;
	params.color.w = 0.5f;
	lb::image::Draw(nullptr, params, cmd);
}

void SpriteRectWindow::SetSprite(const lb::Sprite& sprite)
{
	this->sprite = sprite;
}
void SpriteRectWindow::ResetSelection()
{
	dragStartUV = XMFLOAT2(0, 0);
	dragEndUV = XMFLOAT2(0, 0);
	muladd = XMFLOAT4(1, 1, 0, 0);
}
void SpriteRectWindow::OnAccepted(std::function<void()> cb)
{
	onAccepted = cb;
}
