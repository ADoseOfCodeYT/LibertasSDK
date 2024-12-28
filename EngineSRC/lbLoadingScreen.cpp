#include "lbLoadingScreen.h"
#include "lbApplication.h"
#include "lbEventHandler.h"

#include <thread>

using namespace lb::graphics;

namespace lb
{

	bool LoadingScreen::IsActive() const
	{
		return lb::jobsystem::IsBusy(ctx);
	}

	bool LoadingScreen::isFinished() const
	{
		return tasks.empty();
	}

	int LoadingScreen::getProgress() const
	{
		if (launchedTasks == 0)
			return 100;
		uint32_t counter = AtomicLoad(&ctx.counter);
		float percent = 1 - float(counter) / float(launchedTasks);
		return (int)std::round(percent * 100);
	}

	void LoadingScreen::addLoadingFunction(std::function<void(lb::jobsystem::JobArgs)> loadingFunction)
	{
		if (loadingFunction != nullptr)
		{
			tasks.push_back(loadingFunction);
		}
	}

	void LoadingScreen::addLoadingComponent(RenderPath* component, Application* main, float fadeSeconds, lb::Color fadeColor)
	{
		addLoadingFunction([=](lb::jobsystem::JobArgs args) {
			component->Load();
			});
		onFinished([=] {
			main->ActivatePath(component, fadeSeconds, fadeColor);
			});
	}

	void LoadingScreen::onFinished(std::function<void()> finishFunction)
	{
		if (finishFunction != nullptr)
			finish = finishFunction;
	}

	void LoadingScreen::Start()
	{
		launchedTasks = (uint32_t)tasks.size();
		for (auto& x : tasks)
		{
			lb::jobsystem::Execute(ctx, x);
		}
		std::thread([this]() {
			lb::jobsystem::Wait(ctx);
			lb::eventhandler::Subscribe_Once(lb::eventhandler::EVENT_THREAD_SAFE_POINT, [this](uint64_t) {
				if (finish != nullptr)
					finish();
				tasks.clear();
				launchedTasks = 0;
				finish = nullptr;
			});
		}).detach();

		RenderPath2D::Start();
	}

	void LoadingScreen::Compose(lb::graphics::CommandList cmd) const
	{
		if (backgroundTexture.IsValid())
		{
			lb::image::Params fx;
			const Texture& tex = backgroundTexture.GetTexture();
			const TextureDesc& desc = tex.GetDesc();

			const float canvas_aspect = GetLogicalWidth() / GetLogicalHeight();
			const float image_aspect = float(desc.width) / float(desc.height);

			switch (background_mode)
			{
			default:
			case lb::LoadingScreen::BackgroundMode::Fill:
				if (canvas_aspect > image_aspect)
				{
					// display aspect is wider than image:
					fx.siz.x = GetLogicalWidth();
					fx.siz.y = GetLogicalHeight() / image_aspect * canvas_aspect;
				}
				else
				{
					// image aspect is wider or equal to display
					fx.siz.x = GetLogicalWidth() / canvas_aspect * image_aspect;
					fx.siz.y = GetLogicalHeight();
				}
				fx.pos = XMFLOAT3(GetLogicalWidth() * 0.5f, GetLogicalHeight() * 0.5f, 0);
				fx.pivot = XMFLOAT2(0.5f, 0.5f);
				break;
			case lb::LoadingScreen::BackgroundMode::Fit:
				if (canvas_aspect > image_aspect)
				{
					// display aspect is wider than image:
					fx.siz.x = GetLogicalWidth() / canvas_aspect * image_aspect;
					fx.siz.y = GetLogicalHeight();
				}
				else
				{
					// image aspect is wider or equal to display
					fx.siz.x = GetLogicalWidth();
					fx.siz.y = GetLogicalHeight() * canvas_aspect / image_aspect;
				}
				fx.pos = XMFLOAT3(GetLogicalWidth() * 0.5f, GetLogicalHeight() * 0.5f, 0);
				fx.pivot = XMFLOAT2(0.5f, 0.5f);
				break;
			case lb::LoadingScreen::BackgroundMode::Stretch:
				fx.enableFullScreen();
				break;
			}

			fx.blendFlag = lb::enums::BLENDMODE_ALPHA;
			if (colorspace != ColorSpace::SRGB)
			{
				fx.enableLinearOutputMapping(hdr_scaling);
			}

			lb::image::Draw(&tex, fx, cmd);
		}

		RenderPath2D::Compose(cmd);
	}

}
