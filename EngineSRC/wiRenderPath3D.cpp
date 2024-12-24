#include "wiRenderPath3D.h"
#include "wiRenderer.h"
#include "wiImage.h"
#include "wiHelper.h"
#include "wiTextureHelper.h"
#include "wiProfiler.h"

using namespace lb::graphics;
using namespace lb::enums;

namespace lb
{
	static constexpr float foreground_depth_range = 0.01f;

	void RenderPath3D::DeleteGPUResources()
	{
		RenderPath2D::DeleteGPUResources();

		rtMain = {};
		rtMain_render = {};
		rtPrimitiveID = {};
		rtPrimitiveID_render = {};
		rtVelocity = {};
		rtReflection = {};
		rtSSR = {};
		rtSceneCopy = {};
		rtSceneCopy_tmp = {};
		rtWaterRipple = {};
		rtParticleDistortion_render = {};
		rtParticleDistortion = {};
		rtVolumetricLights = {};
		rtBloom = {};
		rtBloom_tmp = {};
		rtAO = {};
		rtShadow = {};
		rtSun[0] = {};
		rtSun[1] = {};
		rtSun_resolved = {};
		rtGUIBlurredBackground[0] = {};
		rtGUIBlurredBackground[1] = {};
		rtGUIBlurredBackground[2] = {};
		rtShadingRate = {};
		rtOutlineSource = {};

		rtPostprocess = {};

		depthBuffer_Main = {};
		depthBuffer_Copy = {};
		depthBuffer_Copy1 = {};
		depthBuffer_Reflection = {};
		rtLinearDepth = {};
		reprojectedDepth = {};

		debugUAV = {};
		tiledLightResources = {};
		tiledLightResources_planarReflection = {};
		luminanceResources = {};
		ssaoResources = {};
		rtaoResources = {};
		rtreflectionResources = {};
		ssrResources = {};
		rtshadowResources = {};
		screenspaceshadowResources = {};
		depthoffieldResources = {};
		motionblurResources = {};
		aerialperspectiveResources = {};
		aerialperspectiveResources_reflection = {};
		volumetriccloudResources = {};
		volumetriccloudResources_reflection = {};
		bloomResources = {};
		temporalAAResources = {};
		visibilityResources = {};
		vxgiResources = {};
	}

	void RenderPath3D::ResizeBuffers()
	{
		first_frame = true;
		DeleteGPUResources();

		GraphicsDevice* device = lb::graphics::GetDevice();

		XMUINT2 internalResolution = GetInternalResolution();
		camera->width = (float)internalResolution.x;
		camera->height = (float)internalResolution.y;

		// Render targets:

		{
			TextureDesc desc;
			desc.format = lb::renderer::format_rendertarget_main;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.sample_count = 1;
			device->CreateTexture(&desc, nullptr, &rtMain);
			device->SetName(&rtMain, "rtMain");

			if (getMSAASampleCount() > 1)
			{
				desc.sample_count = getMSAASampleCount();
				desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;

				device->CreateTexture(&desc, nullptr, &rtMain_render);
				device->SetName(&rtMain_render, "rtMain_render");
			}
			else
			{
				rtMain_render = rtMain;
			}
		}
		{
			TextureDesc desc;
			desc.format = lb::renderer::format_idbuffer;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
			if (getMSAASampleCount() > 1)
			{
				desc.bind_flags |= BindFlag::UNORDERED_ACCESS;
			}
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.sample_count = 1;
			desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;
			desc.misc_flags = ResourceMiscFlag::ALIASING_TEXTURE_RT_DS;
			device->CreateTexture(&desc, nullptr, &rtPrimitiveID);
			device->SetName(&rtPrimitiveID, "rtPrimitiveID");

			if (getMSAASampleCount() > 1)
			{
				desc.sample_count = getMSAASampleCount();
				desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
				desc.misc_flags = ResourceMiscFlag::NONE;
				device->CreateTexture(&desc, nullptr, &rtPrimitiveID_render);
				device->SetName(&rtPrimitiveID_render, "rtPrimitiveID_render");
			}
			else
			{
				rtPrimitiveID_render = rtPrimitiveID;
			}
		}
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
			desc.format = Format::R16G16_FLOAT;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.sample_count = 1;
			desc.misc_flags = ResourceMiscFlag::ALIASING_TEXTURE_RT_DS;
			device->CreateTexture(&desc, nullptr, &rtParticleDistortion);
			device->SetName(&rtParticleDistortion, "rtParticleDistortion");
			if (getMSAASampleCount() > 1)
			{
				desc.sample_count = getMSAASampleCount();
				desc.misc_flags = ResourceMiscFlag::NONE;
				device->CreateTexture(&desc, nullptr, &rtParticleDistortion_render);
				device->SetName(&rtParticleDistortion_render, "rtParticleDistortion_render");
			}
			else
			{
				rtParticleDistortion_render = rtParticleDistortion;
			}
		}
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = lb::renderer::format_rendertarget_main;
			desc.width = internalResolution.x / 4;
			desc.height = internalResolution.y / 4;
			desc.mip_levels = std::min(8u, (uint32_t)std::log2(std::max(desc.width, desc.height)));
			device->CreateTexture(&desc, nullptr, &rtSceneCopy);
			device->SetName(&rtSceneCopy, "rtSceneCopy");
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS | BindFlag::RENDER_TARGET; // render target for aliasing
			device->CreateTexture(&desc, nullptr, &rtSceneCopy_tmp, &rtPrimitiveID);
			device->SetName(&rtSceneCopy_tmp, "rtSceneCopy_tmp");

			for (uint32_t i = 0; i < rtSceneCopy.GetDesc().mip_levels; ++i)
			{
				int subresource_index;
				subresource_index = device->CreateSubresource(&rtSceneCopy, SubresourceType::SRV, 0, 1, i, 1);
				assert(subresource_index == i);
				subresource_index = device->CreateSubresource(&rtSceneCopy_tmp, SubresourceType::SRV, 0, 1, i, 1);
				assert(subresource_index == i);
				subresource_index = device->CreateSubresource(&rtSceneCopy, SubresourceType::UAV, 0, 1, i, 1);
				assert(subresource_index == i);
				subresource_index = device->CreateSubresource(&rtSceneCopy_tmp, SubresourceType::UAV, 0, 1, i, 1);
				assert(subresource_index == i);
			}

			// because this is used by SSR and SSGI before it gets a chance to be normally rendered, it MUST be cleared!
			CommandList cmd = device->BeginCommandList();
			device->Barrier(GPUBarrier::Image(&rtSceneCopy, rtSceneCopy.desc.layout, ResourceState::UNORDERED_ACCESS), cmd);
			device->ClearUAV(&rtSceneCopy, 0, cmd);
			device->Barrier(GPUBarrier::Image(&rtSceneCopy, ResourceState::UNORDERED_ACCESS, rtSceneCopy.desc.layout), cmd);
			device->SubmitCommandLists();
		}
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = lb::renderer::format_rendertarget_main;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			assert(ComputeTextureMemorySizeInBytes(desc) <= ComputeTextureMemorySizeInBytes(rtPrimitiveID.desc)); // Aliased check
			device->CreateTexture(&desc, nullptr, &rtPostprocess, &rtPrimitiveID); // Aliased!
			device->SetName(&rtPostprocess, "rtPostprocess");
		}
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = Format::R10G10B10A2_UNORM;
			desc.width = internalResolution.x / 4;
			desc.height = internalResolution.y / 4;
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
		if (device->CheckCapability(GraphicsDeviceCapability::VARIABLE_RATE_SHADING_TIER2) &&
			lb::renderer::GetVariableRateShadingClassification())
		{
			uint32_t tileSize = device->GetVariableRateShadingTileSize();

			TextureDesc desc;
			desc.layout = ResourceState::UNORDERED_ACCESS;
			desc.bind_flags = BindFlag::UNORDERED_ACCESS | BindFlag::SHADING_RATE;
			desc.format = Format::R8_UINT;
			desc.width = (internalResolution.x + tileSize - 1) / tileSize;
			desc.height = (internalResolution.y + tileSize - 1) / tileSize;

			device->CreateTexture(&desc, nullptr, &rtShadingRate);
			device->SetName(&rtShadingRate, "rtShadingRate");
		}

		// Depth buffers:
		{
			TextureDesc desc;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;

			desc.sample_count = getMSAASampleCount();
			desc.layout = ResourceState::DEPTHSTENCIL;
			desc.format = lb::renderer::format_depthbuffer_main;
			desc.bind_flags = BindFlag::DEPTH_STENCIL;
			device->CreateTexture(&desc, nullptr, &depthBuffer_Main);
			device->SetName(&depthBuffer_Main, "depthBuffer_Main");

			desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;
			desc.format = Format::R32_FLOAT;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.sample_count = 1;
			desc.mip_levels = 5;
			device->CreateTexture(&desc, nullptr, &depthBuffer_Copy);
			device->SetName(&depthBuffer_Copy, "depthBuffer_Copy");
			device->CreateTexture(&desc, nullptr, &depthBuffer_Copy1);
			device->SetName(&depthBuffer_Copy1, "depthBuffer_Copy1");

			for (uint32_t i = 0; i < depthBuffer_Copy.desc.mip_levels; ++i)
			{
				int subresource = 0;
				subresource = device->CreateSubresource(&depthBuffer_Copy, SubresourceType::SRV, 0, 1, i, 1);
				assert(subresource == i);
				subresource = device->CreateSubresource(&depthBuffer_Copy, SubresourceType::UAV, 0, 1, i, 1);
				assert(subresource == i);
				subresource = device->CreateSubresource(&depthBuffer_Copy1, SubresourceType::SRV, 0, 1, i, 1);
				assert(subresource == i);
				subresource = device->CreateSubresource(&depthBuffer_Copy1, SubresourceType::UAV, 0, 1, i, 1);
				assert(subresource == i);
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

		// Other resources:
		{
			TextureDesc desc;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.mip_levels = 1;
			desc.array_size = 1;
			desc.format = Format::R8G8B8A8_UNORM;
			desc.sample_count = 1;
			desc.usage = Usage::DEFAULT;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.layout = ResourceState::SHADER_RESOURCE;

			device->CreateTexture(&desc, nullptr, &debugUAV);
			device->SetName(&debugUAV, "debugUAV");
		}
		lb::renderer::CreateTiledLightResources(tiledLightResources, internalResolution);
		lb::renderer::CreateScreenSpaceShadowResources(screenspaceshadowResources, internalResolution);

		// These can trigger resource creations if needed:
		setAO(ao);
		setSSREnabled(ssrEnabled);
		setRaytracedReflectionsEnabled(raytracedReflectionsEnabled);
		setEyeAdaptionEnabled(eyeAdaptionEnabled);
		setReflectionsEnabled(reflectionsEnabled);
		setBloomEnabled(bloomEnabled);
		setVolumeLightsEnabled(volumeLightsEnabled);
		setLightShaftsEnabled(lightShaftsEnabled);
		setOutlineEnabled(outlineEnabled);

		RenderPath2D::ResizeBuffers();
	}

	void RenderPath3D::PreUpdate()
	{
		camera_previous = *camera;
		camera_reflection_previous = camera_reflection;
	}

	void RenderPath3D::Update(float dt)
	{
		GraphicsDevice* device = lb::graphics::GetDevice();

		if (rtMain_render.desc.sample_count != msaaSampleCount)
		{
			ResizeBuffers();
		}

		RenderPath2D::Update(dt);

		lb::renderer::SetShadowsEnabled(getShadowsEnabled());

		float update_speed = 0;

		const bool hw_raytrace = device->CheckCapability(GraphicsDeviceCapability::RAYTRACING);
		if (getSceneUpdateEnabled())
		{
			if (lb::renderer::GetDDGIEnabled() ||
				(hw_raytrace && lb::renderer::GetRaytracedShadowsEnabled()) ||
				(hw_raytrace && getAO() == AO_RTAO) ||
				(hw_raytrace && getRaytracedReflectionEnabled())
				)
			{
				scene->SetAccelerationStructureUpdateRequested(true);
			}
			scene->camera = *camera;
			update_speed = dt * lb::renderer::GetGameSpeed();
		}

		scene->Update(update_speed);

		// Frustum culling for main camera:
		visibility_main.layerMask = getLayerMask();
		visibility_main.scene = scene;
		visibility_main.camera = camera;
		visibility_main.flags = lb::renderer::Visibility::ALLOW_EVERYTHING;
		if (!getOcclusionCullingEnabled())
		{
			visibility_main.flags &= ~lb::renderer::Visibility::ALLOW_OCCLUSION_CULLING;
		}
		lb::renderer::UpdateVisibility(visibility_main);

		if (visibility_main.planar_reflection_visible)
		{
			// Frustum culling for planar reflections:
			camera_reflection = *camera;
			camera_reflection.jitter = XMFLOAT2(0, 0);
			camera_reflection.Reflect(visibility_main.reflectionPlane);
			visibility_reflection.layerMask = getLayerMask();
			visibility_reflection.scene = scene;
			visibility_reflection.camera = &camera_reflection;
			visibility_reflection.flags =
				lb::renderer::Visibility::ALLOW_OBJECTS |
				lb::renderer::Visibility::ALLOW_EMITTERS |
				lb::renderer::Visibility::ALLOW_HAIRS |
				lb::renderer::Visibility::ALLOW_LIGHTS
				;
			lb::renderer::UpdateVisibility(visibility_reflection);
		}

		XMUINT2 internalResolution = GetInternalResolution();

		lb::renderer::UpdatePerFrameData(
			*scene,
			visibility_main,
			frameCB,
			getSceneUpdateEnabled() ? dt : 0
		);

		 if (lb::renderer::GetTemporalAAEnabled())
		{
			const XMFLOAT4& halton = lb::math::GetHaltonSequence(lb::graphics::GetDevice()->GetFrameCount() % 256);
			camera->jitter.x = (halton.x * 2 - 1) / (float)internalResolution.x;
			camera->jitter.y = (halton.y * 2 - 1) / (float)internalResolution.y;
			if (!temporalAAResources.IsValid())
			{
				lb::renderer::CreateTemporalAAResources(temporalAAResources, internalResolution);
			}
		}
		else
		{
			camera->jitter = XMFLOAT2(0, 0);
			temporalAAResources = {};
		}

		camera_reflection.jitter = XMFLOAT2(0, 0);

		camera->UpdateCamera();
		if (visibility_main.planar_reflection_visible)
		{
			camera_reflection.UpdateCamera();
		}

		if (getAO() != AO_RTAO)
		{
			rtaoResources.frame = 0;
		}
		if (!lb::renderer::GetRaytracedShadowsEnabled())
		{
			rtshadowResources.frame = 0;
		}
		if (!getSSREnabled() && !getRaytracedReflectionEnabled())
		{
			rtSSR = {};
		}
		if (getAO() == AO_DISABLED)
		{
			rtAO = {};
		}

		if (lb::renderer::GetRaytracedShadowsEnabled() && device->CheckCapability(GraphicsDeviceCapability::RAYTRACING))
		{
			if (!rtshadowResources.denoised.IsValid())
			{
				lb::renderer::CreateRTShadowResources(rtshadowResources, internalResolution);
			}
		}
		else
		{
			rtshadowResources = {};
		}

		if (scene->weather.IsRealisticSky() && scene->weather.IsRealisticSkyAerialPerspective())
		{
			if (!aerialperspectiveResources.texture_output.IsValid())
			{
				lb::renderer::CreateAerialPerspectiveResources(aerialperspectiveResources, internalResolution);
			}
			if (getReflectionsEnabled() && depthBuffer_Reflection.IsValid())
			{
				if (!aerialperspectiveResources_reflection.texture_output.IsValid())
				{
					lb::renderer::CreateAerialPerspectiveResources(aerialperspectiveResources_reflection, XMUINT2(depthBuffer_Reflection.desc.width, depthBuffer_Reflection.desc.height));
				}
			}
			else
			{
				aerialperspectiveResources_reflection = {};
			}
		}
		else
		{
			aerialperspectiveResources = {};
		}

		if (scene->weather.IsVolumetricClouds())
		{
			if (!volumetriccloudResources.texture_cloudRender.IsValid())
			{
				lb::renderer::CreateVolumetricCloudResources(volumetriccloudResources, internalResolution);
			}
			if (getReflectionsEnabled() && depthBuffer_Reflection.IsValid())
			{
				if (!volumetriccloudResources_reflection.texture_cloudRender.IsValid())
				{
					lb::renderer::CreateVolumetricCloudResources(volumetriccloudResources_reflection, XMUINT2(depthBuffer_Reflection.desc.width, depthBuffer_Reflection.desc.height));
				}
			}
			else
			{
				volumetriccloudResources_reflection = {};
			}
		}
		else
		{
			volumetriccloudResources = {};
		}

		if (!scene->waterRipples.empty())
		{
			if (!rtWaterRipple.IsValid())
			{
				TextureDesc desc;
				desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
				desc.format = Format::R16G16_FLOAT;
				desc.width = internalResolution.x / 8;
				desc.height = internalResolution.y / 8;
				assert(ComputeTextureMemorySizeInBytes(desc) <= ComputeTextureMemorySizeInBytes(rtParticleDistortion.desc)); // aliasing check
				device->CreateTexture(&desc, nullptr, &rtWaterRipple, &rtParticleDistortion); // aliased!
				device->SetName(&rtWaterRipple, "rtWaterRipple");
			}
		}
		else
		{
			rtWaterRipple = {};
		}

		if (lb::renderer::GetVXGIEnabled())
		{
			if (!vxgiResources.IsValid())
			{
				lb::renderer::CreateVXGIResources(vxgiResources, internalResolution);
			}
		}
		else
		{
			vxgiResources = {};
		}

		// Check whether reprojected depth is required:
		if (!first_frame && lb::renderer::IsMeshShaderAllowed() && lb::renderer::IsMeshletOcclusionCullingEnabled())
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = Format::R16_UNORM;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.mip_levels = GetMipCount(desc.width, desc.height, 1, 4);
			desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;
			device->CreateTexture(&desc, nullptr, &reprojectedDepth);
			device->SetName(&reprojectedDepth, "reprojectedDepth");

			for (uint32_t i = 0; i < reprojectedDepth.desc.mip_levels; ++i)
			{
				int subresource_index;
				subresource_index = device->CreateSubresource(&reprojectedDepth, SubresourceType::SRV, 0, 1, i, 1);
				assert(subresource_index == i);
				subresource_index = device->CreateSubresource(&reprojectedDepth, SubresourceType::UAV, 0, 1, i, 1);
				assert(subresource_index == i);
			}
		}
		else
		{
			reprojectedDepth = {};
		}

		// Check whether velocity buffer is required:
		if (
			getMotionBlurEnabled() ||
			lb::renderer::GetTemporalAAEnabled() ||
			getSSREnabled() ||
			getRaytracedReflectionEnabled() ||
			lb::renderer::GetRaytracedShadowsEnabled() ||
			getAO() == AO::AO_RTAO ||
			lb::renderer::GetVariableRateShadingClassification() ||
			reprojectedDepth.IsValid()
			)
		{
			if (!rtVelocity.IsValid())
			{
				TextureDesc desc;
				desc.format = Format::R16G16_FLOAT;
				desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS | BindFlag::RENDER_TARGET;
				desc.width = internalResolution.x;
				desc.height = internalResolution.y;
				desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;
				device->CreateTexture(&desc, nullptr, &rtVelocity);
				device->SetName(&rtVelocity, "rtVelocity");
			}
		}
		else
		{
			rtVelocity = {};
		}

		// Check whether shadow mask is required:
		if (lb::renderer::GetScreenSpaceShadowsEnabled() || lb::renderer::GetRaytracedShadowsEnabled())
		{
			if (!rtShadow.IsValid())
			{
				TextureDesc desc;
				desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
				desc.format = Format::R8_UNORM;
				desc.array_size = 16;
				desc.width = internalResolution.x;
				desc.height = internalResolution.y;
				desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;
				device->CreateTexture(&desc, nullptr, &rtShadow);
				device->SetName(&rtShadow, "rtShadow");
			}
		}
		else
		{
			rtShadow = {};
		}

		// Check whether visibility resources are required:
		if (
			visibility_shading_in_compute ||
			getSSREnabled() ||
			getRaytracedReflectionEnabled() ||
			lb::renderer::GetScreenSpaceShadowsEnabled() ||
			lb::renderer::GetRaytracedShadowsEnabled() ||
			lb::renderer::GetVXGIEnabled()
			)
		{
			if (!visibilityResources.IsValid())
			{
				lb::renderer::CreateVisibilityResources(visibilityResources, internalResolution);
			}
		}
		else
		{
			visibilityResources = {};
		}

		// Check for depth of field allocation:
		if (getDepthOfFieldEnabled() &&
			getDepthOfFieldStrength() > 0 &&
			camera->aperture_size > 0
			)
		{
			if (!depthoffieldResources.IsValid())
			{
				XMUINT2 resolution = GetInternalResolution();
				lb::renderer::CreateDepthOfFieldResources(depthoffieldResources, resolution);
			}
		}
		else
		{
			depthoffieldResources = {};
		}

		// Check for motion blur allocation:
		if (getMotionBlurEnabled() && getMotionBlurStrength() > 0)
		{
			if (!motionblurResources.IsValid())
			{
				XMUINT2 resolution = GetInternalResolution();
				lb::renderer::CreateMotionBlurResources(motionblurResources, resolution);
			}
		}
		else
		{
			motionblurResources = {};
		}

		// Keep a copy of last frame's depth buffer for temporal disocclusion checks, so swap with current one every frame:
		std::swap(depthBuffer_Copy, depthBuffer_Copy1);

		visibilityResources.depthbuffer = &depthBuffer_Copy;
		visibilityResources.lineardepth = &rtLinearDepth;
		if (getMSAASampleCount() > 1)
		{
			visibilityResources.primitiveID_resolved = &rtPrimitiveID;
		}
		else
		{
			visibilityResources.primitiveID_resolved = nullptr;
		}

		camera->canvas.init(*this);
		camera->width = (float)internalResolution.x;
		camera->height = (float)internalResolution.y;
		camera->scissor = GetScissorInternalResolution();
		camera->sample_count = depthBuffer_Main.desc.sample_count;
		camera->shadercamera_options = SHADERCAMERA_OPTION_NONE;
		camera->texture_primitiveID_index = device->GetDescriptorIndex(&rtPrimitiveID, SubresourceType::SRV);
		camera->texture_depth_index = device->GetDescriptorIndex(&depthBuffer_Copy, SubresourceType::SRV);
		camera->texture_lineardepth_index = device->GetDescriptorIndex(&rtLinearDepth, SubresourceType::SRV);
		camera->texture_velocity_index = device->GetDescriptorIndex(&rtVelocity, SubresourceType::SRV);
		camera->texture_normal_index = device->GetDescriptorIndex(&visibilityResources.texture_normals, SubresourceType::SRV);
		camera->texture_roughness_index = device->GetDescriptorIndex(&visibilityResources.texture_roughness, SubresourceType::SRV);
		camera->buffer_entitytiles_index = device->GetDescriptorIndex(&tiledLightResources.entityTiles, SubresourceType::SRV);
		camera->texture_reflection_index = device->GetDescriptorIndex(&rtReflection_resolved, SubresourceType::SRV);
		camera->texture_reflection_depth_index = device->GetDescriptorIndex(&depthBuffer_Reflection_resolved, SubresourceType::SRV);
		camera->texture_refraction_index = device->GetDescriptorIndex(&rtSceneCopy, SubresourceType::SRV);
		camera->texture_waterriples_index = device->GetDescriptorIndex(&rtWaterRipple, SubresourceType::SRV);
		camera->texture_ao_index = device->GetDescriptorIndex(&rtAO, SubresourceType::SRV);
		camera->texture_ssr_index = device->GetDescriptorIndex(&rtSSR, SubresourceType::SRV);
		if (rtShadow.IsValid())
		{
			camera->shadercamera_options |= SHADERCAMERA_OPTION_USE_SHADOW_MASK;
			camera->texture_rtshadow_index = device->GetDescriptorIndex(&rtShadow, SubresourceType::SRV);
		}
		else
		{
			camera->texture_rtshadow_index = device->GetDescriptorIndex(lb::texturehelper::getWhite(), SubresourceType::SRV); // AMD descriptor branching fix
		}
		camera->texture_vxgi_diffuse_index = device->GetDescriptorIndex(&vxgiResources.diffuse, SubresourceType::SRV);
		if (lb::renderer::GetVXGIReflectionsEnabled())
		{
			camera->texture_vxgi_specular_index = device->GetDescriptorIndex(&vxgiResources.specular, SubresourceType::SRV);
		}
		else
		{
			camera->texture_vxgi_specular_index = -1;
		}
		camera->texture_reprojected_depth_index = device->GetDescriptorIndex(&reprojectedDepth, SubresourceType::SRV);

		camera_reflection.canvas.init(*this);
		camera_reflection.width = (float)depthBuffer_Reflection.desc.width;
		camera_reflection.height = (float)depthBuffer_Reflection.desc.height;
		camera_reflection.scissor.left = 0;
		camera_reflection.scissor.top = 0;
		camera_reflection.scissor.right = (int)depthBuffer_Reflection.desc.width;
		camera_reflection.scissor.bottom = (int)depthBuffer_Reflection.desc.height;
		camera_reflection.sample_count = depthBuffer_Reflection.desc.sample_count;
		camera_reflection.shadercamera_options = SHADERCAMERA_OPTION_NONE;
		camera_reflection.texture_primitiveID_index = -1;
		camera_reflection.texture_depth_index = device->GetDescriptorIndex(&depthBuffer_Reflection_resolved, SubresourceType::SRV);
		camera_reflection.texture_lineardepth_index = -1;
		camera_reflection.texture_velocity_index = -1;
		camera_reflection.texture_normal_index = -1;
		camera_reflection.texture_roughness_index = -1;
		camera_reflection.buffer_entitytiles_index = device->GetDescriptorIndex(&tiledLightResources_planarReflection.entityTiles, SubresourceType::SRV);
		camera_reflection.texture_reflection_index = -1;
		camera_reflection.texture_reflection_depth_index = -1;
		camera_reflection.texture_refraction_index = -1;
		camera_reflection.texture_waterriples_index = -1;
		camera_reflection.texture_ao_index = -1;
		camera_reflection.texture_ssr_index = -1;
		camera_reflection.texture_rtshadow_index = device->GetDescriptorIndex(lb::texturehelper::getWhite(), SubresourceType::SRV); // AMD descriptor branching fix
		camera_reflection.texture_vxgi_diffuse_index = -1;
		camera_reflection.texture_vxgi_specular_index = -1;
		camera_reflection.texture_reprojected_depth_index = -1;

		video_cmd = {};
		if (getSceneUpdateEnabled() && scene->videos.GetCount() > 0)
		{
			for (size_t i = 0; i < scene->videos.GetCount(); ++i)
			{
				const lb::scene::VideoComponent& video = scene->videos[i];
				if (lb::video::IsDecodingRequired(&video.videoinstance, dt))
				{
					video_cmd = device->BeginCommandList(QUEUE_VIDEO_DECODE);
					break;
				}
			}
			for (size_t i = 0; i < scene->videos.GetCount(); ++i)
			{
				lb::scene::VideoComponent& video = scene->videos[i];
				lb::video::UpdateVideo(&video.videoinstance, dt, video_cmd);
			}
		}
	}

	void RenderPath3D::Render() const
	{
		GraphicsDevice* device = lb::graphics::GetDevice();
		lb::jobsystem::context ctx;

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

		// Preparing the frame:
		CommandList cmd = device->BeginCommandList();
		device->WaitQueue(cmd, QUEUE_COMPUTE); // sync to prev frame compute (disallow prev frame overlapping a compute task into updating global scene resources for this frame)
		lb::renderer::ProcessDeferredTextureRequests(cmd); // Execute it first thing in the frame here, on main thread, to not allow other thread steal it and execute on different command list!
		CommandList cmd_prepareframe = cmd;
		lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {
			GraphicsDevice* device = lb::graphics::GetDevice();

			lb::renderer::BindCameraCB(
				*camera,
				camera_previous,
				camera_reflection,
				cmd
			);
			lb::renderer::UpdateRenderData(visibility_main, frameCB, cmd);

			uint32_t num_barriers = 2;
			GPUBarrier barriers[] = {
				GPUBarrier::Image(&debugUAV, debugUAV.desc.layout, ResourceState::UNORDERED_ACCESS),
				GPUBarrier::Aliasing(&rtPostprocess, &rtPrimitiveID),
				GPUBarrier::Image(&rtMain, rtMain.desc.layout, ResourceState::SHADER_RESOURCE_COMPUTE), // prepares transition for discard in dx12
			};
			if (visibility_shading_in_compute)
			{
				num_barriers++;
			}
			device->Barrier(barriers, num_barriers, cmd);

		});

		// async compute parallel with depth prepass
		cmd = device->BeginCommandList(QUEUE_COMPUTE);
		CommandList cmd_prepareframe_async = cmd;
		device->WaitCommandList(cmd, cmd_prepareframe);
		if (cmd_copypages.IsValid())
		{
			device->WaitCommandList(cmd, cmd_copypages);
		}
		lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

			lb::renderer::BindCameraCB(
				*camera,
				camera_previous,
				camera_reflection,
				cmd
			);
			lb::renderer::UpdateRenderDataAsync(visibility_main, frameCB, cmd);

			if (scene->IsAccelerationStructureUpdateRequested())
			{
				lb::renderer::UpdateRaytracingAccelerationStructures(*scene, cmd);
			}

			if (scene->weather.IsRealisticSky())
			{
				lb::renderer::ComputeSkyAtmosphereTextures(cmd);
				lb::renderer::ComputeSkyAtmosphereSkyViewLut(cmd);
			}

			if (lb::renderer::GetDDGIEnabled() && getSceneUpdateEnabled())
			{
				lb::renderer::DDGI(
					*scene,
					cmd
				);
			}

		});

		static const uint32_t drawscene_flags =
			lb::renderer::DRAWSCENE_OPAQUE |
			lb::renderer::DRAWSCENE_IMPOSTOR |
			lb::renderer::DRAWSCENE_HAIRPARTICLE |
			lb::renderer::DRAWSCENE_TESSELLATION |
			lb::renderer::DRAWSCENE_OCCLUSIONCULLING |
			lb::renderer::DRAWSCENE_MAINCAMERA
			;

		// Main camera depth prepass:
		cmd = device->BeginCommandList();
		CommandList cmd_maincamera_prepass = cmd;
		lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

			GraphicsDevice* device = lb::graphics::GetDevice();

			lb::renderer::BindCameraCB(
				*camera,
				camera_previous,
				camera_reflection,
				cmd
			);

			lb::renderer::RefreshImpostors(*scene, cmd);

			if (reprojectedDepth.IsValid())
			{
				lb::renderer::ComputeReprojectedDepthPyramid(
					depthBuffer_Copy,
					rtVelocity,
					reprojectedDepth,
					cmd
				);
			}

			RenderPassImage rp[] = {
				RenderPassImage::DepthStencil(
					&depthBuffer_Main,
					RenderPassImage::LoadOp::CLEAR,
					RenderPassImage::StoreOp::STORE,
					ResourceState::DEPTHSTENCIL,
					ResourceState::DEPTHSTENCIL,
					ResourceState::DEPTHSTENCIL
				),
				RenderPassImage::RenderTarget(
					&rtPrimitiveID_render,
					RenderPassImage::LoadOp::CLEAR,
					RenderPassImage::StoreOp::STORE,
					ResourceState::SHADER_RESOURCE_COMPUTE,
					ResourceState::SHADER_RESOURCE_COMPUTE
				),
			};
			device->RenderPassBegin(rp, arraysize(rp), cmd);

			device->EventBegin("Opaque Z-prepass", cmd);
			auto range = lb::profiler::BeginRangeGPU("Z-Prepass", cmd);

			Rect scissor = GetScissorInternalResolution();
			device->BindScissorRects(1, &scissor, cmd);

			Viewport vp;
			vp.width = (float)depthBuffer_Main.GetDesc().width;
			vp.height = (float)depthBuffer_Main.GetDesc().height;

			// Foreground:
			vp.min_depth = 1 - foreground_depth_range;
			vp.max_depth = 1;
			device->BindViewports(1, &vp, cmd);
			lb::renderer::DrawScene(
				visibility_main,
				RENDERPASS_PREPASS,
				cmd,
				lb::renderer::DRAWSCENE_OPAQUE |
				lb::renderer::DRAWSCENE_FOREGROUND_ONLY |
				lb::renderer::DRAWSCENE_MAINCAMERA
			);

			// Regular:
			vp.min_depth = 0;
			vp.max_depth = 1;
			device->BindViewports(1, &vp, cmd);
			lb::renderer::DrawScene(
				visibility_main,
				RENDERPASS_PREPASS,
				cmd,
				drawscene_flags
			);

			lb::profiler::EndRange(range);
			device->EventEnd(cmd);

			device->RenderPassEnd(cmd);

		});

		// Main camera compute effects:
		//	(async compute, parallel to "shadow maps" and "update textures",
		//	must finish before "main scene opaque color pass")
		cmd = device->BeginCommandList(QUEUE_COMPUTE);
		device->WaitCommandList(cmd, cmd_maincamera_prepass);
		if (video_cmd.IsValid())
		{
			device->WaitCommandList(cmd, video_cmd);
		}
		CommandList cmd_maincamera_compute_effects = cmd;
		lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

			GraphicsDevice* device = lb::graphics::GetDevice();

			for (size_t i = 0; i < scene->videos.GetCount(); ++i)
			{
				lb::scene::VideoComponent& video = scene->videos[i];
				lb::video::ResolveVideoToRGB(&video.videoinstance, cmd);
			}

			lb::renderer::BindCameraCB(
				*camera,
				camera_previous,
				camera_reflection,
				cmd
			);

			lb::renderer::Visibility_Prepare(
				visibilityResources,
				rtPrimitiveID_render,
				cmd
			);

			lb::renderer::ComputeTiledLightCulling(
				tiledLightResources,
				visibility_main,
				debugUAV,
				cmd
			);

			if (visibility_shading_in_compute)
			{
				lb::renderer::Visibility_Surface(
					visibilityResources,
					rtMain,
					cmd
				);
			}
			else if (
				getSSREnabled() ||
				getRaytracedReflectionEnabled() ||
				lb::renderer::GetScreenSpaceShadowsEnabled() ||
				lb::renderer::GetRaytracedShadowsEnabled() ||
				lb::renderer::GetVXGIEnabled()
				)
			{
				// These post effects require surface normals and/or roughness
				lb::renderer::Visibility_Surface_Reduced(
					visibilityResources,
					cmd
				);
			}

			if (rtVelocity.IsValid())
			{
				lb::renderer::Visibility_Velocity(
					rtVelocity,
					cmd
				);
			}

			RenderAO(cmd);

			if (lb::renderer::GetVariableRateShadingClassification() && device->CheckCapability(GraphicsDeviceCapability::VARIABLE_RATE_SHADING_TIER2))
			{
				lb::renderer::ComputeShadingRateClassification(
					rtShadingRate,
					debugUAV,
					cmd
				);
			}

			RenderSSR(cmd);

			if (lb::renderer::GetScreenSpaceShadowsEnabled())
			{
				lb::renderer::Postprocess_ScreenSpaceShadow(
					screenspaceshadowResources,
					tiledLightResources.entityTiles,
					rtLinearDepth,
					rtShadow,
					cmd,
					getScreenSpaceShadowRange(),
					getScreenSpaceShadowSampleCount()
				);
			}

			if (lb::renderer::GetRaytracedShadowsEnabled())
			{
				lb::renderer::Postprocess_RTShadow(
					rtshadowResources,
					*scene,
					tiledLightResources.entityTiles,
					rtLinearDepth,
					rtShadow,
					cmd
				);
			}

			if (scene->weather.IsVolumetricClouds() && !scene->weather.IsVolumetricCloudsReceiveShadow())
			{
				// When volumetric cloud DOESN'T receives shadow it can be done async to shadow maps!
				lb::renderer::Postprocess_VolumetricClouds(
					volumetriccloudResources,
					cmd,
					*camera,
					camera_previous,
					camera_reflection,
					lb::renderer::GetTemporalAAEnabled(),
					scene->weather.volumetricCloudsWeatherMapFirst.IsValid() ? &scene->weather.volumetricCloudsWeatherMapFirst.GetTexture() : nullptr,
					scene->weather.volumetricCloudsWeatherMapSecond.IsValid() ? &scene->weather.volumetricCloudsWeatherMapSecond.GetTexture() : nullptr
				);
			}

		});

		// Occlusion culling:
		CommandList cmd_occlusionculling;
		if (getOcclusionCullingEnabled())
		{
			cmd = device->BeginCommandList();
			cmd_occlusionculling = cmd;
			lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

				GraphicsDevice* device = lb::graphics::GetDevice();

				device->EventBegin("Occlusion Culling", cmd);
				ScopedGPUProfiling("Occlusion Culling", cmd);

				lb::renderer::BindCameraCB(
					*camera,
					camera_previous,
					camera_reflection,
					cmd
				);

				lb::renderer::OcclusionCulling_Reset(visibility_main, cmd); // must be outside renderpass!

				RenderPassImage rp[] = {
					RenderPassImage::DepthStencil(&depthBuffer_Main),
				};
				device->RenderPassBegin(rp, arraysize(rp), cmd);

				Rect scissor = GetScissorInternalResolution();
				device->BindScissorRects(1, &scissor, cmd);

				Viewport vp;
				vp.width = (float)depthBuffer_Main.GetDesc().width;
				vp.height = (float)depthBuffer_Main.GetDesc().height;
				device->BindViewports(1, &vp, cmd);

				lb::renderer::OcclusionCulling_Render(*camera, visibility_main, cmd);

				device->RenderPassEnd(cmd);

				lb::renderer::OcclusionCulling_Resolve(visibility_main, cmd); // must be outside renderpass!

				device->EventEnd(cmd);
			});
		}

		CommandList cmd_ocean;
		if (scene->weather.IsOceanEnabled() && scene->ocean.IsValid())
		{
			// Ocean simulation can be updated async to opaque passes:
			cmd_ocean = device->BeginCommandList(QUEUE_COMPUTE);
			if (cmd_occlusionculling.IsValid())
			{
				// Ocean occlusion culling must be waited
				device->WaitCommandList(cmd_ocean, cmd_occlusionculling);
			}
			lb::renderer::UpdateOcean(visibility_main, cmd_ocean);

			// Copying to readback is done on copy queue to use DMA instead of compute warps:
			CommandList cmd_oceancopy = device->BeginCommandList(QUEUE_COPY);
			device->WaitCommandList(cmd_oceancopy, cmd_ocean);
			lb::renderer::ReadbackOcean(visibility_main, cmd_oceancopy);
		}

		// Shadow maps:
		if (getShadowsEnabled())
		{
			cmd = device->BeginCommandList();
			lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {
				lb::renderer::DrawShadowmaps(visibility_main, cmd);
			});
		}

		if (lb::renderer::GetVXGIEnabled() && getSceneUpdateEnabled())
		{
			cmd = device->BeginCommandList();
			lb::jobsystem::Execute(ctx, [cmd, this](lb::jobsystem::JobArgs args) {
				lb::renderer::VXGI_Voxelize(visibility_main, cmd);
			});
		}

		// Updating textures:
		if (getSceneUpdateEnabled())
		{
			cmd = device->BeginCommandList();
			device->WaitCommandList(cmd, cmd_prepareframe_async);
			lb::jobsystem::Execute(ctx, [cmd, this](lb::jobsystem::JobArgs args) {
				lb::renderer::BindCommonResources(cmd);
				lb::renderer::BindCameraCB(
					*camera,
					camera_previous,
					camera_reflection,
					cmd
				);
				lb::renderer::RefreshLightmaps(*scene, cmd);
				lb::renderer::RefreshEnvProbes(visibility_main, cmd);
				lb::renderer::PaintDecals(*scene, cmd);
			});
		}

		if (getReflectionsEnabled() && visibility_main.IsRequestedPlanarReflections())
		{
			// Planar reflections depth prepass:
			cmd = device->BeginCommandList();
			lb::jobsystem::Execute(ctx, [cmd, this](lb::jobsystem::JobArgs args) {

				GraphicsDevice* device = lb::graphics::GetDevice();

				lb::renderer::BindCameraCB(
					camera_reflection,
					camera_reflection_previous,
					camera_reflection,
					cmd
				);

				// Render SkyAtmosphere assets from planar reflections point of view
				if (scene->weather.IsRealisticSky())
				{
					lb::renderer::ComputeSkyAtmosphereSkyViewLut(cmd);

					if (scene->weather.IsRealisticSkyAerialPerspective())
					{
						lb::renderer::ComputeSkyAtmosphereCameraVolumeLut(cmd);
					}
				}

				device->EventBegin("Planar reflections Z-Prepass", cmd);
				auto range = lb::profiler::BeginRangeGPU("Planar Reflections Z-Prepass", cmd);

				RenderPassImage rp[] = {
					RenderPassImage::DepthStencil(
						&depthBuffer_Reflection,
						RenderPassImage::LoadOp::CLEAR,
						RenderPassImage::StoreOp::STORE,
						ResourceState::SHADER_RESOURCE,
						ResourceState::DEPTHSTENCIL,
						ResourceState::SHADER_RESOURCE
					)
				};
				device->RenderPassBegin(rp, arraysize(rp), cmd);

				Viewport vp;
				vp.width = (float)depthBuffer_Reflection.GetDesc().width;
				vp.height = (float)depthBuffer_Reflection.GetDesc().height;
				vp.min_depth = 0;
				vp.max_depth = 1;
				device->BindViewports(1, &vp, cmd);

				lb::renderer::DrawScene(
					visibility_reflection,
					RENDERPASS_PREPASS_DEPTHONLY,
					cmd,
					lb::renderer::DRAWSCENE_OPAQUE |
					lb::renderer::DRAWSCENE_IMPOSTOR |
					lb::renderer::DRAWSCENE_HAIRPARTICLE |
					lb::renderer::DRAWSCENE_SKIP_PLANAR_REFLECTION_OBJECTS
				);

				device->RenderPassEnd(cmd);

				lb::renderer::ResolveMSAADepthBuffer(depthBuffer_Reflection_resolved, depthBuffer_Reflection, cmd);

				if (scene->weather.IsRealisticSky() && scene->weather.IsRealisticSkyAerialPerspective())
				{
					lb::renderer::Postprocess_AerialPerspective(
						aerialperspectiveResources_reflection,
						cmd
					);
				}

				lb::profiler::EndRange(range); // Planar Reflections
				device->EventEnd(cmd);

			});

			// Planar reflections color pass:
			cmd = device->BeginCommandList();
			lb::jobsystem::Execute(ctx, [cmd, this](lb::jobsystem::JobArgs args) {

				GraphicsDevice* device = lb::graphics::GetDevice();

				lb::renderer::BindCameraCB(
					camera_reflection,
					camera_reflection_previous,
					camera_reflection,
					cmd
				);

				lb::renderer::ComputeTiledLightCulling(
					tiledLightResources_planarReflection,
					visibility_reflection,
					Texture(),
					cmd
				);

				if (scene->weather.IsVolumetricClouds())
				{
					lb::renderer::Postprocess_VolumetricClouds(
						volumetriccloudResources_reflection,
						cmd,
						camera_reflection,
						camera_reflection_previous,
						camera_reflection,
						lb::renderer::GetTemporalAAEnabled(),
						scene->weather.volumetricCloudsWeatherMapFirst.IsValid() ? &scene->weather.volumetricCloudsWeatherMapFirst.GetTexture() : nullptr,
						scene->weather.volumetricCloudsWeatherMapSecond.IsValid() ? &scene->weather.volumetricCloudsWeatherMapSecond.GetTexture() : nullptr
					);
				}

				device->EventBegin("Planar reflections", cmd);
				auto range = lb::profiler::BeginRangeGPU("Planar Reflections", cmd);

				RenderPassImage rp[] = {
					RenderPassImage::RenderTarget(
						&rtReflection,
						RenderPassImage::LoadOp::CLEAR,
						RenderPassImage::StoreOp::DONTCARE,
						ResourceState::RENDERTARGET,
						ResourceState::RENDERTARGET
					),
					RenderPassImage::DepthStencil(
						&depthBuffer_Reflection,
						RenderPassImage::LoadOp::LOAD,
						RenderPassImage::StoreOp::STORE,
						ResourceState::SHADER_RESOURCE,
						ResourceState::DEPTHSTENCIL,
						ResourceState::SHADER_RESOURCE
					),
					RenderPassImage::Resolve(&rtReflection_resolved)
				};
				device->RenderPassBegin(rp, arraysize(rp), cmd);

				Viewport vp;
				vp.width = (float)depthBuffer_Reflection.GetDesc().width;
				vp.height = (float)depthBuffer_Reflection.GetDesc().height;
				vp.min_depth = 0;
				vp.max_depth = 1;
				device->BindViewports(1, &vp, cmd);

				lb::renderer::DrawScene(
					visibility_reflection,
					RENDERPASS_MAIN,
					cmd,
					lb::renderer::DRAWSCENE_OPAQUE |
					lb::renderer::DRAWSCENE_IMPOSTOR |
					lb::renderer::DRAWSCENE_HAIRPARTICLE |
					lb::renderer::DRAWSCENE_SKIP_PLANAR_REFLECTION_OBJECTS
				);
				lb::renderer::DrawScene(
					visibility_reflection,
					RENDERPASS_MAIN,
					cmd,
					lb::renderer::DRAWSCENE_TRANSPARENT |
					lb::renderer::DRAWSCENE_SKIP_PLANAR_REFLECTION_OBJECTS
				); // separate renderscene, to be drawn after opaque and transparent sort order
				lb::renderer::DrawSky(*scene, cmd);

				if (scene->weather.IsRealisticSky() && scene->weather.IsRealisticSkyAerialPerspective())
				{
					// Blend Aerial Perspective on top:
					device->EventBegin("Aerial Perspective Reflection Blend", cmd);
					lb::image::Params fx;
					fx.enableFullScreen();
					fx.blendFlag = BLENDMODE_PREMULTIPLIED;
					lb::image::Draw(&aerialperspectiveResources_reflection.texture_output, fx, cmd);
					device->EventEnd(cmd);
				}

				// Blend the volumetric clouds on top:
				//	For planar reflections, we don't use upsample, because there is no linear depth here
				if (scene->weather.IsVolumetricClouds())
				{
					device->EventBegin("Volumetric Clouds Reflection Blend", cmd);
					lb::image::Params fx;
					fx.enableFullScreen();
					fx.blendFlag = BLENDMODE_PREMULTIPLIED;
					lb::image::Draw(&volumetriccloudResources_reflection.texture_reproject[volumetriccloudResources_reflection.frame % 2], fx, cmd);
					device->EventEnd(cmd);
				}

				lb::renderer::DrawSoftParticles(visibility_reflection, false, cmd);
				lb::renderer::DrawSpritesAndFonts(*scene, camera_reflection, false, cmd);

				device->RenderPassEnd(cmd);

				lb::profiler::EndRange(range); // Planar Reflections
				device->EventEnd(cmd);
			});
		}

		// Main camera weather compute effects depending on shadow maps, envmaps, etc, but don't depend on async surface pass:
		if (scene->weather.IsRealisticSky() || scene->weather.IsVolumetricClouds())
		{
			cmd = device->BeginCommandList();
			lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

				lb::renderer::BindCameraCB(
					*camera,
					camera_previous,
					camera_reflection,
					cmd
				);

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
				if (scene->weather.IsVolumetricClouds() && scene->weather.IsVolumetricCloudsReceiveShadow())
				{
					// When volumetric cloud receives shadow it must be done AFTER shadow maps!
					lb::renderer::Postprocess_VolumetricClouds(
						volumetriccloudResources,
						cmd,
						*camera,
						camera_previous,
						camera_reflection,
						lb::renderer::GetTemporalAAEnabled(),
						scene->weather.volumetricCloudsWeatherMapFirst.IsValid() ? &scene->weather.volumetricCloudsWeatherMapFirst.GetTexture() : nullptr,
						scene->weather.volumetricCloudsWeatherMapSecond.IsValid() ? &scene->weather.volumetricCloudsWeatherMapSecond.GetTexture() : nullptr
					);
				}
			});
		}

		// Main camera opaque color pass:
		cmd = device->BeginCommandList();
		device->WaitCommandList(cmd, cmd_maincamera_compute_effects);
		lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {

			GraphicsDevice* device = lb::graphics::GetDevice();
			device->EventBegin("Opaque Scene", cmd);

			lb::renderer::BindCameraCB(
				*camera,
				camera_previous,
				camera_reflection,
				cmd
			);

			if (getRaytracedReflectionEnabled())
			{
				lb::renderer::Postprocess_RTReflection(
					rtreflectionResources,
					*scene,
					rtSSR,
					cmd,
					getRaytracedReflectionsRange(),
					getReflectionRoughnessCutoff()
				);
			}
			if (lb::renderer::GetVXGIEnabled())
			{
				lb::renderer::VXGI_Resolve(
					vxgiResources,
					*scene,
					rtLinearDepth,
					cmd
				);
			}

			// Depth buffers were created on COMPUTE queue, so make them available for pixel shaders here:
			{
				GPUBarrier barriers[] = {
					GPUBarrier::Image(&rtLinearDepth, rtLinearDepth.desc.layout, ResourceState::SHADER_RESOURCE),
					GPUBarrier::Image(&depthBuffer_Copy, depthBuffer_Copy.desc.layout, ResourceState::SHADER_RESOURCE),
				};
				device->Barrier(barriers, arraysize(barriers), cmd);
			}

			if (lb::renderer::GetRaytracedShadowsEnabled() || lb::renderer::GetScreenSpaceShadowsEnabled())
			{
				GPUBarrier barrier = GPUBarrier::Image(&rtShadow, rtShadow.desc.layout, ResourceState::SHADER_RESOURCE);
				device->Barrier(&barrier, 1, cmd);
			}

			if (visibility_shading_in_compute)
			{
				lb::renderer::Visibility_Shade(
					visibilityResources,
					rtMain,
					cmd
				);
			}

			Viewport vp;
			vp.width = (float)depthBuffer_Main.GetDesc().width;
			vp.height = (float)depthBuffer_Main.GetDesc().height;
			device->BindViewports(1, &vp, cmd);

			Rect scissor = GetScissorInternalResolution();
			device->BindScissorRects(1, &scissor, cmd);

			if (getOutlineEnabled())
			{
				// Cut off outline source from linear depth:
				device->EventBegin("Outline Source", cmd);

				RenderPassImage rp[] = {
					RenderPassImage::RenderTarget(&rtOutlineSource, RenderPassImage::LoadOp::CLEAR),
					RenderPassImage::DepthStencil(&depthBuffer_Main, RenderPassImage::LoadOp::LOAD)
				};
				device->RenderPassBegin(rp, arraysize(rp), cmd);
				lb::image::Params params;
				params.enableFullScreen();
				params.stencilRefMode = lb::image::STENCILREFMODE_ENGINE;
				params.stencilComp = lb::image::STENCILMODE_EQUAL;
				params.stencilRef = lb::enums::STENCILREF_OUTLINE;
				lb::image::Draw(&rtLinearDepth, params, cmd);
				params.stencilRef = lb::enums::STENCILREF_CUSTOMSHADER_OUTLINE;
				lb::image::Draw(&rtLinearDepth, params, cmd);
				device->RenderPassEnd(cmd);
				device->EventEnd(cmd);
			}

			RenderPassImage rp[4] = {};
			uint32_t rp_count = 0;
			rp[rp_count++] = RenderPassImage::RenderTarget(
				&rtMain_render,
				visibility_shading_in_compute ? RenderPassImage::LoadOp::LOAD : RenderPassImage::LoadOp::CLEAR
			);
			rp[rp_count++] = RenderPassImage::DepthStencil(
				&depthBuffer_Main,
				RenderPassImage::LoadOp::LOAD,
				RenderPassImage::StoreOp::STORE,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL
			);
			if (getMSAASampleCount() > 1)
			{
				rp[rp_count++] = RenderPassImage::Resolve(&rtMain);
			}
			if (device->CheckCapability(GraphicsDeviceCapability::VARIABLE_RATE_SHADING_TIER2) && rtShadingRate.IsValid())
			{
				rp[rp_count++] = RenderPassImage::ShadingRateSource(&rtShadingRate, ResourceState::UNORDERED_ACCESS, ResourceState::UNORDERED_ACCESS);
			}
			device->RenderPassBegin(rp, rp_count, cmd, RenderPassFlags::ALLOW_UAV_WRITES);

			if (visibility_shading_in_compute)
			{
				// In visibility compute shading, the impostors must still be drawn using rasterization:
				lb::renderer::DrawScene(
					visibility_main,
					RENDERPASS_MAIN,
					cmd,
					lb::renderer::DRAWSCENE_IMPOSTOR
				);
			}
			else
			{
				auto range = lb::profiler::BeginRangeGPU("Opaque Scene", cmd);

				// Foreground:
				vp.min_depth = 1 - foreground_depth_range;
				vp.max_depth = 1;
				device->BindViewports(1, &vp, cmd);
				lb::renderer::DrawScene(
					visibility_main,
					RENDERPASS_MAIN,
					cmd,
					lb::renderer::DRAWSCENE_OPAQUE |
					lb::renderer::DRAWSCENE_FOREGROUND_ONLY |
					lb::renderer::DRAWSCENE_MAINCAMERA
				);

				// Regular:
				vp.min_depth = 0;
				vp.max_depth = 1;
				device->BindViewports(1, &vp, cmd);
				lb::renderer::DrawScene(
					visibility_main,
					RENDERPASS_MAIN,
					cmd,
					drawscene_flags
				);
				lb::renderer::DrawSky(*scene, cmd);
				lb::profiler::EndRange(range); // Opaque Scene
			}

			// Blend Aerial Perspective on top:
			if (scene->weather.IsRealisticSky() && scene->weather.IsRealisticSkyAerialPerspective())
			{
				device->EventBegin("Aerial Perspective Blend", cmd);
				lb::image::Params fx;
				fx.enableFullScreen();
				fx.blendFlag = BLENDMODE_PREMULTIPLIED;
				lb::image::Draw(&aerialperspectiveResources.texture_output, fx, cmd);
				device->EventEnd(cmd);
			}

			// Blend the volumetric clouds on top:
			if (scene->weather.IsVolumetricClouds())
			{
				lb::renderer::Postprocess_VolumetricClouds_Upsample(volumetriccloudResources, cmd);
			}

			RenderOutline(cmd);

			device->RenderPassEnd(cmd);

			if (lb::renderer::GetRaytracedShadowsEnabled() || lb::renderer::GetScreenSpaceShadowsEnabled())
			{
				GPUBarrier barrier = GPUBarrier::Image(&rtShadow, ResourceState::SHADER_RESOURCE, rtShadow.desc.layout);
				device->Barrier(&barrier, 1, cmd);
			}

			if (rtAO.IsValid())
			{
				device->Barrier(GPUBarrier::Aliasing(&rtAO, &rtParticleDistortion), cmd);
			}

			device->EventEnd(cmd);
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

		// Transparents, post processes, etc:
		cmd = device->BeginCommandList();
		if (cmd_ocean.IsValid())
		{
			device->WaitCommandList(cmd, cmd_ocean);
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

			RenderLightShafts(cmd);

			RenderVolumetrics(cmd);

			RenderTransparents(cmd);

			// Depth buffers expect a non-pixel shader resource state as they are generated on compute queue:
			{
				GPUBarrier barriers[] = {
					GPUBarrier::Image(&rtLinearDepth, ResourceState::SHADER_RESOURCE, rtLinearDepth.desc.layout),
					GPUBarrier::Image(&depthBuffer_Copy, ResourceState::SHADER_RESOURCE, depthBuffer_Copy.desc.layout),
					GPUBarrier::Image(&debugUAV, ResourceState::UNORDERED_ACCESS, debugUAV.desc.layout),
				};
				device->Barrier(barriers, arraysize(barriers), cmd);
			}
		});

		if (scene->IsWetmapProcessingRequired())
		{
			CommandList wetmap_cmd = device->BeginCommandList(QUEUE_COMPUTE);
			device->WaitCommandList(wetmap_cmd, cmd); // wait for transparents, it will be scheduled with late frame (GUI, etc)
			// Note: GPU processing of this compute task can overlap with beginning of the next frame because no one is waiting for it
			lb::jobsystem::Execute(ctx, [this, wetmap_cmd](lb::jobsystem::JobArgs args) {
				lb::renderer::RefreshWetmaps(visibility_main, wetmap_cmd);
			});
		}

		cmd = device->BeginCommandList();
		lb::jobsystem::Execute(ctx, [this, cmd](lb::jobsystem::JobArgs args) {
			RenderPostprocessChain(cmd);
			lb::renderer::TextureStreamingReadbackCopy(*scene, cmd);
		});

		RenderPath2D::Render();

		lb::jobsystem::Wait(ctx);

		first_frame = false;
	}

	void RenderPath3D::Compose(CommandList cmd) const
	{
		GraphicsDevice* device = lb::graphics::GetDevice();

		lb::image::Params fx;
		fx.blendFlag = BLENDMODE_OPAQUE;
		fx.quality = lb::image::QUALITY_LINEAR;
		fx.enableFullScreen();

		device->EventBegin("Composition", cmd);
		lb::image::Draw(GetLastPostprocessRT(), fx, cmd);
		device->EventEnd(cmd);

		if (
			lb::renderer::GetDebugLightCulling() ||
			lb::renderer::GetVariableRateShadingClassificationDebug()
			)
		{
			fx.enableFullScreen();
			fx.blendFlag = BLENDMODE_PREMULTIPLIED;
			lb::image::Draw(&debugUAV, fx, cmd);
		}

		RenderPath2D::Compose(cmd);
	}

	void RenderPath3D::Stop()
	{
		DeleteGPUResources();
	}

	void RenderPath3D::Start()
	{
		ResizeBuffers();
	}

	void RenderPath3D::RenderAO(CommandList cmd) const
	{
		if (rtAO.IsValid())
		{
			GetDevice()->Barrier(GPUBarrier::Aliasing(&rtParticleDistortion, &rtAO), cmd);
		}

		if (getAOEnabled())
		{
			switch (getAO())
			{
			case AO_SSAO:
				lb::renderer::Postprocess_SSAO(
					ssaoResources,
					rtLinearDepth,
					rtAO,
					cmd,
					getAORange(),
					getAOSampleCount(),
					getAOPower()
				);
				break;
			case AO_HBAO:
				lb::renderer::Postprocess_HBAO(
					ssaoResources,
					*camera,
					rtLinearDepth,
					rtAO,
					cmd,
					getAOPower()
				);
				break;
			case AO_RTAO:
				lb::renderer::Postprocess_RTAO(
					rtaoResources,
					*scene,
					rtLinearDepth,
					rtAO,
					cmd,
					getAORange(),
					getAOPower()
				);
				break;
			case AO_DISABLED:
				break;
			}
		}
	}
	void RenderPath3D::RenderSSR(CommandList cmd) const
	{
		if (getSSREnabled() && !getRaytracedReflectionEnabled())
		{
			lb::renderer::Postprocess_SSR(
				ssrResources,
				rtSceneCopy,
				rtSSR,
				cmd,
				getReflectionRoughnessCutoff()
			);
		}
	}
	void RenderPath3D::RenderOutline(CommandList cmd) const
	{
		if (getOutlineEnabled())
		{
			lb::renderer::Postprocess_Outline(
				rtOutlineSource,
				cmd,
				getOutlineThreshold(),
				getOutlineThickness(),
				getOutlineColor()
			);
		}
	}
	void RenderPath3D::RenderLightShafts(CommandList cmd) const
	{
		XMVECTOR sunDirection = XMLoadFloat3(&scene->weather.sunDirection);
		if (getLightShaftsEnabled() && XMVectorGetX(XMVector3Dot(sunDirection, camera->GetAt())) > 0)
		{
			GraphicsDevice* device = lb::graphics::GetDevice();

			device->EventBegin("Light Shafts", cmd);

			// Render sun stencil cutout:
			{
				if (getMSAASampleCount() > 1)
				{
					RenderPassImage rp[] = {
						RenderPassImage::DepthStencil(
							&depthBuffer_Main,
							RenderPassImage::LoadOp::LOAD,
							RenderPassImage::StoreOp::STORE,
							ResourceState::DEPTHSTENCIL,
							ResourceState::DEPTHSTENCIL,
							ResourceState::DEPTHSTENCIL
						),
						RenderPassImage::RenderTarget(&rtSun[0], RenderPassImage::LoadOp::CLEAR, RenderPassImage::StoreOp::DONTCARE),
						RenderPassImage::Resolve(&rtSun_resolved),
					};
					device->RenderPassBegin(rp, arraysize(rp), cmd);
				}
				else
				{
					RenderPassImage rp[] = {
						RenderPassImage::DepthStencil(
							&depthBuffer_Main,
							RenderPassImage::LoadOp::LOAD,
							RenderPassImage::StoreOp::STORE,
							ResourceState::DEPTHSTENCIL,
							ResourceState::DEPTHSTENCIL,
							ResourceState::DEPTHSTENCIL
						),
						RenderPassImage::RenderTarget(&rtSun[0], RenderPassImage::LoadOp::CLEAR),
					};
					device->RenderPassBegin(rp, arraysize(rp), cmd);
				}

				Viewport vp;
				vp.width = (float)depthBuffer_Main.GetDesc().width;
				vp.height = (float)depthBuffer_Main.GetDesc().height;
				device->BindViewports(1, &vp, cmd);

				Rect scissor = GetScissorInternalResolution();
				device->BindScissorRects(1, &scissor, cmd);

				lb::renderer::DrawSun(cmd);

				if (scene->weather.IsVolumetricClouds())
				{
					device->EventBegin("Volumetric cloud occlusion mask", cmd);
					lb::image::Params fx;
					fx.enableFullScreen();
					fx.blendFlag = BLENDMODE_MULTIPLY;
					lb::image::Draw(&volumetriccloudResources.texture_cloudMask, fx, cmd);
					device->EventEnd(cmd);
				}

				device->RenderPassEnd(cmd);
			}

			// Radial blur on the sun:
			{
				XMVECTOR sunPos = XMVector3Project(camera->GetEye() + sunDirection * camera->zFarP, 0, 0,
					1.0f, 1.0f, 0.1f, 1.0f,
					camera->GetProjection(), camera->GetView(), XMMatrixIdentity());
				{
					XMFLOAT2 sun;
					XMStoreFloat2(&sun, sunPos);
					lb::renderer::Postprocess_LightShafts(
						getMSAASampleCount() > 1 ? rtSun_resolved : rtSun[0],
						rtSun[1],
						cmd,
						sun,
						getLightShaftsStrength()
					);
				}
			}
			device->EventEnd(cmd);
		}
	}
	void RenderPath3D::RenderVolumetrics(CommandList cmd) const
	{
		if (getVolumeLightsEnabled() && visibility_main.IsRequestedVolumetricLights())
		{
			auto range = lb::profiler::BeginRangeGPU("Volumetric Lights", cmd);

			GraphicsDevice* device = lb::graphics::GetDevice();

			RenderPassImage rp[] = {
				RenderPassImage::RenderTarget(&rtVolumetricLights, RenderPassImage::LoadOp::CLEAR),
			};
			device->RenderPassBegin(rp, arraysize(rp), cmd);

			Viewport vp;
			vp.width = (float)rtVolumetricLights.GetDesc().width;
			vp.height = (float)rtVolumetricLights.GetDesc().height;
			device->BindViewports(1, &vp, cmd);

			lb::renderer::DrawVolumeLights(visibility_main, cmd);

			device->RenderPassEnd(cmd);

			lb::profiler::EndRange(range);
		}
	}
	void RenderPath3D::RenderSceneMIPChain(CommandList cmd) const
	{
		GraphicsDevice* device = lb::graphics::GetDevice();

		auto range = lb::profiler::BeginRangeGPU("Scene MIP Chain", cmd);
		device->EventBegin("RenderSceneMIPChain", cmd);

		{
			GPUBarrier barriers[] = {
				GPUBarrier::Aliasing(&rtPrimitiveID, &rtSceneCopy_tmp),
				GPUBarrier::Image(&rtSceneCopy_tmp, rtSceneCopy_tmp.desc.layout, ResourceState::UNORDERED_ACCESS),
			};
			device->Barrier(barriers, arraysize(barriers), cmd);
			device->ClearUAV(&rtSceneCopy_tmp, 0, cmd);
		}

		lb::renderer::Postprocess_Downsample4x(rtMain, rtSceneCopy, cmd);

		device->Barrier(GPUBarrier::Image(&rtSceneCopy_tmp, ResourceState::UNORDERED_ACCESS, rtSceneCopy_tmp.desc.layout), cmd);

		lb::renderer::MIPGEN_OPTIONS mipopt;
		mipopt.gaussian_temp = &rtSceneCopy_tmp;
		lb::renderer::GenerateMipChain(rtSceneCopy, lb::renderer::MIPGENFILTER_GAUSSIAN, cmd, mipopt);

		device->Barrier(GPUBarrier::Aliasing(&rtSceneCopy_tmp, &rtPrimitiveID), cmd);

		device->EventEnd(cmd);
		lb::profiler::EndRange(range);
	}
	void RenderPath3D::RenderTransparents(CommandList cmd) const
	{
		GraphicsDevice* device = lb::graphics::GetDevice();

		// Water ripple rendering:
		if (!scene->waterRipples.empty())
		{
			device->Barrier(GPUBarrier::Aliasing(&rtParticleDistortion, &rtWaterRipple), cmd);
			RenderPassImage rp[] = {
				RenderPassImage::RenderTarget(&rtWaterRipple, RenderPassImage::LoadOp::CLEAR),
			};
			device->RenderPassBegin(rp, arraysize(rp), cmd);

			Viewport vp;
			vp.width = (float)rtWaterRipple.GetDesc().width;
			vp.height = (float)rtWaterRipple.GetDesc().height;
			device->BindViewports(1, &vp, cmd);

			lb::renderer::DrawWaterRipples(visibility_main, cmd);
			device->RenderPassEnd(cmd);
		}

		Rect scissor = GetScissorInternalResolution();
		device->BindScissorRects(1, &scissor, cmd);

		Viewport vp;
		vp.width = (float)depthBuffer_Main.GetDesc().width;
		vp.height = (float)depthBuffer_Main.GetDesc().height;
		vp.min_depth = 0;
		vp.max_depth = 1;
		device->BindViewports(1, &vp, cmd);

		RenderPassImage rp[] = {
			RenderPassImage::RenderTarget(&rtMain_render, RenderPassImage::LoadOp::LOAD),
			RenderPassImage::DepthStencil(
				&depthBuffer_Main,
				RenderPassImage::LoadOp::LOAD,
				RenderPassImage::StoreOp::STORE,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL
			),
			RenderPassImage::Resolve(&rtMain),
		};

		// Draw only the ocean first, fog and lightshafts will be blended on top:
		if (scene->weather.IsOceanEnabled() && scene->ocean.IsValid() && (!scene->ocean.IsOccluded() || !lb::renderer::GetOcclusionCullingEnabled()))
		{
			device->EventBegin("Copy scene tex only mip0 for ocean", cmd);
			lb::renderer::Postprocess_Downsample4x(rtMain, rtSceneCopy, cmd);
			device->EventEnd(cmd);

			device->RenderPassBegin(rp, getMSAASampleCount() > 1 ? 3 : 2, cmd);

			lb::renderer::DrawScene(
				visibility_main,
				RENDERPASS_MAIN,
				cmd,
				lb::renderer::DRAWSCENE_OCEAN
			);

			device->RenderPassEnd(cmd);
		}

		if (visibility_main.IsTransparentsVisible())
		{
			RenderSceneMIPChain(cmd);
		}

		device->RenderPassBegin(rp, getMSAASampleCount() > 1 ? 3 : 2, cmd);

		// Note: volumetrics and light shafts are blended before transparent scene, because they used depth of the opaques
		//	But the ocean is special, because it does have depth for them implicitly computed from ocean plane

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
			fx.blendFlag = BLENDMODE_ADDITIVE;
			lb::image::Draw(&rtSun[1], fx, cmd);
			device->EventEnd(cmd);
		}

		// Transparent scene
		if (visibility_main.IsTransparentsVisible())
		{
			auto range = lb::profiler::BeginRangeGPU("Transparent Scene", cmd);
			device->EventBegin("Transparent Scene", cmd);

			// Regular:
			vp.min_depth = 0;
			vp.max_depth = 1;
			device->BindViewports(1, &vp, cmd);
			lb::renderer::DrawScene(
				visibility_main,
				RENDERPASS_MAIN,
				cmd,
				lb::renderer::DRAWSCENE_TRANSPARENT |
				lb::renderer::DRAWSCENE_TESSELLATION |
				lb::renderer::DRAWSCENE_OCCLUSIONCULLING |
				lb::renderer::DRAWSCENE_MAINCAMERA
			);

			// Foreground:
			vp.min_depth = 1 - foreground_depth_range;
			vp.max_depth = 1;
			device->BindViewports(1, &vp, cmd);
			lb::renderer::DrawScene(
				visibility_main,
				RENDERPASS_MAIN,
				cmd,
				lb::renderer::DRAWSCENE_TRANSPARENT |
				lb::renderer::DRAWSCENE_FOREGROUND_ONLY |
				lb::renderer::DRAWSCENE_MAINCAMERA
			);

			// Reset normal viewport:
			vp.min_depth = 0;
			vp.max_depth = 1;
			device->BindViewports(1, &vp, cmd);

			device->EventEnd(cmd);
			lb::profiler::EndRange(range); // Transparent Scene
		}

		lb::renderer::DrawDebugWorld(*scene, *camera, *this, cmd);

		lb::renderer::DrawLightVisualizers(visibility_main, cmd);

		lb::renderer::DrawSoftParticles(visibility_main, false, cmd);
		lb::renderer::DrawSpritesAndFonts(*scene, *camera, false, cmd);

		if (getLensFlareEnabled())
		{
			lb::renderer::DrawLensFlares(
				visibility_main,
				cmd,
				scene->weather.IsVolumetricClouds() ? &volumetriccloudResources.texture_cloudMask : nullptr
			);
		}

		device->RenderPassEnd(cmd);

		// Distortion particles:
		{
			if (rtWaterRipple.IsValid())
			{
				device->Barrier(GPUBarrier::Aliasing(&rtWaterRipple, &rtParticleDistortion), cmd);
			}

			if (getMSAASampleCount() > 1)
			{
				RenderPassImage rp[] = {
					RenderPassImage::RenderTarget(&rtParticleDistortion_render, RenderPassImage::LoadOp::CLEAR),
					RenderPassImage::DepthStencil(
						&depthBuffer_Main,
						RenderPassImage::LoadOp::LOAD,
						RenderPassImage::StoreOp::STORE,
						ResourceState::DEPTHSTENCIL,
						ResourceState::DEPTHSTENCIL,
						ResourceState::DEPTHSTENCIL
					),
					RenderPassImage::Resolve(&rtParticleDistortion)
				};
				device->RenderPassBegin(rp, arraysize(rp), cmd);
			}
			else
			{
				RenderPassImage rp[] = {
					RenderPassImage::RenderTarget(&rtParticleDistortion, RenderPassImage::LoadOp::CLEAR),
					RenderPassImage::DepthStencil(
						&depthBuffer_Main,
						RenderPassImage::LoadOp::LOAD,
						RenderPassImage::StoreOp::STORE,
						ResourceState::DEPTHSTENCIL,
						ResourceState::DEPTHSTENCIL,
						ResourceState::DEPTHSTENCIL
					),
				};
				device->RenderPassBegin(rp, arraysize(rp), cmd);
			}

			Viewport vp;
			vp.width = (float)rtParticleDistortion.GetDesc().width;
			vp.height = (float)rtParticleDistortion.GetDesc().height;
			device->BindViewports(1, &vp, cmd);

			lb::renderer::DrawSoftParticles(visibility_main, true, cmd);
			lb::renderer::DrawSpritesAndFonts(*scene, *camera, true, cmd);

			device->RenderPassEnd(cmd);
		}

		lb::renderer::Postprocess_Downsample4x(rtMain, rtSceneCopy, cmd);
	}
	void RenderPath3D::RenderPostprocessChain(CommandList cmd) const
	{
		GraphicsDevice* device = lb::graphics::GetDevice();

		lb::renderer::BindCommonResources(cmd);
		lb::renderer::BindCameraCB(*camera, camera_previous, camera_reflection, cmd);

		const Texture* rt_first = nullptr; // not ping-ponged with read / write
		const Texture* rt_read = &rtMain;
		const Texture* rt_write = &rtPostprocess;

		// rtPostprocess aliasing transition:
		{
			GPUBarrier barriers[] = {
				GPUBarrier::Aliasing(&rtPrimitiveID, &rtPostprocess),
				GPUBarrier::Image(&rtPostprocess, rtPostprocess.desc.layout, ResourceState::UNORDERED_ACCESS),
			};
			device->Barrier(barriers, arraysize(barriers), cmd);
			device->ClearUAV(&rtPostprocess, 0, cmd);
			device->Barrier(GPUBarrier::Image(&rtPostprocess, ResourceState::UNORDERED_ACCESS, rtPostprocess.desc.layout), cmd);
		}

		// 1.) HDR post process chain
		{
			if (lb::renderer::GetTemporalAAEnabled() && !lb::renderer::GetTemporalAADebugEnabled() && temporalAAResources.IsValid())
			{
				lb::renderer::Postprocess_TemporalAA(
					temporalAAResources,
					*rt_read,
					cmd
				);
				rt_first = temporalAAResources.GetCurrent();
			}

			if (scene->weather.IsOceanEnabled())
			{
				lb::renderer::Postprocess_Underwater(
					rt_first == nullptr ? *rt_read : *rt_first,
					*rt_write,
					cmd
				);

				rt_first = nullptr;
				std::swap(rt_read, rt_write);
			}

			for (auto& x : custom_post_processes)
			{
				if (x.stage == CustomPostprocess::Stage::BeforeTonemap)
				{
					lb::renderer::Postprocess_Custom(
						x.computeshader,
						rt_first == nullptr ? *rt_read : *rt_first,
						*rt_write,
						cmd,
						x.params0,
						x.params1,
						x.name.c_str()
					);

					rt_first = nullptr;
					std::swap(rt_read, rt_write);
				}
			}

			if (getDepthOfFieldEnabled() && camera->aperture_size > 0.001f && getDepthOfFieldStrength() > 0.001f && depthoffieldResources.IsValid())
			{
				lb::renderer::Postprocess_DepthOfField(
					depthoffieldResources,
					rt_first == nullptr ? *rt_read : *rt_first,
					*rt_write,
					cmd,
					getDepthOfFieldStrength()
				);

				rt_first = nullptr;
				std::swap(rt_read, rt_write);
			}

			if (getMotionBlurEnabled() && getMotionBlurStrength() > 0 && motionblurResources.IsValid())
			{
				lb::renderer::Postprocess_MotionBlur(
					scene->dt,
					motionblurResources,
					rt_first == nullptr ? *rt_read : *rt_first,
					*rt_write,
					cmd,
					getMotionBlurStrength()
				);

				rt_first = nullptr;
				std::swap(rt_read, rt_write);
			}
		}

		// 2.) Tone mapping HDR -> LDR
		{
			// Bloom and eye adaption is not part of post process "chain",
			//	because they will be applied to the screen in tonemap
			if (getEyeAdaptionEnabled())
			{
				lb::renderer::ComputeLuminance(
					luminanceResources,
					rt_first == nullptr ? *rt_read : *rt_first,
					cmd,
					getEyeAdaptionRate(),
					getEyeAdaptionKey()
				);
			}
			if (getBloomEnabled())
			{
				lb::renderer::ComputeBloom(
					bloomResources,
					rt_first == nullptr ? *rt_read : *rt_first,
					cmd,
					getBloomThreshold(),
					getExposure(),
					getEyeAdaptionEnabled() ? &luminanceResources.luminance : nullptr
				);
			}

			lb::renderer::Postprocess_Tonemap(
				rt_first == nullptr ? *rt_read : *rt_first,
				*rt_write,
				cmd,
				getExposure(),
				getBrightness(),
				getContrast(),
				getSaturation(),
				getDitherEnabled(),
				getColorGradingEnabled() ? (scene->weather.colorGradingMap.IsValid() ? &scene->weather.colorGradingMap.GetTexture() : nullptr) : nullptr,
				&rtParticleDistortion,
				getEyeAdaptionEnabled() ? &luminanceResources.luminance : nullptr,
				getBloomEnabled() ? &bloomResources.texture_bloom : nullptr,
				colorspace,
				getTonemap(),
				&distortion_overlay,
				getHDRCalibration()
			);

			rt_first = nullptr;
			std::swap(rt_read, rt_write);
		}

		// 3.) LDR post process chain
		{
			for (auto& x : custom_post_processes)
			{
				if (x.stage == CustomPostprocess::Stage::AfterTonemap)
				{
					lb::renderer::Postprocess_Custom(
						x.computeshader,
						*rt_read,
						*rt_write,
						cmd,
						x.params0,
						x.params1,
						x.name.c_str()
					);

					std::swap(rt_read, rt_write);
				}
			}

			if (getSharpenFilterEnabled())
			{
				lb::renderer::Postprocess_Sharpen(*rt_read, *rt_write, cmd, getSharpenFilterAmount());

				std::swap(rt_read, rt_write);
			}

			if (getFXAAEnabled())
			{
				lb::renderer::Postprocess_FXAA(*rt_read, *rt_write, cmd);

				std::swap(rt_read, rt_write);
			}

			if (getChromaticAberrationEnabled())
			{
				lb::renderer::Postprocess_Chromatic_Aberration(*rt_read, *rt_write, cmd, getChromaticAberrationAmount());

				std::swap(rt_read, rt_write);
			}

			lastPostprocessRT = rt_read;

			// GUI Background blurring:
			{
				auto range = lb::profiler::BeginRangeGPU("GUI Background Blur", cmd);
				device->EventBegin("GUI Background Blur", cmd);
				lb::renderer::Postprocess_Downsample4x(*rt_read, rtGUIBlurredBackground[0], cmd);
				lb::renderer::Postprocess_Downsample4x(rtGUIBlurredBackground[0], rtGUIBlurredBackground[2], cmd);
				lb::renderer::Postprocess_Blur_Gaussian(rtGUIBlurredBackground[2], rtGUIBlurredBackground[1], rtGUIBlurredBackground[2], cmd, -1, -1, true);
				device->EventEnd(cmd);
				lb::profiler::EndRange(range);
			}
		}
	}

	void RenderPath3D::setAO(AO value)
	{
		ao = value;

		if (!rtParticleDistortion.IsValid())
			return; // ResizeBuffers hasn't been called yet

		rtAO = {};
		ssaoResources = {};
		rtaoResources = {};

		if (ao == AO_DISABLED)
		{
			return;
		}

		XMUINT2 internalResolution = GetInternalResolution();
		if (internalResolution.x == 0 || internalResolution.y == 0)
			return;

		TextureDesc desc;
		desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS | BindFlag::RENDER_TARGET; // render target binding for aliasing (in case resource heap tier < 2)
		desc.format = Format::R8_UNORM;
		desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;

		switch (ao)
		{
		case RenderPath3D::AO_SSAO:
		case RenderPath3D::AO_HBAO:
			desc.width = internalResolution.x / 2;
			desc.height = internalResolution.y / 2;
			lb::renderer::CreateSSAOResources(ssaoResources, internalResolution);
			break;
		case RenderPath3D::AO_RTAO:
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			lb::renderer::CreateRTAOResources(rtaoResources, internalResolution);
			break;
		default:
			break;
		}

		GraphicsDevice* device = lb::graphics::GetDevice();
		assert(ComputeTextureMemorySizeInBytes(desc) <= ComputeTextureMemorySizeInBytes(rtParticleDistortion.desc)); // aliasing check
		device->CreateTexture(&desc, nullptr, &rtAO, &rtParticleDistortion); // aliasing!
		device->SetName(&rtAO, "rtAO");
	}
	void RenderPath3D::setSSREnabled(bool value)
	{
		ssrEnabled = value;

		if (value)
		{
			GraphicsDevice* device = lb::graphics::GetDevice();
			XMUINT2 internalResolution = GetInternalResolution();
			if (internalResolution.x == 0 || internalResolution.y == 0)
				return;

			TextureDesc desc;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = Format::R16G16B16A16_FLOAT;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.layout = ResourceState::SHADER_RESOURCE_COMPUTE;
			device->CreateTexture(&desc, nullptr, &rtSSR);
			device->SetName(&rtSSR, "rtSSR");

			lb::renderer::CreateSSRResources(ssrResources, internalResolution);
		}
		else
		{
			ssrResources = {};
		}
	}
	void RenderPath3D::setRaytracedReflectionsEnabled(bool value)
	{
		raytracedReflectionsEnabled = value;

		if (value)
		{
			GraphicsDevice* device = lb::graphics::GetDevice();
			XMUINT2 internalResolution = GetInternalResolution();
			if (internalResolution.x == 0 || internalResolution.y == 0)
				return;

			TextureDesc desc;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = Format::R16G16B16A16_FLOAT;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			device->CreateTexture(&desc, nullptr, &rtSSR);
			device->SetName(&rtSSR, "rtSSR");

			lb::renderer::CreateRTReflectionResources(rtreflectionResources, internalResolution);
		}
		else
		{
			rtreflectionResources = {};
		}
	}
	void RenderPath3D::setMotionBlurEnabled(bool value)
	{
		motionBlurEnabled = value;
	}
	void RenderPath3D::setDepthOfFieldEnabled(bool value)
	{
		depthOfFieldEnabled = value;
	}
	void RenderPath3D::setEyeAdaptionEnabled(bool value)
	{
		eyeAdaptionEnabled = value;

		if (value)
		{
			lb::renderer::CreateLuminanceResources(luminanceResources, GetInternalResolution());
		}
		else
		{
			luminanceResources = {};
		}
	}
	void RenderPath3D::setReflectionsEnabled(bool value)
	{
		reflectionsEnabled = value;

		if (value)
		{
			GraphicsDevice* device = lb::graphics::GetDevice();
			XMUINT2 internalResolution = GetInternalResolution();
			if (internalResolution.x == 0 || internalResolution.y == 0)
				return;

			TextureDesc desc;
			desc.sample_count = 4;
			desc.bind_flags = BindFlag::RENDER_TARGET;
			desc.format = lb::renderer::format_rendertarget_main;
			desc.width = internalResolution.x / 4;
			desc.height = internalResolution.y / 4;
			desc.misc_flags = ResourceMiscFlag::TRANSIENT_ATTACHMENT;
			desc.layout = ResourceState::RENDERTARGET;
			device->CreateTexture(&desc, nullptr, &rtReflection);
			device->SetName(&rtReflection, "rtReflection");

			desc.misc_flags = ResourceMiscFlag::NONE;
			desc.bind_flags = BindFlag::DEPTH_STENCIL | BindFlag::SHADER_RESOURCE;
			desc.format = lb::renderer::format_depthbuffer_main;
			desc.layout = ResourceState::SHADER_RESOURCE;
			device->CreateTexture(&desc, nullptr, &depthBuffer_Reflection);
			device->SetName(&depthBuffer_Reflection, "depthBuffer_Reflection");


			desc.sample_count = 1;
			desc.format = lb::renderer::format_rendertarget_main;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
			device->CreateTexture(&desc, nullptr, &rtReflection_resolved);
			device->SetName(&rtReflection_resolved, "rtReflection_resolved");

			desc.format = Format::R16_UNORM;
			desc.bind_flags = BindFlag::UNORDERED_ACCESS | BindFlag::SHADER_RESOURCE;
			device->CreateTexture(&desc, nullptr, &depthBuffer_Reflection_resolved);
			device->SetName(&depthBuffer_Reflection_resolved, "depthBuffer_Reflection_resolved");

			lb::renderer::CreateTiledLightResources(tiledLightResources_planarReflection, XMUINT2(depthBuffer_Reflection.desc.width, depthBuffer_Reflection.desc.height));
		}
		else
		{
			rtReflection = {};
			depthBuffer_Reflection = {};
			tiledLightResources_planarReflection = {};
		}
	}
	void RenderPath3D::setBloomEnabled(bool value)
	{
		bloomEnabled = value;

		if (value)
		{
			lb::renderer::CreateBloomResources(bloomResources, GetInternalResolution());
		}
		else
		{
			bloomResources = {};
		}
	}
	void RenderPath3D::setVolumeLightsEnabled(bool value)
	{
		volumeLightsEnabled = value;

		if (value)
		{
			GraphicsDevice* device = lb::graphics::GetDevice();
			XMUINT2 internalResolution = GetInternalResolution();
			if (internalResolution.x == 0 || internalResolution.y == 0)
				return;

			TextureDesc desc;
			desc.format = Format::R16G16B16A16_FLOAT;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.width = internalResolution.x / 2;
			desc.height = internalResolution.y / 2;
			device->CreateTexture(&desc, nullptr, &rtVolumetricLights);
			device->SetName(&rtVolumetricLights, "rtVolumetricLights");
		}
		else
		{
			rtVolumetricLights = {};
		}
	}
	void RenderPath3D::setLightShaftsEnabled(bool value)
	{
		lightShaftsEnabled = value;

		if (value)
		{
			GraphicsDevice* device = lb::graphics::GetDevice();
			XMUINT2 internalResolution = GetInternalResolution();
			if (internalResolution.x == 0 || internalResolution.y == 0)
				return;

			TextureDesc desc;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
			desc.format = lb::renderer::format_rendertarget_main;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.sample_count = getMSAASampleCount();
			device->CreateTexture(&desc, nullptr, &rtSun[0]);
			device->SetName(&rtSun[0], "rtSun[0]");

			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.sample_count = 1;
			desc.width = internalResolution.x / 2;
			desc.height = internalResolution.y / 2;
			device->CreateTexture(&desc, nullptr, &rtSun[1]);
			device->SetName(&rtSun[1], "rtSun[1]");

			if (getMSAASampleCount() > 1)
			{
				desc.width = internalResolution.x;
				desc.height = internalResolution.y;
				desc.sample_count = 1;
				device->CreateTexture(&desc, nullptr, &rtSun_resolved);
				device->SetName(&rtSun_resolved, "rtSun_resolved");
			}
		}
		else
		{
			rtSun[0] = {};
			rtSun[1] = {};
			rtSun_resolved = {};
		}
	}
	void RenderPath3D::setOutlineEnabled(bool value)
	{
		outlineEnabled = value;

		if (value)
		{
			GraphicsDevice* device = lb::graphics::GetDevice();
			XMUINT2 internalResolution = GetInternalResolution();
			if (internalResolution.x == 0 || internalResolution.y == 0)
				return;

			TextureDesc desc;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;
			desc.format = Format::R32_FLOAT;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			device->CreateTexture(&desc, nullptr, &rtOutlineSource);
			device->SetName(&rtOutlineSource, "rtOutlineSource");
		}
		else
		{
			rtOutlineSource = {};
		}
	}

}
