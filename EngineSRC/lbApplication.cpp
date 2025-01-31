#include "lbApplication.h"
#include "lbRenderPath.h"
#include "lbRenderer.h"
#include "lbHelper.h"
#include "lbTimer.h"
#include "lbInput.h"
#include "lbConsole.h"
#include "lbApplication_BindLua.h"
#include "lbVersion.h"
#include "lbEnums.h"
#include "lbTextureHelper.h"
#include "lbProfiler.h"
#include "lbInitializer.h"
#include "lbArguments.h"
#include "lbFont.h"
#include "lbImage.h"
#include "lbEventHandler.h"
#include "lbPlatform.h"

#ifdef PLATFORM_PS5
#include "lbGraphicsDevice_PS5.h"
#else
#include "lbGraphicsDevice_DX12.h"
#include "lbGraphicsDevice_Vulkan.h"
#endif // PLATFORM_PS5

#include <string>
#include <algorithm>
#include <new>
#include <cstdlib>
#include <atomic>

//#define LIBERTAS_ENGINE_HEAP_ALLOCATION_COUNTER

#ifdef LIBERTAS_ENGINE_HEAP_ALLOCATION_COUNTER
static std::atomic<uint32_t> number_of_heap_allocations{ 0 };
static std::atomic<size_t> size_of_heap_allocations{ 0 };
#endif // LIBERTAS_ENGINE_HEAP_ALLOCATION_COUNTER

using namespace lb::graphics;

namespace lb
{

	void Application::Initialize()
	{
		if (initialized)
		{
			return;
		}
		initialized = true;

		lb::initializer::InitializeComponentsAsync();

		alwaysactive = lb::arguments::HasArgument("alwaysactive");

		// Note: lua is always initialized immediately on main thread by lb::initializer, so this is safe to do:
		assert(lb::initializer::IsInitializeFinished(lb::initializer::INITIALIZED_SYSTEM_LUA));
		Luna<lb::lua::Application_BindLua>::push_global(lb::lua::GetLuaState(), "main", this);
		Luna<lb::lua::Application_BindLua>::push_global(lb::lua::GetLuaState(), "application", this);
	}

	void Application::ActivatePath(RenderPath* component, float fadeSeconds, lb::Color fadeColor)
	{
		if (component != nullptr)
		{
			component->init(canvas);
		}

		// Fade manager will activate on fadeout
		fadeManager.Start(fadeSeconds, fadeColor, [this, component]() {

			if (GetActivePath() != nullptr)
			{
				GetActivePath()->Stop();
			}

			if (component != nullptr)
			{
				component->Start();
			}
			activePath = component;
			});

		fadeManager.Update(0); // If user calls ActivatePath without fadeout, it will be instant
	}

	void Application::Run()
	{
		if (!initialized)
		{
			// Initialize in a lazy way, so the user application doesn't have to call this explicitly
			Initialize();
			initialized = true;
		}

		lb::font::UpdateAtlas(canvas.GetDPIScaling());

		ColorSpace colorspace = graphicsDevice->GetSwapChainColorSpace(&swapChain);
		if (colorspace == ColorSpace::HDR10_ST2084)
		{
			// In HDR10, we perform the compositing in a custom linear color space render target
			if (!rendertarget.IsValid())
			{
				TextureDesc desc;
				desc.width = swapChain.desc.width;
				desc.height = swapChain.desc.height;
				desc.format = Format::R11G11B10_FLOAT;
				desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
				bool success = graphicsDevice->CreateTexture(&desc, nullptr, &rendertarget);
				assert(success);
				graphicsDevice->SetName(&rendertarget, "Application::rendertarget");
			}
		}
		else
		{
			// If swapchain is SRGB or Linear HDR, it can be used for blending
			//	- If it is SRGB, the render path will ensure tonemapping to SDR
			//	- If it is Linear HDR, we can blend trivially in linear space
			rendertarget = {};
		}

		if (!lb::initializer::IsInitializeFinished())
		{
			// Until engine is not loaded, present initialization screen...
			CommandList cmd = graphicsDevice->BeginCommandList();
			if (rendertarget.IsValid())
			{
				RenderPassImage rp[] = {
					RenderPassImage::RenderTarget(&rendertarget, RenderPassImage::LoadOp::CLEAR),
				};
				graphicsDevice->RenderPassBegin(rp, arraysize(rp), cmd);
			}
			else
			{
				graphicsDevice->RenderPassBegin(&swapChain, cmd);
			}
			Viewport viewport;
			viewport.width = (float)swapChain.desc.width;
			viewport.height = (float)swapChain.desc.height;
			graphicsDevice->BindViewports(1, &viewport, cmd);
			if (lb::initializer::IsInitializeFinished(lb::initializer::INITIALIZED_SYSTEM_FONT))
			{
				lb::console::DrawOutputText(canvas, cmd, colorspace);
			}
			graphicsDevice->RenderPassEnd(cmd);

			if (rendertarget.IsValid())
			{
				// In HDR10, we perform a final mapping from linear to HDR10, into the swapchain
				graphicsDevice->RenderPassBegin(&swapChain, cmd);
				lb::image::Params fx;
				fx.enableFullScreen();
				fx.enableHDR10OutputMapping();
				lb::image::Draw(&rendertarget, fx, cmd);
				graphicsDevice->RenderPassEnd(cmd);
			}
			graphicsDevice->SubmitCommandLists();
			return;
		}

		static bool startup_script = false;
		if (!startup_script)
		{
			startup_script = true;
			std::string startup_lua_filename = lb::helper::GetCurrentPath() + "/startup.lua";
			if (lb::helper::FileExists(startup_lua_filename))
			{
				if (lb::lua::RunFile(startup_lua_filename))
				{
					lb::console::Post("Executed startup file: " + startup_lua_filename);
				}
			}
			std::string startup_luab_filename = lb::helper::GetCurrentPath() + "/startup.luab";
			if (lb::helper::FileExists(startup_luab_filename))
			{
				if (lb::lua::RunBinaryFile(startup_luab_filename))
				{
					lb::console::Post("Executed startup file: " + startup_luab_filename);
				}
			}
		}

		if (!is_window_active && !alwaysactive)
		{
			// If the application is not active, disable Update loops:
			deltaTimeAccumulator = 0;
			lb::helper::Sleep(10);
			lb::input::Update(window, canvas); // update input while inactive, this solves a problem with past inputs processed immediately after activation
			timer.record_elapsed_seconds(); // after application becomes active, delta time shouldn't spike, could blow up gameplay or physics
			return;
		}

		lb::profiler::BeginFrame();

		deltaTime = float(timer.record_elapsed_seconds());

		const float target_deltaTime = 1.0f / targetFrameRate;
		if (framerate_lock && deltaTime < target_deltaTime)
		{
			lb::helper::QuickSleep((target_deltaTime - deltaTime) * 1000);
			deltaTime += float(timer.record_elapsed_seconds());
		}

		// avoid instability caused by large delta time
		deltaTime = clamp(deltaTime, 0.0f, 0.5f);

		lb::input::Update(window, canvas);

		// Wake up the events that need to be executed on the main thread, in thread safe manner:
		lb::eventhandler::FireEvent(lb::eventhandler::EVENT_THREAD_SAFE_POINT, 0);

		fadeManager.Update(deltaTime);

		if (GetActivePath() != nullptr)
		{
			GetActivePath()->colorspace = colorspace;
			GetActivePath()->init(canvas);
			GetActivePath()->PreUpdate();
		}

		// Fixed time update:
		auto range = lb::profiler::BeginRangeCPU("Fixed Update");
		{
			if (frameskip)
			{
				deltaTimeAccumulator += deltaTime;
				if (deltaTimeAccumulator > 10)
				{
					// application probably lost control, fixed update would take too long
					deltaTimeAccumulator = 0;
				}

				const float targetFrameRateInv = 1.0f / targetFrameRate;
				while (deltaTimeAccumulator >= targetFrameRateInv)
				{
					FixedUpdate();
					deltaTimeAccumulator -= targetFrameRateInv;
				}
			}
			else
			{
				FixedUpdate();
			}
		}
		lb::profiler::EndRange(range); // Fixed Update

		// Variable-timed update:
		Update(deltaTime);

		Render();

		// Begin final compositing:
		CommandList cmd = graphicsDevice->BeginCommandList();
		lb::image::SetCanvas(canvas);
		lb::font::SetCanvas(canvas);
		Viewport viewport;
		viewport.width = (float)swapChain.desc.width;
		viewport.height = (float)swapChain.desc.height;
		graphicsDevice->BindViewports(1, &viewport, cmd);

		if (rendertarget.IsValid())
		{
			RenderPassImage rp[] = {
				RenderPassImage::RenderTarget(&rendertarget, RenderPassImage::LoadOp::CLEAR),
			};
			graphicsDevice->RenderPassBegin(rp, arraysize(rp), cmd);
		}
		else
		{
			graphicsDevice->RenderPassBegin(&swapChain, cmd);
		}
		Compose(cmd);
		graphicsDevice->RenderPassEnd(cmd);

		if (rendertarget.IsValid())
		{
			// In HDR10, we perform a final mapping from linear to HDR10, into the swapchain
			graphicsDevice->RenderPassBegin(&swapChain, cmd);
			lb::image::Params fx;
			fx.enableFullScreen();
			fx.enableHDR10OutputMapping();
			lb::image::Draw(&rendertarget, fx, cmd);
			graphicsDevice->RenderPassEnd(cmd);
		}

		lb::input::ClearForNextFrame();
		lb::profiler::EndFrame(cmd);
		graphicsDevice->SubmitCommandLists();
	}

	void Application::Update(float dt)
	{
		auto range = lb::profiler::BeginRangeCPU("Update");

		infoDisplay.rect = {};

		lb::lua::SetDeltaTime(double(dt));
		lb::lua::Update();

		lb::console::Update(canvas, dt);

		lb::resourcemanager::UpdateStreamingResources(dt);

		if (GetActivePath() != nullptr)
		{
			GetActivePath()->Update(dt);
			GetActivePath()->PostUpdate();
		}

		lb::profiler::EndRange(range); // Update
	}

	void Application::FixedUpdate()
	{
		lb::lua::FixedUpdate();

		if (GetActivePath() != nullptr)
		{
			GetActivePath()->FixedUpdate();
		}
	}

	void Application::Render()
	{
		auto range = lb::profiler::BeginRangeCPU("Render");

		lb::lua::Render();

		if (GetActivePath() != nullptr)
		{
			GetActivePath()->Render();
		}

		lb::profiler::EndRange(range); // Render
	}

	void Application::Compose(CommandList cmd)
	{
		auto range = lb::profiler::BeginRangeCPU("Compose");
		ColorSpace colorspace = graphicsDevice->GetSwapChainColorSpace(&swapChain);

		if (GetActivePath() != nullptr)
		{
			GetActivePath()->Compose(cmd);
		}

		if (fadeManager.IsActive())
		{
			// display fade rect
			lb::image::Params fx;
			fx.enableFullScreen();
			fx.color = fadeManager.color;
			fx.opacity = fadeManager.opacity;
			lb::image::Draw(nullptr, fx, cmd);
		}

		// Draw the information display
		if (infoDisplay.active)
		{
			if (infoDisplay.rect.right > 0)
			{
				graphicsDevice->BindScissorRects(1, &infoDisplay.rect, cmd);
			}

			infodisplay_str.clear();
			if (infoDisplay.watermark)
			{
				infodisplay_str += "Libertas Engine ";
				infodisplay_str += lb::version::GetVersionString();
				infodisplay_str += " ";

#if defined(_ARM)
				infodisplay_str += "[ARM]";
#elif defined(_WIN64)
				infodisplay_str += "[64-bit]";
#elif defined(_WIN32)
				infodisplay_str += "[32-bit]";
#endif // _ARM

#ifdef LIBERTASENGINE_BUILD_DX12
				if (dynamic_cast<GraphicsDevice_DX12*>(graphicsDevice.get()))
				{
					infodisplay_str += "[DX12]";
				}
#endif // LIBERTASENGINE_BUILD_DX12
#ifdef LIBERTASENGINE_BUILD_VULKAN
				if (dynamic_cast<GraphicsDevice_Vulkan*>(graphicsDevice.get()))
				{
					infodisplay_str += "[Vulkan]";
				}
#endif // LIBERTASENGINE_BUILD_VULKAN
#ifdef PLATFORM_PS5
				if (dynamic_cast<GraphicsDevice_PS5*>(graphicsDevice.get()))
				{
					infodisplay_str += "[PS5]";
				}
#endif // PLATFORM_PS5

#ifdef _DEBUG
				infodisplay_str += "[DEBUG]";
#endif // _DEBUG
				if (graphicsDevice->IsDebugDevice())
				{
					infodisplay_str += "[debugdevice]";
				}
				infodisplay_str += "\n";
			}
			if (infoDisplay.device_name)
			{
				infodisplay_str += "Device: ";
				infodisplay_str += graphicsDevice->GetAdapterName();
				infodisplay_str += "\n";
			}
			if (infoDisplay.resolution)
			{
				infodisplay_str += "Resolution: ";
				infodisplay_str += std::to_string(canvas.GetPhysicalWidth());
				infodisplay_str += " x ";
				infodisplay_str += std::to_string(canvas.GetPhysicalHeight());
				infodisplay_str += " (";
				infodisplay_str += std::to_string(int(canvas.GetDPI()));
				infodisplay_str += " dpi)\n";
			}
			if (infoDisplay.logical_size)
			{
				infodisplay_str += "Logical Size: ";
				infodisplay_str += std::to_string(int(canvas.GetLogicalWidth()));
				infodisplay_str += " x ";
				infodisplay_str += std::to_string(int(canvas.GetLogicalHeight()));
				infodisplay_str += "\n";
			}
			if (infoDisplay.colorspace)
			{
				infodisplay_str += "Color Space: ";
				ColorSpace colorSpace = graphicsDevice->GetSwapChainColorSpace(&swapChain);
				switch (colorSpace)
				{
				default:
				case lb::graphics::ColorSpace::SRGB:
					infodisplay_str += "sRGB";
					break;
				case lb::graphics::ColorSpace::HDR10_ST2084:
					infodisplay_str += "ST.2084 (HDR10)";
					break;
				case lb::graphics::ColorSpace::HDR_LINEAR:
					infodisplay_str += "Linear (HDR)";
					break;
				}
				infodisplay_str += "\n";
			}
			if (infoDisplay.fpsinfo)
			{
				deltatimes[fps_avg_counter++ % arraysize(deltatimes)] = deltaTime;
				float displaydeltatime = deltaTime;
				if (fps_avg_counter > arraysize(deltatimes))
				{
					float avg_time = 0;
					for (int i = 0; i < arraysize(deltatimes); ++i)
					{
						avg_time += deltatimes[i];
					}
					displaydeltatime = avg_time / arraysize(deltatimes);
				}

				infodisplay_str += std::to_string(int(std::round(1.0f / displaydeltatime))) + " FPS\n";
			}
			if (infoDisplay.heap_allocation_counter)
			{
				infodisplay_str += "Heap allocations per frame: ";
#ifdef LIBERTAS_ENGINE_HEAP_ALLOCATION_COUNTER
				infodisplay_str += std::to_string(number_of_heap_allocations.load());
				infodisplay_str += " (";
				infodisplay_str += std::to_string(size_of_heap_allocations.load());
				infodisplay_str += " bytes)\n";
				number_of_heap_allocations.store(0);
				size_of_heap_allocations.store(0);
#else
				infodisplay_str += "[disabled]\n";
#endif // LIBERTAS_ENGINE_HEAP_ALLOCATION_COUNTER
			}
			if (infoDisplay.pipeline_count)
			{
				infodisplay_str += "Graphics pipelines active: ";
				infodisplay_str += std::to_string(graphicsDevice->GetActivePipelineCount());
				infodisplay_str += "\n";
			}
			if (infoDisplay.pipeline_creation && lb::renderer::IsPipelineCreationActive())
			{
				infodisplay_str += "Shader pipeline creation is running...\n";
			}

			lb::font::Params params = lb::font::Params(
				4 + canvas.PhysicalToLogical((uint32_t)infoDisplay.rect.left),
				4 + canvas.PhysicalToLogical((uint32_t)infoDisplay.rect.top),
				infoDisplay.size,
				lb::font::WIFALIGN_LEFT,
				lb::font::WIFALIGN_TOP,
				lb::Color::White(),
				lb::Color::Shadow()
			);
			params.shadow_softness = 0.4f;

			// Explanation: this compose pass is in LINEAR space if display output is linear or HDR10
			//	If HDR10, the HDR10 output mapping will be performed on whole image later when drawing to swapchain
			if (colorspace != ColorSpace::SRGB)
			{
				params.enableLinearOutputMapping(9);
			}

			params.cursor = lb::font::Draw(infodisplay_str, params, cmd);

			// VRAM:
			{
				GraphicsDevice::MemoryUsage vram = graphicsDevice->GetMemoryUsage();
				bool warn = false;
				if (vram.usage > vram.budget)
				{
					params.color = lb::Color::Error();
					warn = true;
				}
				else if (float(vram.usage) / float(vram.budget) > 0.9f)
				{
					params.color = lb::Color::Warning();
					warn = true;
				}
				if (infoDisplay.vram_usage || warn)
				{
					params.cursor = lb::font::Draw("VRAM usage: " + std::to_string(vram.usage / 1024 / 1024) + "MB / " + std::to_string(vram.budget / 1024 / 1024) + "MB\n", params, cmd);
					params.color = lb::Color::White();
				}
			}

			// Write warnings below:
			params.color = lb::Color::Warning();
#ifdef _DEBUG
			params.cursor = lb::font::Draw("Warning: This is a [DEBUG] build, performance will be slow!\n", params, cmd);
#endif
			if (graphicsDevice->IsDebugDevice())
			{
				params.cursor = lb::font::Draw("Warning: Graphics is in [debugdevice] mode, performance will be slow!\n", params, cmd);
			}

			// Write errors below:
			params.color = lb::Color::Error();
			if (lb::renderer::GetShaderMissingCount() > 0)
			{
				params.cursor = lb::font::Draw(std::to_string(lb::renderer::GetShaderMissingCount()) + " shaders missing! Check the console for more information!\n", params, cmd);
			}
			if (lb::renderer::GetShaderErrorCount() > 0)
			{
				params.cursor = lb::font::Draw(std::to_string(lb::renderer::GetShaderErrorCount()) + " shader compilation errors! Check the console for more information!\n", params, cmd);
			}
			if (lb::console::GetUnseenLogLevelMax() >= lb::console::LogLevel::Error)
			{
				params.cursor = lb::font::Draw("Errors found, check the console for more information!", params, cmd);
			}

			if (infoDisplay.colorgrading_helper)
			{
				lb::image::Draw(
					lb::texturehelper::getColorGradeDefault(),
					lb::image::Params(
						canvas.PhysicalToLogical((uint32_t)infoDisplay.rect.left),
						canvas.PhysicalToLogical((uint32_t)infoDisplay.rect.top),
						canvas.PhysicalToLogical(256),
						canvas.PhysicalToLogical(16)
					),
					cmd
				);
			}

			if (infoDisplay.rect.right > 0)
			{
				Rect rect;
				rect.right = canvas.width;
				rect.bottom = canvas.height;
				graphicsDevice->BindScissorRects(1, &rect, cmd);
			}
		}

		lb::profiler::DrawData(canvas, 4, 10, cmd, colorspace);

		lb::console::Draw(canvas, cmd, colorspace);

		lb::profiler::EndRange(range); // Compose
	}

	void Application::SetWindow(lb::platform::window_type window)
	{
		this->window = window;

		// User can also create a graphics device if custom logic is desired, but they must do before this function!
		if (graphicsDevice == nullptr)
		{
			ValidationMode validationMode = ValidationMode::Disabled;
			if (lb::arguments::HasArgument("debugdevice"))
			{
				validationMode = ValidationMode::Enabled;
			}
			if (lb::arguments::HasArgument("gpuvalidation"))
			{
				validationMode = ValidationMode::GPU;
			}
			if (lb::arguments::HasArgument("gpu_verbose"))
			{
				validationMode = ValidationMode::Verbose;
			}

			GPUPreference preference = GPUPreference::Discrete;
			if (lb::arguments::HasArgument("igpu"))
			{
				preference = GPUPreference::Integrated;
			}

#ifdef PLATFORM_PS5
			lb::renderer::SetShaderPath(lb::renderer::GetShaderPath() + "ps5/");
			graphicsDevice = std::make_unique<GraphicsDevice_PS5>(validationMode);

#else
			bool use_dx12 = lb::arguments::HasArgument("dx12");
			bool use_vulkan = lb::arguments::HasArgument("vulkan");

#ifndef LIBERTASENGINE_BUILD_DX12
			if (use_dx12) {
				lb::helper::messageBox("The engine was built without DX12 support!", "Error");
				use_dx12 = false;
			}
#endif // LIBERTASENGINE_BUILD_DX12
#ifndef LIBERTASENGINE_BUILD_VULKAN
			if (use_vulkan) {
				lb::helper::messageBox("The engine was built without Vulkan support!", "Error");
				use_vulkan = false;
			}
#endif // LIBERTASENGINE_BUILD_VULKAN

			if (!use_dx12 && !use_vulkan)
			{
#if defined(LIBERTASENGINE_BUILD_DX12)
				use_dx12 = true;
#elif defined(LIBERTASENGINE_BUILD_VULKAN)
				use_vulkan = true;
#else
				lb::console::Post("No rendering backend is enabled! Please enable at least one so we can use it as default", lb::console::LogLevel::Error);
				assert(false);
#endif
			}
			assert(use_dx12 || use_vulkan);

			if (use_vulkan)
			{
#ifdef LIBERTASENGINE_BUILD_VULKAN
				lb::renderer::SetShaderPath(lb::renderer::GetShaderPath() + "spirv/");
				graphicsDevice = std::make_unique<GraphicsDevice_Vulkan>(window, validationMode, preference);
#endif
			}
			else if (use_dx12)
			{
#ifdef LIBERTASENGINE_BUILD_DX12
#ifdef PLATFORM_XBOX
				lb::renderer::SetShaderPath(lb::renderer::GetShaderPath() + "hlsl6_xs/");
#else
				lb::renderer::SetShaderPath(lb::renderer::GetShaderPath() + "hlsl6/");
#endif // PLATFORM_XBOX
				graphicsDevice = std::make_unique<GraphicsDevice_DX12>(validationMode, preference);
#endif
			}
#endif // PLATFORM_PS5
		}
		lb::graphics::GetDevice() = graphicsDevice.get();

		rendertarget = {};
		canvas.init(window);

		SwapChainDesc desc;
		if (swapChain.IsValid())
		{
			// it will only resize, but keep format and other settings
			desc = swapChain.desc;
		}
		else
		{
			// initialize for the first time
			desc.buffer_count = 3;
			if (graphicsDevice->CheckCapability(GraphicsDeviceCapability::R9G9B9E5_SHAREDEXP_RENDERABLE))
			{
				desc.format = Format::R9G9B9E5_SHAREDEXP;
			}
			else
			{
				desc.format = Format::R10G10B10A2_UNORM;
			}
		}
		desc.width = canvas.GetPhysicalWidth();
		desc.height = canvas.GetPhysicalHeight();
		desc.allow_hdr = allow_hdr;
		bool success = graphicsDevice->CreateSwapChain(&desc, window, &swapChain);
		assert(success);

#ifdef PLATFORM_PS5
		// PS5 swapchain resolution was decided in CreateSwapchain(), so reinit canvas:
		canvas.init(swapChain.desc.width, swapChain.desc.height);
#endif // PLATFORM_PS5

		swapChainVsyncChangeEvent = lb::eventhandler::Subscribe(lb::eventhandler::EVENT_SET_VSYNC, [this](uint64_t userdata) {
			SwapChainDesc desc = swapChain.desc;
			desc.vsync = userdata != 0;
			bool success = graphicsDevice->CreateSwapChain(&desc, nullptr, &swapChain);
			assert(success);
			});

	}

	void Application::SetFullScreen(bool fullscreen)
	{
#if defined(PLATFORM_WINDOWS_DESKTOP)

		// Based on: https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
		static WINDOWPLACEMENT wp = {};
		DWORD dwStyle = GetWindowLong(window, GWL_STYLE);
		bool currently_windowed = dwStyle & WS_OVERLAPPEDWINDOW;
		if (currently_windowed && fullscreen) {
			MONITORINFO mi = { sizeof(mi) };
			if (GetWindowPlacement(window, &wp) &&
				GetMonitorInfo(MonitorFromWindow(window,
					MONITOR_DEFAULTTOPRIMARY), &mi)) {
				SetWindowLong(window, GWL_STYLE,
					dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(window, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
		}
		else if (!currently_windowed && !fullscreen) {
			SetWindowLong(window, GWL_STYLE,
				dwStyle | WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(window, &wp);
			SetWindowPos(window, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}

#elif defined(PLATFORM_LINUX)
		SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
#endif // PLATFORM_WINDOWS_DESKTOP
	}

}


#ifdef LIBERTAS_ENGINE_HEAP_ALLOCATION_COUNTER
// Heap alloc replacements are used to count heap allocations:
//	It is good practice to reduce the amount of heap allocations that happen during the frame,
//	so keep an eye on the info display of the engine while Application::InfoDisplayer::heap_allocation_counter is enabled

void* operator new(std::size_t size) {
	number_of_heap_allocations.fetch_add(1);
	size_of_heap_allocations.fetch_add(size);
	void* p = malloc(size);
	if (!p) throw std::bad_alloc();
	return p;
}
void* operator new[](std::size_t size) {
	number_of_heap_allocations.fetch_add(1);
	size_of_heap_allocations.fetch_add(size);
	void* p = malloc(size);
	if (!p) throw std::bad_alloc();
	return p;
}
void* operator new[](std::size_t size, const std::nothrow_t&) throw() {
	number_of_heap_allocations.fetch_add(1);
	size_of_heap_allocations.fetch_add(size);
	return malloc(size);
}
void* operator new(std::size_t size, const std::nothrow_t&) throw() {
	number_of_heap_allocations.fetch_add(1);
	size_of_heap_allocations.fetch_add(size);
	return malloc(size);
}
void operator delete(void* ptr) throw() { free(ptr); }
void operator delete (void* ptr, const std::nothrow_t&) throw() { free(ptr); }
void operator delete[](void* ptr) throw() { free(ptr); }
void operator delete[](void* ptr, const std::nothrow_t&) throw() { free(ptr); }
#endif // LIBERTAS_ENGINE_HEAP_ALLOCATION_COUNTER
