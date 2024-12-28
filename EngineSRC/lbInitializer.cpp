#include "lbInitializer.h"
#include "LibertasEngine.h"

#include <string>
#include <thread>
#include <atomic>

namespace lb::initializer
{
	static std::atomic_bool initializationStarted{ false };
	static lb::jobsystem::context ctx;
	static lb::Timer timer;
	static std::atomic_bool systems[INITIALIZED_SYSTEM_COUNT]{};

	void InitializeComponentsImmediate()
	{
		if (IsInitializeFinished())
			return;
		if (!initializationStarted.load())
		{
			InitializeComponentsAsync();
		}
		WaitForInitializationsToFinish();
	}
	void InitializeComponentsAsync()
	{
		if (IsInitializeFinished())
			return;
		timer.record();

		initializationStarted.store(true);

		std::string ss;
		ss += "\nInitializing Libertas Engine, please wait...\n";
		ss += "Version: ";
		ss += lb::version::GetVersionString();
		lb::console::Post(ss);

		size_t shaderdump_count = lb::renderer::GetShaderDumpCount();
		if (shaderdump_count > 0)
		{
			lb::console::Post("\nEmbedded shaders found: " + std::to_string(shaderdump_count));
		}
		else
		{
			lb::console::Post("\nNo embedded shaders found, shaders will be compiled at runtime if needed.\n\tShader source path: " + lb::renderer::GetShaderSourcePath() + "\n\tShader binary path: " + lb::renderer::GetShaderPath());
		}

		lb::console::Post("");
		lb::jobsystem::Initialize();

		lb::console::Post("");
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::font::Initialize(); systems[INITIALIZED_SYSTEM_FONT].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::image::Initialize(); systems[INITIALIZED_SYSTEM_IMAGE].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::input::Initialize(); systems[INITIALIZED_SYSTEM_INPUT].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::renderer::Initialize(); systems[INITIALIZED_SYSTEM_RENDERER].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::texturehelper::Initialize(); systems[INITIALIZED_SYSTEM_TEXTUREHELPER].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::HairParticleSystem::Initialize(); systems[INITIALIZED_SYSTEM_HAIRPARTICLESYSTEM].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::EmittedParticleSystem::Initialize(); systems[INITIALIZED_SYSTEM_EMITTEDPARTICLESYSTEM].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::Ocean::Initialize(); systems[INITIALIZED_SYSTEM_OCEAN].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::gpusortlib::Initialize(); systems[INITIALIZED_SYSTEM_GPUSORTLIB].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::GPUBVH::Initialize(); systems[INITIALIZED_SYSTEM_GPUBVH].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::physics::Initialize(); systems[INITIALIZED_SYSTEM_PHYSICS].store(true); });
		lb::jobsystem::Execute(ctx, [](lb::jobsystem::JobArgs args) { lb::TrailRenderer::Initialize(); systems[INITIALIZED_SYSTEM_TRAILRENDERER].store(true); });

		// Initialize these immediately:
		lb::lua::Initialize(); systems[INITIALIZED_SYSTEM_LUA].store(true);
		lb::audio::Initialize(); systems[INITIALIZED_SYSTEM_AUDIO].store(true);

		std::thread([] {
			lb::jobsystem::Wait(ctx);
			lb::console::Post("\n[lb::initializer] Libertas Engine Initialized (" + std::to_string((int)std::round(timer.elapsed())) + " ms)");
		}).detach();

	}

	bool IsInitializeFinished(INITIALIZED_SYSTEM system)
	{
		if (system == INITIALIZED_SYSTEM_COUNT)
		{
			return initializationStarted.load() && !lb::jobsystem::IsBusy(ctx);
		}
		else
		{
			return systems[system].load();
		}
	}

	void WaitForInitializationsToFinish()
	{
		lb::jobsystem::Wait(ctx);
	}
}
