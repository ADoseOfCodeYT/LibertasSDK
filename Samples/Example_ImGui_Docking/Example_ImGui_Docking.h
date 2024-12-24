#pragma once
#include "LibertasEngine.h"


class Example_ImGuiRenderer : public lb::RenderPath3D
{
	lb::gui::Label label;
public:
	void Load() override;
	void Update(float dt) override;
	void ResizeLayout() override;
	void Render() const override;
	void DisplayPerformanceData(bool* p_open);
	void igTextTitle(const char* text);
};

class Example_ImGui : public lb::Application
{
	Example_ImGuiRenderer renderer;

public:
	~Example_ImGui() override;
	void Initialize() override;
	void Compose(lb::graphics::CommandList cmd) override;
};

