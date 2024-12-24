#pragma once
#include "wiRenderPath2D.h"
#include "wiColor.h"
#include "wiJobSystem.h"
#include "wiVector.h"

#include <functional>

namespace lb
{

	class Application;

	class LoadingScreen :
		public RenderPath2D
	{
	protected:
		lb::jobsystem::context ctx;
		lb::vector<std::function<void(lb::jobsystem::JobArgs)>> tasks;
		std::function<void()> finish;
		uint32_t launchedTasks = 0;
	public:
		lb::Resource backgroundTexture;

		enum class BackgroundMode
		{
			Fill,	// fill the whole screen, will cut off parts of the image if aspects don't match
			Fit,	// fit the image completely inside the screen, will result in black bars on screen if aspects don't match
			Stretch	// fill the whole screen, and stretch the image if needed
		} background_mode = BackgroundMode::Fill;

		//Add a loading task which should be executed
		void addLoadingFunction(std::function<void(lb::jobsystem::JobArgs)> loadingFunction);
		//Helper for loading a whole renderable component
		void addLoadingComponent(RenderPath* component, Application* main, float fadeSeconds = 0, lb::Color fadeColor = lb::Color(0, 0, 0, 255));
		//Set a function that should be called when the loading finishes
		void onFinished(std::function<void()> finishFunction);
		//See if the loading is currently running
		bool isActive() const;
		// See if there are any loading tasks that are still not finished
		bool isFinished() const;
		// Returns the percentage of loading tasks that are finished (0% - 100%)
		int getProgress() const;

		//Start Executing the tasks and mark the loading as active
		void Start() override;

		void Compose(lb::graphics::CommandList cmd) const override;
	};

}
