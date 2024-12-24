#pragma once
class EditorComponent;

class ProfilerWidget : public lb::gui::Widget
{
	void Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const override;
};

class ProfilerWindow : public lb::gui::Window
{
public:
	void Create();

	ProfilerWidget profilerWidget;

	void Update(const lb::Canvas& canvas, float dt) override;
	void ResizeLayout() override;
};
