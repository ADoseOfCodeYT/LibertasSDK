#include "stdafx.h"
#include "ProfilerWindow.h"

using namespace lb::graphics;

void ProfilerWidget::Render(const lb::Canvas& canvas, lb::graphics::CommandList cmd) const
{
	if (!IsVisible())
	{
		lb::profiler::DisableDrawForThisFrame();
		lb::profiler::SetBackgroundColor(lb::Color::fromFloat4(sprites[lb::gui::IDLE].params.color));
		return;
	}

	GraphicsDevice* device = GetDevice();

	ApplyScissor(canvas, scissorRect, cmd);

	lb::profiler::SetBackgroundColor(lb::Color::Transparent());
	lb::profiler::SetTextColor(font.params.color);
	lb::profiler::DrawData(canvas, translation.x + 5, translation.y + 5, cmd);
}

void ProfilerWindow::Create()
{
	Window::Create("Profiler", lb::gui::Window::WindowControls::ALL);

	OnClose([=](lb::gui::EventArgs args) {
		lb::profiler::SetEnabled(false);
	});

	profilerWidget.SetSize(XMFLOAT2(200, 1000));
	AddWidget(&profilerWidget);

	SetSize(XMFLOAT2(300, 400));
	SetPos(XMFLOAT2(5, 100));
	SetVisible(false);
	SetShadowRadius(2);
}

void ProfilerWindow::Update(const lb::Canvas& canvas, float dt)
{
	lb::gui::Window::Update(canvas, dt);
}
void ProfilerWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();

	profilerWidget.SetSize(XMFLOAT2(GetWidgetAreaSize().x - control_size, profilerWidget.GetSize().y));
}
