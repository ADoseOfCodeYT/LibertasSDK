#include "lbRenderPath3D_PathTracing.h"
#include "lbRenderer.h"
#include "lbImage.h"
#include "lbHelper.h"
#include "lbTextureHelper.h"
#include "lbSprite.h"
#include "lbProfiler.h"
#include "lbScene.h"
#include "lbConsole.h"

#if __has_include("OpenImageDenoise/oidn.hpp")
#include "OpenImageDenoise/oidn.hpp"
#if OIDN_VERSION_MAJOR >= 2
#define OPEN_IMAGE_DENOISE
#pragma comment(lib,"OpenImageDenoise.lib")
// Also provide the required DLL files from OpenImageDenoise release near the exe!
#endif // OIDN_VERSION_MAJOR >= 2
#endif // __has_include("OpenImageDenoise/oidn.hpp")

using namespace lb::graphics;
using namespace lb::scene;


namespace lb
{
#ifdef OPEN_IMAGE_DENOISE
	bool DenoiserCallback(void* userPtr, double n)
	{
		auto renderpath = (RenderPath3D_PathTracing*)userPtr;
		if (renderpath->getProgress() < 1)
		{
			renderpath->denoiserProgress = 0;
			return false;
		}
		renderpath->denoiserProgress = (float)n;
		return true;
	}
	bool RenderPath3D_PathTracing::isDenoiserAvailable() const { return true; }
#else
	bool RenderPath3D_PathTracing::isDenoiserAvailable() const { return false; }
#endif // OPEN_IMAGE_DENOISE

	void RenderPath3D_PathTracing::ResizeBuffers()
	{
		DeleteGPUResources();

		GraphicsDevice* device = lb::graphics::GetDevice();

		XMUINT2 internalResolution = GetInternalResolution();

		{
			TextureDesc desc;
			desc.format = lb::renderer::format_rendertarget_main;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			device->CreateTexture(&desc, nullptr, &rtMain);
			device->SetName(&rtMain, "rtMain");

			desc.bind_flags = BindFlag::UNORDERED_ACCESS | BindFlag::SHADER_RESOURCE | BindFlag::RENDER_TARGET;
			desc.format = Format::R32G32B32A32_FLOAT;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			device->CreateTexture(&desc, nullptr, &traceResult);
			device->SetName(&traceResult, "traceResult");

#ifdef OPEN_IMAGE_DENOISE
			desc.bind_flags = BindFlag::UNORDERED_ACCESS;
			desc.layout = ResourceState::UNORDERED_ACCESS;
			device->CreateTexture(&desc, nullptr, &denoiserAlbedo);
			device->SetName(&denoiserAlbedo, "denoiserAlbedo");
			device->CreateTexture(&desc, nullptr, &denoiserNormal);
			device->SetName(&denoiserNormal, "denoiserNormal");
#endif // OPEN_IMAGE_DENOISE

			{
				desc.bind_flags = BindFlag::UNORDERED_ACCESS | BindFlag::SHADER_RESOURCE;
				desc.layout = ResourceState::UNORDERED_ACCESS;
				desc.format = Format::R32_FLOAT;
				device->CreateTexture(&desc, nullptr, &traceDepth);
				device->SetName(&traceDepth, "traceDepth");
				desc.format = Format::R8_UINT;
				device->CreateTexture(&desc, nullptr, &traceStencil);
				device->SetName(&traceStencil, "traceStencil");

				depthBuffer_Copy = traceDepth;
				depthBuffer_Copy1 = traceDepth;

				desc.layout = ResourceState::DEPTHSTENCIL;
				desc.bind_flags = BindFlag::DEPTH_STENCIL;
				desc.format = lb::renderer::format_depthbuffer_main;
				device->CreateTexture(&desc, nullptr, &depthBuffer_Main);
				device->SetName(&depthBuffer_Main, "depthBuffer_Main");
			}
		}
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = Format::R32_FLOAT;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.mip_levels = 5;
			desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;
			device->CreateTexture(&desc, nullptr, &rtLinearDepth);
			device->SetName(&rtLinearDepth, "rtLinearDepth");

			for (uint32_t i = 0; i < desc.mip_levels; ++i)
			{
				int subresource_index;
				subresource_index = device->CreateSubresource(&rtLinearDepth, SubresourceType::SRV, 0, 1, i, 1);
				assert(subresource_index == i);
				subresource_index = device->CreateSubresource(&rtLinearDepth, SubresourceType::UAV, 0, 1, i, 1);
				assert(subresource_index == i);
			}
		}
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = Format::R11G11B10_FLOAT;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			device->CreateTexture(&desc, nullptr, &rtPostprocess);
			device->SetName(&rtPostprocess, "rtPostprocess");


			desc.width /= 4;
			desc.height /= 4;
			desc.bind_flags = BindFlag::UNORDERED_ACCESS | BindFlag::SHADER_RESOURCE;
			device->CreateTexture(&desc, nullptr, &rtGUIBlurredBackground[0]);
			device->SetName(&rtGUIBlurredBackground[0], "rtGUIBlurredBackground[0]");

			desc.width /= 4;
			desc.height /= 4;
			device->CreateTexture(&desc, nullptr, &rtGUIBlurredBackground[1]);
			device->SetName(&rtGUIBlurredBackground[1], "rtGUIBlurredBackground[1]");
			device->CreateTexture(&desc, nullptr, &rtGUIBlurredBackground[2]);
			device->SetName(&rtGUIBlurredBackground[2], "rtGUIBlurredBackground[2]");
		}

		lb::renderer::CreateLuminanceResources(luminanceResources, internalResolution);
		lb::renderer::CreateBloomResources(bloomResources, internalResolution);

		setLightShaftsEnabled(getLightShaftsEnabled());
		setVolumeLightsEnabled(getVolumeLightsEnabled());

		resetProgress();

		RenderPath2D::ResizeBuffers(); // we don't need to use any buffers from RenderPath3D, so skip those
	}

	void RenderPath3D_PathTracing::Update(float dt)
	{
		setOcclusionCullingEnabled(false);

		if (camera->IsDirty())
		{
			camera->SetDirty(false);
			resetProgress();
		}
		else
		{
			for (size_t i = 0; i < scene->transforms.GetCount(); ++i)
			{
				const TransformComponent& transform = scene->transforms[i];

				if (transform.IsDirty())
				{
					sam = -1;
					break;
				}
			}

			if (sam >= 0)
			{
				for (size_t i = 0; i < scene->materials.GetCount(); ++i)
				{
					const MaterialComponent& material = scene->materials[i];

					if (material.IsDirty())
					{
						sam = -1;
						break;
					}
				}
			}
		}
		sam++;

		if (sam > target)
		{
			sam = target;
		}

		scene->SetAccelerationStructureUpdateRequested(sam == 0);
		setSceneUpdateEnabled(sam == 0);

		RenderPath3D::Update(dt);

#ifdef OPEN_IMAGE_DENOISE
		if (sam == target)
		{
			if (!denoiserResult.IsValid() && !lb::jobsystem::IsBusy(denoiserContext))
			{
				//lb::helper::saveTextureToFile(denoiserAlbedo, "C:/PROJECTS/WickedEngine/Editor/_albedo.png");
				//lb::helper::saveTextureToFile(denoiserNormal, "C:/PROJECTS/WickedEngine/Editor/_normal.png");

				texturedata_src.clear();
				texturedata_dst.clear();
				texturedata_albedo.clear();
				texturedata_normal.clear();

				if (lb::helper::saveTextureToMemory(traceResult, texturedata_src))
				{
					lb::helper::saveTextureToMemory(denoiserAlbedo, texturedata_albedo);
					lb::helper::saveTextureToMemory(denoiserNormal, texturedata_normal);

					texturedata_dst.resize(texturedata_src.size());

					lb::jobsystem::Execute(denoiserContext, [&](lb::jobsystem::JobArgs args) {

						size_t width = (size_t)traceResult.desc.width;
						size_t height = (size_t)traceResult.desc.height;
						{
							// https://github.com/OpenImageDenoise/oidn#c11-api-example

							// Create an Intel Open Image Denoise device
							static oidn::DeviceRef device = oidn::newDevice();
							static bool init = false;
							if (!init)
							{
								device.commit();
								init = true;
							}

							oidn::BufferRef texturedata_src_buffer = device.newBuffer(texturedata_src.size());
							oidn::BufferRef texturedata_dst_buffer = device.newBuffer(texturedata_dst.size());
							oidn::BufferRef texturedata_albedo_buffer;
							oidn::BufferRef texturedata_normal_buffer;

							texturedata_src_buffer.write(0, texturedata_src.size(), texturedata_src.data());

							// Create a denoising filter
							oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
							filter.setImage("color", texturedata_src_buffer, oidn::Format::Float3, width, height, 0, sizeof(XMFLOAT4));
							if (!texturedata_albedo.empty())
							{
								texturedata_albedo_buffer = device.newBuffer(texturedata_albedo.size());
								texturedata_albedo_buffer.write(0, texturedata_albedo.size(), texturedata_albedo.data());
								filter.setImage("albedo", texturedata_albedo_buffer, oidn::Format::Float3, width, height, 0, sizeof(XMFLOAT4)); // optional
							}
							if (!texturedata_normal.empty())
							{
								texturedata_normal_buffer = device.newBuffer(texturedata_normal.size());
								texturedata_normal_buffer.write(0, texturedata_normal.size(), texturedata_normal.data());
								filter.setImage("normal", texturedata_normal_buffer, oidn::Format::Float3, width, height, 0, sizeof(XMFLOAT4)); // optional
							}
							filter.setImage("output", texturedata_dst_buffer, oidn::Format::Float3, width, height, 0, sizeof(XMFLOAT4));
							filter.set("hdr", true); // image is HDR
							//filter.set("cleanAux", true);
							filter.commit();

							denoiserProgress = 0;
							filter.setProgressMonitorFunction(DenoiserCallback, this);

							// Filter the image
							filter.execute();

							// Check for errors
							const char* errorMessage;
							auto error = device.getError(errorMessage);
							if (error != oidn::Error::None && error != oidn::Error::Cancelled)
							{
								lb::console::Post(std::string("[OpenImageDenoise error] ") + errorMessage);
							}
							else
							{
								texturedata_dst_buffer.read(0, texturedata_dst.size(), texturedata_dst.data());
							}
						}

						GraphicsDevice* device = lb::graphics::GetDevice();

						TextureDesc desc;
						desc.width = (uint32_t)width;
						desc.height = (uint32_t)height;
						desc.bind_flags = BindFlag::SHADER_RESOURCE;
						desc.format = Format::R32G32B32A32_FLOAT;

						SubresourceData initdata;
						initdata.data_ptr = texturedata_dst.data();
						initdata.row_pitch = uint32_t(sizeof(XMFLOAT4) * width);
						device->CreateTexture(&desc, &initdata, &denoiserResult);

						});
				}
			}
		}
		else
		{
			denoiserResult = Texture();
			denoiserProgress = 0;
		}
#endif // OPEN_IMAGE_DENOISE
	}

	void RenderPath3D_PathTracing::Render() const
	{
		GraphicsDevice* device = lb::graphics::GetDevice();
		lb::jobsystem::context ctx;

		if (sam < target)
		{
			CommandList cmd_copypages;
			if (scene->terrains.GetCount() > 0)
			{
				cmd_copypages = device->BeginCommandList(QUEUE_COPY);
				lb::jobsystem::Execute(ctx, [this, cmd_copypages](lb::jobsystem::JobArgs args) {
					for (size_t i = 0; i < scene->terrains.GetCount(); ++i)
					{
						scene->terrains[i].CopyVirtualTexturePageStatusGPU(cmd_copypages);
					}
					});
			}

			// Setup:
			CommandList cmd = device->BeginCommandList();
			lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

				lb::renderer::BindCameraCB(
					*camera,
					camera_previous,
					camera_reflection,
					cmd
				);
				lb::renderer::UpdateRenderData(visibility_main, frameCB, cmd);

				if (scene->IsAccelerationStructureUpdateRequested())
				{
					lb::renderer::UpdateRaytracingAccelerationStructures(*scene, cmd);
				}
			});

			// Main scene:
			cmd = device->BeginCommandList();
			if (cmd_copypages.IsValid())
			{
				device->WaitCommandList(cmd, cmd_copypages);
			}
			lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

				GraphicsDevice* device = lb::graphics::GetDevice();

				lb::renderer::BindCameraCB(
					*camera,
					camera_previous,
					camera_reflection,
					cmd
				);
				lb::renderer::BindCommonResources(cmd);
				lb::renderer::UpdateRenderDataAsync(visibility_main, frameCB, cmd);

				if (scene->weather.IsRealisticSky())
				{
					lb::renderer::ComputeSkyAtmosphereTextures(cmd);
					lb::renderer::ComputeSkyAtmosphereSkyViewLut(cmd);
				}

				if (lb::renderer::GetRaytraceDebugBVHVisualizerEnabled())
				{
					RenderPassImage rp[] = {
						RenderPassImage::RenderTarget(&traceResult, RenderPassImage::LoadOp::CLEAR)
					};
					device->RenderPassBegin(rp, arraysize(rp), cmd);

					Viewport vp;
					vp.width = (float)traceResult.GetDesc().width;
					vp.height = (float)traceResult.GetDesc().height;
					device->BindViewports(1, &vp, cmd);

					lb::renderer::RayTraceSceneBVH(*scene, cmd);

					device->RenderPassEnd(cmd);
				}
				else
				{
					auto range = lb::profiler::BeginRangeGPU("Traced Scene", cmd);

					lb::renderer::RayTraceScene(
						*scene,
						traceResult,
						sam,
						cmd,
						denoiserAlbedo.IsValid() ? &denoiserAlbedo : nullptr,
						denoiserNormal.IsValid() ? &denoiserNormal : nullptr,
						&traceDepth,
						&traceStencil,
						&depthBuffer_Main
					);

					lb::profiler::EndRange(range); // Traced Scene
				}

				if (getVolumeLightsEnabled() && visibility_main.IsRequestedVolumetricLights())
				{
					lb::renderer::DrawShadowmaps(visibility_main, cmd);
				}

			});

			if (scene->terrains.GetCount() > 0)
			{
				CommandList cmd_allocation_tilerequest = device->BeginCommandList(QUEUE_COMPUTE);
				device->WaitCommandList(cmd_allocation_tilerequest, cmd); // wait for opaque scene
				lb::jobsystem::Execute(ctx, [this, cmd_allocation_tilerequest](lb::jobsystem::JobArgs args) {
					for (size_t i = 0; i < scene->terrains.GetCount(); ++i)
					{
						scene->terrains[i].AllocateVirtualTextureTileRequestsGPU(cmd_allocation_tilerequest);
					}
					});

				CommandList cmd_writeback_tilerequest = device->BeginCommandList(QUEUE_COPY);
				device->WaitCommandList(cmd_writeback_tilerequest, cmd_allocation_tilerequest);
				lb::jobsystem::Execute(ctx, [this, cmd_writeback_tilerequest](lb::jobsystem::JobArgs args) {
					for (size_t i = 0; i < scene->terrains.GetCount(); ++i)
					{
						scene->terrains[i].WritebackTileRequestsGPU(cmd_writeback_tilerequest);
					}
					});
			}
		}

		// Composite, tonemap etc:
		CommandList cmd = device->BeginCommandList();
		lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

			GraphicsDevice* device = lb::graphics::GetDevice();

			lb::renderer::BindCameraCB(
				*camera,
				camera_previous,
				camera_reflection,
				cmd
			);
			lb::renderer::BindCommonResources(cmd);

			lb::renderer::Postprocess_Lineardepth(traceDepth, rtLinearDepth, cmd);

			if (scene->weather.IsRealisticSky())
			{
				lb::renderer::ComputeSkyAtmosphereSkyViewLut(cmd);

				if (scene->weather.IsRealisticSkyAerialPerspective())
				{
					lb::renderer::ComputeSkyAtmosphereCameraVolumeLut(cmd);
				}
			}
			if (scene->weather.IsRealisticSky() && scene->weather.IsRealisticSkyAerialPerspective())
			{
				lb::renderer::Postprocess_AerialPerspective(
					aerialperspectiveResources,
					cmd
				);
			}
			if (scene->weather.IsVolumetricClouds())
			{
				lb::renderer::Postprocess_VolumetricClouds(
					volumetriccloudResources,
					cmd,
					*camera,
					camera_previous,
					camera_reflection,
					false,
					scene->weather.volumetricCloudsWeatherMapFirst.IsValid() ? &scene->weather.volumetricCloudsWeatherMapFirst.GetTexture() : nullptr,
					scene->weather.volumetricCloudsWeatherMapSecond.IsValid() ? &scene->weather.volumetricCloudsWeatherMapSecond.GetTexture() : nullptr
				);
			}

			RenderVolumetrics(cmd);

			RenderLightShafts(cmd);

			// Composite other effects on top:
			{
				RenderPassImage rp[] = {
					RenderPassImage::DepthStencil(&depthBuffer_Main, RenderPassImage::LoadOp::LOAD),
					RenderPassImage::RenderTarget(&rtMain, RenderPassImage::LoadOp::CLEAR)
				};
				device->RenderPassBegin(rp, arraysize(rp), cmd);

				Viewport vp;
				vp.width = (float)rtMain.GetDesc().width;
				vp.height = (float)rtMain.GetDesc().height;
				device->BindViewports(1, &vp, cmd);

				// Clear to trace result:
				{
					device->EventBegin("Clear to trace result", cmd);
					lb::image::Params fx;
					fx.enableFullScreen();
					fx.blendFlag = lb::enums::BLENDMODE_OPAQUE;
					if (denoiserResult.IsValid() && !lb::jobsystem::IsBusy(denoiserContext))
					{
						lb::image::Draw(&denoiserResult, fx, cmd);
					}
					else
					{
						lb::image::Draw(&traceResult, fx, cmd);
					}
					device->EventEnd(cmd);
				}

				// Blend Aerial Perspective on top:
				if (scene->weather.IsRealisticSky() && scene->weather.IsRealisticSkyAerialPerspective())
				{
					device->EventBegin("Aerial Perspective Blend", cmd);
					lb::image::Params fx;
					fx.enableFullScreen();
					fx.blendFlag = lb::enums::BLENDMODE_PREMULTIPLIED;
					lb::image::Draw(&aerialperspectiveResources.texture_output, fx, cmd);
					device->EventEnd(cmd);
				}

				// Blend the volumetric clouds on top:
				if (scene->weather.IsVolumetricClouds())
				{
					lb::renderer::Postprocess_VolumetricClouds_Upsample(volumetriccloudResources, cmd);
				}

				lb::renderer::DrawDebugWorld(*scene, *camera, *this, cmd);
				lb::renderer::DrawLightVisualizers(visibility_main, cmd);
				lb::renderer::DrawSpritesAndFonts(*scene, *camera, false, cmd);

				if (getVolumeLightsEnabled() && visibility_main.IsRequestedVolumetricLights())
				{
					device->EventBegin("Contribute Volumetric Lights", cmd);
					lb::renderer::Postprocess_Upsample_Bilateral(
						rtVolumetricLights,
						rtLinearDepth,
						rtMain,
						cmd,
						true,
						1.5f
					);
					device->EventEnd(cmd);
				}

				XMVECTOR sunDirection = XMLoadFloat3(&scene->weather.sunDirection);
				if (getLightShaftsEnabled() && XMVectorGetX(XMVector3Dot(sunDirection, camera->GetAt())) > 0)
				{
					device->EventBegin("Contribute LightShafts", cmd);
					lb::image::Params fx;
					fx.enableFullScreen();
					fx.blendFlag = lb::enums::BLENDMODE_ADDITIVE;
					lb::image::Draw(&rtSun[1], fx, cmd);
					device->EventEnd(cmd);
				}
				if (getLensFlareEnabled())
				{
					lb::renderer::DrawLensFlares(
						visibility_main,
						cmd,
						scene->weather.IsVolumetricClouds() ? &volumetriccloudResources.texture_cloudMask : nullptr
					);
				}

				device->RenderPassEnd(cmd);
			}

			if (getEyeAdaptionEnabled())
			{
				lb::renderer::ComputeLuminance(
					luminanceResources,
					rtMain,
					cmd,
					getEyeAdaptionRate(),
					getEyeAdaptionKey()
				);
			}
			if (getBloomEnabled())
			{
				lb::renderer::ComputeBloom(
					bloomResources,
					rtMain,
					cmd,
					getBloomThreshold(),
					getExposure(),
					getEyeAdaptionEnabled() ? &luminanceResources.luminance : nullptr
				);
			}

			lb::renderer::Postprocess_Tonemap(
				rtMain,
				rtPostprocess,
				cmd,
				getExposure(),
				getBrightness(),
				getContrast(),
				getSaturation(),
				getDitherEnabled(),
				getColorGradingEnabled() ? (scene->weather.colorGradingMap.IsValid() ? &scene->weather.colorGradingMap.GetTexture() : nullptr) : nullptr,
				nullptr,
				getEyeAdaptionEnabled() ? &luminanceResources.luminance : nullptr,
				getBloomEnabled() ? &bloomResources.texture_bloom : nullptr,
				colorspace,
				getTonemap(),
				&distortion_overlay,
				getHDRCalibration()
			);
			lastPostprocessRT = &rtPostprocess;

			// GUI Background blurring:
			{
				auto range = lb::profiler::BeginRangeGPU("GUI Background Blur", cmd);
				device->EventBegin("GUI Background Blur", cmd);
				lb::renderer::Postprocess_Downsample4x(rtPostprocess, rtGUIBlurredBackground[0], cmd);
				lb::renderer::Postprocess_Downsample4x(rtGUIBlurredBackground[0], rtGUIBlurredBackground[2], cmd);
				lb::renderer::Postprocess_Blur_Gaussian(rtGUIBlurredBackground[2], rtGUIBlurredBackground[1], rtGUIBlurredBackground[2], cmd, -1, -1, true);
				device->EventEnd(cmd);
				lb::profiler::EndRange(range);
			}
			});

		RenderPath2D::Render();

		lb::jobsystem::Wait(ctx);
	}

	void RenderPath3D_PathTracing::Compose(CommandList cmd) const
	{
		GraphicsDevice* device = lb::graphics::GetDevice();

		device->EventBegin("RenderPath3D_PathTracing::Compose", cmd);

		lb::renderer::BindCommonResources(cmd);

		lb::image::Params fx;
		fx.enableFullScreen();
		fx.blendFlag = lb::enums::BLENDMODE_OPAQUE;
		fx.quality = lb::image::QUALITY_LINEAR;
		lb::image::Draw(&rtPostprocess, fx, cmd);

		device->EventEnd(cmd);

		RenderPath2D::Compose(cmd);
	}

}
