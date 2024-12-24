#pragma once
class EditorComponent;

class SpriteRectWindow : public lb::gui::Window
{
	std::function<void()> onAccepted;
public:
	void Create(EditorComponent* editor);

	lb::gui::Button spriteButton;
	lb::gui::Button okButton;

	lb::Sprite sprite;
	XMFLOAT2 dragStartUV = XMFLOAT2(0, 0);
	XMFLOAT2 dragEndUV = XMFLOAT2(0, 0);
	XMFLOAT4 muladd = XMFLOAT4(1, 1, 0, 0);

	void SetSprite(const lb::Sprite& sprite);
	void ResetSelection();
	void OnAccepted(std::function<void()> cb);

	void ResizeLayout() override;
	void Update(const lb::Canvas& canvas, float dt) override;
	void Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const override;
};
