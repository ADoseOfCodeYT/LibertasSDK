#include "stdafx.h"
#include "GraphicsWindow.h"
#include "shaders/ShaderInterop_DDGI.h"

using namespace lb::graphics;

void GraphicsWindow::Create(EditorComponent* _editor)
{
	editor = _editor;
	lb::gui::Window::Create("Graphics", lb::gui::Window::WindowControls::CLOSE | lb::gui::Window::WindowControls::RESIZE_RIGHT);
	SetText("Graphics Options " ICON_GRAPHICSOPTIONS);

	lb::renderer::SetToDrawDebugEnvProbes(true);
	lb::renderer::SetToDrawGridHelper(true);
	lb::renderer::SetToDrawDebugCameras(true);

	SetSize(XMFLOAT2(300, 1680));

	float step = 21;
	float itemheight = 18;
	float x = 160;
	float y = 0;
	float wid = 110;

	vsyncCheckBox.Create("VSync: ");
	vsyncCheckBox.SetTooltip("Toggle vertical sync");
	vsyncCheckBox.SetScriptTip("SetVSyncEnabled(opt bool enabled)");
	vsyncCheckBox.SetPos(XMFLOAT2(x, y));
	vsyncCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("vsync"))
	{
		vsyncCheckBox.SetCheck(editor->main->config.GetSection("graphics").GetBool("vsync"));
	}
	else
	{
		vsyncCheckBox.SetCheck(editor->main->swapChain.desc.vsync);
	}
	vsyncCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->main->config.GetSection("graphics").Set("vsync", args.bValue);
		editor->main->config.Commit();
	});
	AddWidget(&vsyncCheckBox);

	swapchainComboBox.Create("Display Output: ");
	swapchainComboBox.SetSize(XMFLOAT2(wid, itemheight));
	swapchainComboBox.SetPos(XMFLOAT2(x, y += step));
	swapchainComboBox.SetTooltip("Choose between different display output formats.\nIf the display doesn't support the selected format, it will switch back to a reasonable default.\nHDR formats will be only selectable when the current display supports HDR output");
	AddWidget(&swapchainComboBox);
	UpdateSwapChainFormats(&editor->main->swapChain);

	resolutionScaleSlider.Create(0.25f, 2.0f, 1.0f, 7.0f, "Resolution Scale: ");
	resolutionScaleSlider.SetTooltip("Adjust the internal rendering resolution.");
	resolutionScaleSlider.SetSize(XMFLOAT2(wid, itemheight));
	resolutionScaleSlider.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("resolution_scale"))
	{
		editor->resolutionScale = editor->main->config.GetSection("graphics").GetFloat("resolution_scale");
	}
	resolutionScaleSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->resolutionScale = args.fValue;
		editor->main->config.GetSection("graphics").Set("resolution_scale", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&resolutionScaleSlider);

	streamingSlider.Create(0.1f, 1.0f, lb::resourcemanager::GetStreamingMemoryThreshold(), 100, "Streaming Threshold: ");
	streamingSlider.SetTooltip("Adjust the threshold for texture streaming (percent of available video memory).\nIf VRAM is used above the budget, the streaming system will try to reduce textures.\nNote: only DDS textures support streaming.");
	streamingSlider.SetSize(XMFLOAT2(wid, itemheight));
	streamingSlider.SetPos(XMFLOAT2(x, y += step));
	streamingSlider.OnSlide([=](lb::gui::EventArgs args) {
		lb::resourcemanager::SetStreamingMemoryThreshold(args.fValue);
	});
	AddWidget(&streamingSlider);

	renderPathComboBox.Create("Render Path: ");
	renderPathComboBox.SetSize(XMFLOAT2(wid, itemheight));
	renderPathComboBox.SetPos(XMFLOAT2(x, y += step));
	renderPathComboBox.AddItem("Default", RENDERPATH_DEFAULT);
	renderPathComboBox.AddItem("Path Tracing", RENDERPATH_PATHTRACING);
	renderPathComboBox.OnSelect([&](lb::gui::EventArgs args) {
		ChangeRenderPath((RENDERPATH)args.userdata);
		});
	renderPathComboBox.SetEnabled(true);
	renderPathComboBox.SetTooltip("Choose a render path...\nPath tracing will use fallback raytracing with non-raytracing GPU, which will be slow.\nChanging render path will reset some graphics settings!");
	AddWidget(&renderPathComboBox);

	pathTraceTargetSlider.Create(1, 2048, 1024, 2047, "Sample count: ");
	pathTraceTargetSlider.SetSize(XMFLOAT2(wid, itemheight));
	pathTraceTargetSlider.SetPos(XMFLOAT2(x, y += step));
	pathTraceTargetSlider.SetTooltip("The path tracing will perform this many samples per pixel.");
	AddWidget(&pathTraceTargetSlider);

	pathTraceStatisticsLabel.Create("Path tracing statistics");
	pathTraceStatisticsLabel.SetSize(XMFLOAT2(wid, 70));
	pathTraceStatisticsLabel.SetPos(XMFLOAT2(x, y += step));
	AddWidget(&pathTraceStatisticsLabel);


	renderPathComboBox.SetSelected(RENDERPATH_DEFAULT);


	occlusionCullingCheckBox.Create("Occlusion Culling: ");
	occlusionCullingCheckBox.SetTooltip("Toggle occlusion culling. This can boost framerate if many objects are occluded in the scene.");
	occlusionCullingCheckBox.SetScriptTip("SetOcclusionCullingEnabled(bool enabled)");
	occlusionCullingCheckBox.SetPos(XMFLOAT2(x, y += step));
	occlusionCullingCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	occlusionCullingCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetOcclusionCullingEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("occlusion_culling", args.bValue);
		editor->main->config.Commit();
	});
	if (editor->main->config.GetSection("graphics").Has("occlusion_culling"))
	{
		lb::renderer::SetOcclusionCullingEnabled(editor->main->config.GetSection("graphics").GetBool("occlusion_culling"));
	}
	AddWidget(&occlusionCullingCheckBox);

	visibilityComputeShadingCheckBox.Create("Visibility Compute Shading: ");
	visibilityComputeShadingCheckBox.SetTooltip("Visibility Compute Shading (experimental)\nThis will shade the scene in compute shaders instead of pixel shaders\nThis has a higher initial performance cost, but it will be faster in high polygon scenes.\nIt is not compatible with MSAA and tessellation.");
	visibilityComputeShadingCheckBox.SetPos(XMFLOAT2(x, y += step));
	visibilityComputeShadingCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("visibility_compute_shading"))
	{
		editor->renderPath->setVisibilityComputeShadingEnabled(editor->main->config.GetSection("graphics").GetBool("visibility_compute_shading"));
	}
	visibilityComputeShadingCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setVisibilityComputeShadingEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("visibility_compute_shading", args.bValue);
		editor->main->config.Commit();
	});
	AddWidget(&visibilityComputeShadingCheckBox);

	meshShaderCheckBox.Create("Allow Mesh Shader: ");
	meshShaderCheckBox.SetTooltip("Allow using mesh shaders to render objects (Requires support from GPU).\nNote: scene must be reloaded for this to take effect, as this will require additional mesh processing!");
	meshShaderCheckBox.SetPos(XMFLOAT2(x, y += step));
	meshShaderCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("mesh_shader"))
	{
		lb::renderer::SetMeshShaderAllowed(editor->main->config.GetSection("graphics").GetBool("mesh_shader"));
	}
	meshShaderCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetMeshShaderAllowed(args.bValue);
		editor->main->config.GetSection("graphics").Set("mesh_shader", args.bValue);
		editor->main->config.Commit();
	});
	AddWidget(&meshShaderCheckBox);
	meshShaderCheckBox.SetEnabled(lb::graphics::GetDevice()->CheckCapability(lb::graphics::GraphicsDeviceCapability::MESH_SHADER));

	meshletOcclusionCullingCheckBox.Create("Meshlet Occlusion Culling: ");
	meshletOcclusionCullingCheckBox.SetTooltip("Whether individual meshlets can use specialized occlusion culling (requires GPU support for mesh shaders).");
	meshletOcclusionCullingCheckBox.SetPos(XMFLOAT2(x, y += step));
	meshletOcclusionCullingCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("meshlet_occlusion_culling"))
	{
		lb::renderer::SetMeshletOcclusionCullingEnabled(editor->main->config.GetSection("graphics").GetBool("meshlet_occlusion_culling"));
	}
	meshletOcclusionCullingCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetMeshletOcclusionCullingEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("meshlet_occlusion_culling", args.bValue);
		editor->main->config.Commit();
	});
	AddWidget(&meshletOcclusionCullingCheckBox);
	meshletOcclusionCullingCheckBox.SetEnabled(lb::graphics::GetDevice()->CheckCapability(lb::graphics::GraphicsDeviceCapability::MESH_SHADER));

	GIBoostSlider.Create(1, 10, 1.0f, 1000.0f, "GI Boost: ");
	GIBoostSlider.SetTooltip("Adjust the strength of GI.\nNote that values other than 1.0 will cause mismatch with path tracing reference!");
	GIBoostSlider.SetSize(XMFLOAT2(wid, itemheight));
	GIBoostSlider.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("gi_boost"))
	{
		lb::renderer::SetGIBoost(editor->main->config.GetSection("graphics").GetFloat("gi_boost"));
	}
	GIBoostSlider.OnSlide([=](lb::gui::EventArgs args) {
		lb::renderer::SetGIBoost(args.fValue);
		editor->main->config.GetSection("graphics").Set("gi_boost", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&GIBoostSlider);

	ddgiCheckBox.Create("DDGI: ");
	ddgiCheckBox.SetTooltip("Toggle Dynamic Diffuse Global Illumination (DDGI).\nNote that DDGI probes that were loaded with the scene will still be active if this is turned off, but they won't be updated.");
	ddgiCheckBox.SetPos(XMFLOAT2(x, y += step));
	ddgiCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	ddgiCheckBox.OnClick([](lb::gui::EventArgs args) {
		lb::renderer::SetDDGIEnabled(args.bValue);
		});
	ddgiCheckBox.SetCheck(lb::renderer::GetDDGIEnabled());
	AddWidget(&ddgiCheckBox);

	ddgiDebugCheckBox.Create("DEBUG: ");
	ddgiDebugCheckBox.SetTooltip("Toggle DDGI probe visualization.");
	ddgiDebugCheckBox.SetPos(XMFLOAT2(x + wid + 1, y));
	ddgiDebugCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	ddgiDebugCheckBox.OnClick([](lb::gui::EventArgs args) {
		lb::renderer::SetDDGIDebugEnabled(args.bValue);
		});
	ddgiDebugCheckBox.SetCheck(lb::renderer::GetDDGIDebugEnabled());
	AddWidget(&ddgiDebugCheckBox);

	ddgiRayCountSlider.Create(0, DDGI_MAX_RAYCOUNT, 64, DDGI_MAX_RAYCOUNT, "DDGI RayCount: ");
	ddgiRayCountSlider.SetTooltip("Adjust the ray count per DDGI probe.");
	ddgiRayCountSlider.SetSize(XMFLOAT2(wid, itemheight));
	ddgiRayCountSlider.SetPos(XMFLOAT2(x, y += step));
	ddgiRayCountSlider.SetValue((float)lb::renderer::GetDDGIRayCount());
	ddgiRayCountSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::renderer::SetDDGIRayCount((uint32_t)args.iValue);
		});
	AddWidget(&ddgiRayCountSlider);

	ddgiBlendSpeedSlider.Create(0, 0.2f, 0.1f, 1000, "DDGI Blend Speed: ");
	ddgiBlendSpeedSlider.SetTooltip("Adjust the contribution of newly traced rays. Higher values will make the DDGI update faster, but can result in increased flickering.");
	ddgiBlendSpeedSlider.SetSize(XMFLOAT2(wid, itemheight));
	ddgiBlendSpeedSlider.SetPos(XMFLOAT2(x, y += step));
	ddgiBlendSpeedSlider.SetValue(lb::renderer::GetDDGIBlendSpeed());
	ddgiBlendSpeedSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::renderer::SetDDGIBlendSpeed(args.fValue);
		});
	AddWidget(&ddgiBlendSpeedSlider);

	ddgiSmoothBackfaceSlider.Create(0, 1, 0, 1000, "DDGI Smoothen: ");
	ddgiSmoothBackfaceSlider.SetTooltip("Adjust the amount of smooth backface test.");
	ddgiSmoothBackfaceSlider.SetSize(XMFLOAT2(wid, itemheight));
	ddgiSmoothBackfaceSlider.SetPos(XMFLOAT2(x, y += step));
	ddgiSmoothBackfaceSlider.OnSlide([=](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		scene.ddgi.smooth_backface = args.fValue;
		});
	AddWidget(&ddgiSmoothBackfaceSlider);

	ddgiX.Create("");
	ddgiX.SetTooltip("Probe count in X dimension.");
	ddgiX.SetDescription("DDGI Probes: ");
	ddgiX.SetPos(XMFLOAT2(x, y += step));
	ddgiX.SetSize(XMFLOAT2(40, itemheight));
	ddgiX.OnInputAccepted([=](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		auto grid_dimensions = scene.ddgi.grid_dimensions;
		grid_dimensions.x = (uint32_t)args.iValue;
		scene.ddgi = {}; // reset ddgi
		scene.ddgi.grid_dimensions = grid_dimensions;
	});
	AddWidget(&ddgiX);

	ddgiY.Create("");
	ddgiY.SetTooltip("Probe count in Y dimension.");
	ddgiY.SetPos(XMFLOAT2(x + 45, y));
	ddgiY.SetSize(XMFLOAT2(40, itemheight));
	ddgiY.OnInputAccepted([=](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		auto grid_dimensions = scene.ddgi.grid_dimensions;
		grid_dimensions.y = (uint32_t)args.iValue;
		scene.ddgi = {}; // reset ddgi
		scene.ddgi.grid_dimensions = grid_dimensions;
		});
	AddWidget(&ddgiY);

	ddgiZ.Create("");
	ddgiZ.SetTooltip("Probe count in Z dimension.");
	ddgiZ.SetPos(XMFLOAT2(x + 45 * 2, y));
	ddgiZ.SetSize(XMFLOAT2(40, itemheight));
	ddgiZ.OnInputAccepted([=](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		auto grid_dimensions = scene.ddgi.grid_dimensions;
		grid_dimensions.z = (uint32_t)args.iValue;
		scene.ddgi = {}; // reset ddgi
		scene.ddgi.grid_dimensions = grid_dimensions;
		});
	AddWidget(&ddgiZ);

	vxgiCheckBox.Create("VXGI: ");
	vxgiCheckBox.SetTooltip("Toggle Voxel Global Illumination.");
	vxgiCheckBox.SetPos(XMFLOAT2(x, y += step));
	vxgiCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("vxgi"))
	{
		lb::renderer::SetVXGIEnabled(editor->main->config.GetSection("graphics").GetBool("vxgi"));
	}
	vxgiCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetVXGIEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("vxgi", args.bValue);
		editor->main->config.Commit();
	});
	vxgiCheckBox.SetCheck(lb::renderer::GetVXGIEnabled());
	AddWidget(&vxgiCheckBox);

	vxgiDebugCombo.Create("");
	vxgiDebugCombo.SetTooltip("Toggle VXGI visualization.");
	vxgiDebugCombo.SetPos(XMFLOAT2(x + wid + 1, y));
	vxgiDebugCombo.SetSize(XMFLOAT2(80, itemheight));
	vxgiDebugCombo.AddItem("No debug", 0);
	vxgiDebugCombo.AddItem("Clipmaps", VXGI_CLIPMAP_COUNT);
	for (uint32_t i = 0; i < VXGI_CLIPMAP_COUNT; ++i)
	{
		vxgiDebugCombo.AddItem("Clipmap " + std::to_string(i), i);
	}
	vxgiDebugCombo.OnSelect([](lb::gui::EventArgs args) {
		lb::renderer::SetToDrawVoxelHelper(args.iValue != 0, (int)args.userdata);
	});
	AddWidget(&vxgiDebugCombo);

	vxgiReflectionsCheckBox.Create("VXGI Reflections: ");
	vxgiReflectionsCheckBox.SetTooltip("Toggle specular reflections computation for VXGI.");
	vxgiReflectionsCheckBox.SetPos(XMFLOAT2(x + wid + 1, y));
	vxgiReflectionsCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("vxgi.reflections"))
	{
		lb::renderer::SetVXGIReflectionsEnabled(editor->main->config.GetSection("graphics").GetBool("vxgi.reflections"));
	}
	vxgiReflectionsCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetVXGIReflectionsEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("vxgi.reflections", args.bValue);
		editor->main->config.Commit();
	});
	vxgiReflectionsCheckBox.SetCheck(lb::renderer::GetVXGIReflectionsEnabled());
	AddWidget(&vxgiReflectionsCheckBox);

	vxgiVoxelSizeSlider.Create(0.125f, 0.5f, 1, 7, "VXGI Voxel Size: ");
	vxgiVoxelSizeSlider.SetTooltip("Adjust the voxel size for VXGI calculations.");
	vxgiVoxelSizeSlider.SetSize(XMFLOAT2(wid, itemheight));
	vxgiVoxelSizeSlider.SetPos(XMFLOAT2(x, y += step));
	vxgiVoxelSizeSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		scene.vxgi.clipmaps[0].voxelsize = args.fValue;
	});
	AddWidget(&vxgiVoxelSizeSlider);

	vxgiRayStepSizeSlider.Create(0.5f, 2.0f, 1.0f, 10000, "VXGI Ray Step: ");
	vxgiRayStepSizeSlider.SetTooltip("Adjust the precision of ray marching for [reflection] cone tracing step. Lower values = more precision but slower performance.");
	vxgiRayStepSizeSlider.SetSize(XMFLOAT2(wid, itemheight));
	vxgiRayStepSizeSlider.SetPos(XMFLOAT2(x, y += step));
	vxgiRayStepSizeSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		scene.vxgi.rayStepSize = args.fValue;
	});
	AddWidget(&vxgiRayStepSizeSlider);

	vxgiMaxDistanceSlider.Create(0, 100, 10, 10000, "VXGI Distance: ");
	vxgiMaxDistanceSlider.SetTooltip("Adjust max raymarching distance for VXGI.");
	vxgiMaxDistanceSlider.SetSize(XMFLOAT2(wid, itemheight));
	vxgiMaxDistanceSlider.SetPos(XMFLOAT2(x, y += step));
	vxgiMaxDistanceSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::scene::Scene& scene = editor->GetCurrentScene();
		scene.vxgi.maxDistance = args.fValue;
	});
	AddWidget(&vxgiMaxDistanceSlider);

	variableRateShadingClassificationCheckBox.Create("VRS Classification: ");
	variableRateShadingClassificationCheckBox.SetTooltip("Enable classification of variable rate shading on the screen. Less important parts will be shaded with lesser resolution.\nRequires Tier2 support for variable shading rate");
	variableRateShadingClassificationCheckBox.SetPos(XMFLOAT2(x, y += step));
	variableRateShadingClassificationCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	variableRateShadingClassificationCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetVariableRateShadingClassification(args.bValue);
		editor->ResizeBuffers();
		});
	variableRateShadingClassificationCheckBox.SetCheck(lb::renderer::GetVariableRateShadingClassification());
	AddWidget(&variableRateShadingClassificationCheckBox);
	variableRateShadingClassificationCheckBox.SetEnabled(lb::graphics::GetDevice()->CheckCapability(lb::graphics::GraphicsDeviceCapability::VARIABLE_RATE_SHADING_TIER2));

	variableRateShadingClassificationDebugCheckBox.Create("DEBUG: ");
	variableRateShadingClassificationDebugCheckBox.SetTooltip("Toggle visualization of variable rate shading classification feature");
	variableRateShadingClassificationDebugCheckBox.SetPos(XMFLOAT2(x + wid + 1, y));
	variableRateShadingClassificationDebugCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	variableRateShadingClassificationDebugCheckBox.OnClick([](lb::gui::EventArgs args) {
		lb::renderer::SetVariableRateShadingClassificationDebug(args.bValue);
		});
	variableRateShadingClassificationDebugCheckBox.SetCheck(lb::renderer::GetVariableRateShadingClassificationDebug());
	AddWidget(&variableRateShadingClassificationDebugCheckBox);
	variableRateShadingClassificationDebugCheckBox.SetEnabled(lb::graphics::GetDevice()->CheckCapability(lb::graphics::GraphicsDeviceCapability::VARIABLE_RATE_SHADING_TIER2));

	advancedLightCullingCheckBox.Create("2.5D Light Culling: ");
	advancedLightCullingCheckBox.SetTooltip("Enable a more aggressive light culling approach which can result in slower culling but faster rendering (Tiled renderer only)");
	advancedLightCullingCheckBox.SetPos(XMFLOAT2(x, y += step));
	advancedLightCullingCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	advancedLightCullingCheckBox.OnClick([](lb::gui::EventArgs args) {
		lb::renderer::SetAdvancedLightCulling(args.bValue);
	});
	advancedLightCullingCheckBox.SetCheck(lb::renderer::GetAdvancedLightCulling());
	AddWidget(&advancedLightCullingCheckBox);

	debugLightCullingCheckBox.Create("DEBUG: ");
	debugLightCullingCheckBox.SetTooltip("Toggle visualization of the screen space light culling heatmap grid (Tiled renderer only)");
	debugLightCullingCheckBox.SetPos(XMFLOAT2(x + wid + 1, y));
	debugLightCullingCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	debugLightCullingCheckBox.OnClick([](lb::gui::EventArgs args) {
		lb::renderer::SetDebugLightCulling(args.bValue);
	});
	debugLightCullingCheckBox.SetCheck(lb::renderer::GetDebugLightCulling());
	AddWidget(&debugLightCullingCheckBox);

	tessellationCheckBox.Create("Tessellation: ");
	tessellationCheckBox.SetTooltip("Enable tessellation feature. You also need to specify a tessellation factor for individual objects.");
	tessellationCheckBox.SetPos(XMFLOAT2(x, y += step));
	tessellationCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("tessellation"))
	{
		lb::renderer::SetTessellationEnabled(editor->main->config.GetSection("graphics").GetBool("tessellation"));
	}
	tessellationCheckBox.SetCheck(lb::renderer::GetTessellationEnabled());
	tessellationCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetTessellationEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("tessellation", args.bValue);
		editor->main->config.Commit();
	});
	AddWidget(&tessellationCheckBox);
	tessellationCheckBox.SetEnabled(lb::graphics::GetDevice()->CheckCapability(lb::graphics::GraphicsDeviceCapability::TESSELLATION));

	speedMultiplierSlider.Create(0, 4, 1, 100000, "Speed: ");
	speedMultiplierSlider.SetTooltip("Adjust the global speed (time multiplier)");
	speedMultiplierSlider.SetSize(XMFLOAT2(wid, itemheight));
	speedMultiplierSlider.SetPos(XMFLOAT2(x, y += step));
	speedMultiplierSlider.SetValue(lb::renderer::GetGameSpeed());
	speedMultiplierSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::renderer::SetGameSpeed(args.fValue);
	});
	AddWidget(&speedMultiplierSlider);

	shadowTypeComboBox.Create("Shadow type: ");
	shadowTypeComboBox.SetSize(XMFLOAT2(wid, itemheight));
	shadowTypeComboBox.SetPos(XMFLOAT2(x, y += step));
	shadowTypeComboBox.AddItem("Shadowmaps");
	if (lb::graphics::GetDevice()->CheckCapability(lb::graphics::GraphicsDeviceCapability::RAYTRACING))
	{
		shadowTypeComboBox.AddItem("Ray traced");

		if (editor->main->config.GetSection("graphics").Has("raytraced_shadows"))
		{
			lb::renderer::SetRaytracedShadowsEnabled(editor->main->config.GetSection("graphics").GetBool("raytraced_shadows"));
		}
	}
	shadowTypeComboBox.OnSelect([&](lb::gui::EventArgs args) {

		switch (args.iValue)
		{
		default:
		case 0:
			lb::renderer::SetRaytracedShadowsEnabled(false);
			editor->main->config.GetSection("graphics").Set("raytraced_shadows", false);
			break;
		case 1:
			lb::renderer::SetRaytracedShadowsEnabled(true);
			editor->main->config.GetSection("graphics").Set("raytraced_shadows", true);
			break;
		}

		editor->main->config.Commit();

		});
	shadowTypeComboBox.SetTooltip("Choose between shadowmaps and ray traced shadows.\nNote that ray traced shadows need hardware raytracing support, otherwise they are not available");
	AddWidget(&shadowTypeComboBox);

	shadowProps2DComboBox.Create("2D Shadowmap res: ");
	shadowProps2DComboBox.SetSize(XMFLOAT2(wid, itemheight));
	shadowProps2DComboBox.SetPos(XMFLOAT2(x, y += step));
	shadowProps2DComboBox.AddItem("Off");
	shadowProps2DComboBox.AddItem("128");
	shadowProps2DComboBox.AddItem("256");
	shadowProps2DComboBox.AddItem("512");
	shadowProps2DComboBox.AddItem("1024");
	shadowProps2DComboBox.AddItem("2048");
	shadowProps2DComboBox.AddItem("4096");
	shadowProps2DComboBox.OnSelect([&](lb::gui::EventArgs args) {

		switch (args.iValue)
		{
		case 0:
			lb::renderer::SetShadowProps2D(0);
			break;
		case 1:
			lb::renderer::SetShadowProps2D(128);
			break;
		case 2:
			lb::renderer::SetShadowProps2D(256);
			break;
		case 3:
			lb::renderer::SetShadowProps2D(512);
			break;
		case 4:
			lb::renderer::SetShadowProps2D(1024);
			break;
		case 5:
			lb::renderer::SetShadowProps2D(2048);
			break;
		case 6:
			lb::renderer::SetShadowProps2D(4096);
			break;
		default:
			break;
		}
		editor->main->config.GetSection("graphics").Set("shadow_resolution_2d", args.iValue);
		editor->main->config.Commit();
	});
	if (editor->main->config.GetSection("graphics").Has("shadow_resolution_2d"))
	{
		shadowProps2DComboBox.SetSelected(editor->main->config.GetSection("graphics").GetInt("shadow_resolution_2d"));
	}
	else
	{
		shadowProps2DComboBox.SetSelected(4);
	}
	shadowProps2DComboBox.SetTooltip("Choose a shadow quality preset for 2D shadow maps (spotlights, directional lights)...\nThis specifies the maximum shadow resolution for these light types, but that can dynamically change unless they are set to a fixed resolution individually.");
	shadowProps2DComboBox.SetScriptTip("SetShadowProps2D(int resolution, int count, int softShadowQuality)");
	AddWidget(&shadowProps2DComboBox);

	shadowPropsCubeComboBox.Create("Cube Shadowmap res: ");
	shadowPropsCubeComboBox.SetSize(XMFLOAT2(wid, itemheight));
	shadowPropsCubeComboBox.SetPos(XMFLOAT2(x, y += step));
	shadowPropsCubeComboBox.AddItem("Off");
	shadowPropsCubeComboBox.AddItem("128");
	shadowPropsCubeComboBox.AddItem("256");
	shadowPropsCubeComboBox.AddItem("512");
	shadowPropsCubeComboBox.AddItem("1024");
	shadowPropsCubeComboBox.AddItem("2048");
	shadowPropsCubeComboBox.OnSelect([&](lb::gui::EventArgs args) {
		switch (args.iValue)
		{
		case 0:
			lb::renderer::SetShadowPropsCube(0);
			break;
		case 1:
			lb::renderer::SetShadowPropsCube(128);
			break;
		case 2:
			lb::renderer::SetShadowPropsCube(256);
			break;
		case 3:
			lb::renderer::SetShadowPropsCube(512);
			break;
		case 4:
			lb::renderer::SetShadowPropsCube(1024);
			break;
		case 5:
			lb::renderer::SetShadowPropsCube(2048);
			break;
		default:
			break;
		}
		editor->main->config.GetSection("graphics").Set("shadow_resolution_cube", args.iValue);
		editor->main->config.Commit();
	});
	if (editor->main->config.GetSection("graphics").Has("shadow_resolution_cube"))
	{
		shadowPropsCubeComboBox.SetSelected(editor->main->config.GetSection("graphics").GetInt("shadow_resolution_cube"));
	}
	else
	{
		shadowPropsCubeComboBox.SetSelected(2);
	}
	shadowPropsCubeComboBox.SetTooltip("Choose a shadow quality preset for cube shadow maps (pointlights, area lights)...\nThis specifies the maximum shadow resolution for these light types, but that can dynamically change unless they are set to a fixed resolution individually.");
	shadowPropsCubeComboBox.SetScriptTip("SetShadowPropsCube(int resolution, int count)");
	AddWidget(&shadowPropsCubeComboBox);

	MSAAComboBox.Create("MSAA: ");
	MSAAComboBox.SetSize(XMFLOAT2(wid, itemheight));
	MSAAComboBox.SetPos(XMFLOAT2(x, y += step));
	MSAAComboBox.AddItem("Off", 1);
	MSAAComboBox.AddItem("2", 2);
	MSAAComboBox.AddItem("4", 4);
	MSAAComboBox.AddItem("8", 8);
	if (editor->main->config.GetSection("graphics").Has("msaa"))
	{
		MSAAComboBox.SetSelectedWithoutCallback(editor->main->config.GetSection("graphics").GetInt("msaa"));
	}
	MSAAComboBox.OnSelect([=](lb::gui::EventArgs args) {
		editor->main->config.GetSection("graphics").Set("msaa", args.iValue);
		editor->main->config.Commit();
	});
	MSAAComboBox.SetTooltip("Multisampling Anti Aliasing quality. ");
	AddWidget(&MSAAComboBox);

	temporalAACheckBox.Create("Temporal AA: ");
	temporalAACheckBox.SetTooltip("Toggle Temporal Anti Aliasing. It is a supersampling techique which is performed across multiple frames.");
	temporalAACheckBox.SetPos(XMFLOAT2(x, y += step));
	temporalAACheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	if (editor->main->config.GetSection("graphics").Has("temporal_anti_aliasing"))
	{
		lb::renderer::SetTemporalAAEnabled(editor->main->config.GetSection("graphics").GetBool("temporal_anti_aliasing"));
	}
	temporalAACheckBox.SetCheck(lb::renderer::GetTemporalAAEnabled());
	temporalAACheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetTemporalAAEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("temporal_anti_aliasing", args.bValue);
		editor->main->config.Commit();
	});
	AddWidget(&temporalAACheckBox);

	temporalAADebugCheckBox.Create("DEBUGJitter: ");
	temporalAADebugCheckBox.SetText("DEBUG: ");
	temporalAADebugCheckBox.SetTooltip("Disable blending of frame history. Camera Subpixel jitter will be visible.");
	temporalAADebugCheckBox.SetPos(XMFLOAT2(x + wid + 1, y));
	temporalAADebugCheckBox.SetSize(XMFLOAT2(itemheight, itemheight));
	temporalAADebugCheckBox.OnClick([](lb::gui::EventArgs args) {
		lb::renderer::SetTemporalAADebugEnabled(args.bValue);
	});
	temporalAADebugCheckBox.SetCheck(lb::renderer::GetTemporalAADebugEnabled());
	AddWidget(&temporalAADebugCheckBox);

	textureQualityComboBox.Create("Texture Quality: ");
	textureQualityComboBox.SetSize(XMFLOAT2(wid, itemheight));
	textureQualityComboBox.SetPos(XMFLOAT2(x, y += step));
	textureQualityComboBox.AddItem("Nearest");
	textureQualityComboBox.AddItem("Bilinear");
	textureQualityComboBox.AddItem("Trilinear");
	textureQualityComboBox.AddItem("Anisotropic");
	textureQualityComboBox.OnSelect([&](lb::gui::EventArgs args) {
		lb::graphics::SamplerDesc desc = lb::renderer::GetSampler(lb::enums::SAMPLER_OBJECTSHADER)->GetDesc();

		switch (args.iValue)
		{
		case 0:
			desc.filter = lb::graphics::Filter::MIN_MAG_MIP_POINT;
			break;
		case 1:
			desc.filter = lb::graphics::Filter::MIN_MAG_LINEAR_MIP_POINT;
			break;
		case 2:
			desc.filter = lb::graphics::Filter::MIN_MAG_MIP_LINEAR;
			break;
		case 3:
			desc.filter = lb::graphics::Filter::ANISOTROPIC;
			break;
		default:
			break;
		}

		desc.mip_lod_bias = lb::math::Clamp(mipLodBiasSlider.GetValue(), -15.9f, 15.9f);

		lb::renderer::ModifyObjectSampler(desc);

		editor->main->config.GetSection("graphics").Set("texture_quality", args.iValue);
		editor->main->config.Commit();
	});
	textureQualityComboBox.SetTooltip("Choose a texture sampling method for material textures.");
	AddWidget(&textureQualityComboBox);

	mipLodBiasSlider.Create(-16, 16, 0, 100000, "MipLOD Bias: ");
	mipLodBiasSlider.SetTooltip("Bias the rendered mip map level of the material textures.");
	mipLodBiasSlider.SetSize(XMFLOAT2(wid, itemheight));
	mipLodBiasSlider.SetPos(XMFLOAT2(x, y += step));
	mipLodBiasSlider.SetValue(editor->main->config.GetSection("graphics").GetFloat("mip_lod_bias"));
	mipLodBiasSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::graphics::SamplerDesc desc = lb::renderer::GetSampler(lb::enums::SAMPLER_OBJECTSHADER)->GetDesc();
		desc.mip_lod_bias = lb::math::Clamp(args.fValue, -15.9f, 15.9f);
		lb::renderer::ModifyObjectSampler(desc);
		editor->main->config.GetSection("graphics").Set("mip_lod_bias", args.fValue);
		editor->main->config.Commit();
	});
	AddWidget(&mipLodBiasSlider);

	raytraceBounceCountSlider.Create(1, 10, 1, 9, "Raytrace Bounces: ");
	raytraceBounceCountSlider.SetTooltip("How many light bounces to compute when doing these ray tracing effects:\n- Path tracing\n- Lightmap baking");
	raytraceBounceCountSlider.SetSize(XMFLOAT2(wid, itemheight));
	raytraceBounceCountSlider.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("raytracing_bounce_count"))
	{
		lb::renderer::SetRaytraceBounceCount(editor->main->config.GetSection("graphics").GetInt("raytracing_bounce_count"));
	}
	raytraceBounceCountSlider.SetValue((float)lb::renderer::GetRaytraceBounceCount());
	raytraceBounceCountSlider.OnSlide([&](lb::gui::EventArgs args) {
		lb::renderer::SetRaytraceBounceCount((uint32_t)args.iValue);
		editor->main->config.GetSection("graphics").Set("raytracing_bounces", args.iValue);
		editor->main->config.Commit();
	});
	AddWidget(&raytraceBounceCountSlider);


	// Old Postprocess window params:
	y += step;
	x = 110;
	float hei = itemheight;
	wid = 140;
	float mod_wid = 60;

	hdrcalibrationSlider.Create(0, 8, 1, 100, "HDR calibration: ");
	hdrcalibrationSlider.SetTooltip("Set multiplier for HDR output, this only takes effect when swapchain output format is non-SRGB");
	hdrcalibrationSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setHDRCalibration(args.fValue);
		editor->main->config.GetSection("graphics").Set("hdr_calibration", args.fValue);
		editor->main->config.Commit();
		});
	if (editor->main->config.GetSection("graphics").Has("hdr_calibration"))
	{
		editor->renderPath->setHDRCalibration(editor->main->config.GetSection("graphics").GetFloat("hdr_calibration"));
	}
	AddWidget(&hdrcalibrationSlider);

	tonemapCombo.Create("Tonemap: ");
	tonemapCombo.SetTooltip("Choose tone mapping type");
	tonemapCombo.SetScriptTip("RenderPath3D::SetTonemap(Tonemap value)");
	tonemapCombo.AddItem("Reinhard");
	tonemapCombo.AddItem("ACES");
	tonemapCombo.OnSelect([=](lb::gui::EventArgs args) {
		editor->renderPath->setTonemap((lb::renderer::Tonemap)args.iValue);
		editor->main->config.GetSection("graphics").Set("tonemap", args.iValue);
		editor->main->config.Commit();
		});
	if (editor->main->config.GetSection("graphics").Has("tonemap"))
	{
		int tonemap = editor->main->config.GetSection("graphics").GetInt("tonemap");
		tonemapCombo.SetSelected(tonemap);
	}
	AddWidget(&tonemapCombo);

	exposureSlider.Create(0.0f, 3.0f, 1, 10000, "Tonemap Exposure: ");
	exposureSlider.SetTooltip("Set the tonemap exposure value");
	exposureSlider.SetScriptTip("RenderPath3D::SetExposure(float value)");
	exposureSlider.SetSize(XMFLOAT2(wid, hei));
	exposureSlider.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("tonemap_exposure"))
	{
		editor->renderPath->setExposure(editor->main->config.GetSection("graphics").GetFloat("tonemap_exposure"));
	}
	exposureSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setExposure(args.fValue);
		editor->main->config.GetSection("graphics").Set("tonemap_exposure", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&exposureSlider);

	brightnessSlider.Create(-1.0f, 1.0f, 0, 10000, "Brightness: ");
	brightnessSlider.SetSize(XMFLOAT2(wid, hei));
	brightnessSlider.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("brightness"))
	{
		editor->renderPath->setBrightness(editor->main->config.GetSection("graphics").GetFloat("brightness"));
	}
	brightnessSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setBrightness(args.fValue);
		editor->main->config.GetSection("graphics").Set("brightness", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&brightnessSlider);

	contrastSlider.Create(0.0f, 2.0f, 1, 10000, "Contrast: ");
	contrastSlider.SetSize(XMFLOAT2(wid, hei));
	contrastSlider.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("contrast"))
	{
		editor->renderPath->setContrast(editor->main->config.GetSection("graphics").GetFloat("contrast"));
	}
	contrastSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setContrast(args.fValue);
		editor->main->config.GetSection("graphics").Set("contrast", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&contrastSlider);

	saturationSlider.Create(0.0f, 2.0f, 1, 10000, "Saturation: ");
	saturationSlider.SetSize(XMFLOAT2(wid, hei));
	saturationSlider.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("saturation"))
	{
		editor->renderPath->setSaturation(editor->main->config.GetSection("graphics").GetFloat("saturation"));
	}
	saturationSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setSaturation(args.fValue);
		editor->main->config.GetSection("graphics").Set("saturation", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&saturationSlider);

	lensFlareCheckBox.Create("LensFlare: ");
	lensFlareCheckBox.SetTooltip("Toggle visibility of light source flares. Additional setup needed per light for a lensflare to be visible.");
	lensFlareCheckBox.SetScriptTip("RenderPath3D::SetLensFlareEnabled(bool value)");
	lensFlareCheckBox.SetSize(XMFLOAT2(hei, hei));
	lensFlareCheckBox.SetPos(XMFLOAT2(x, y += step));
	editor->renderPath->setLensFlareEnabled(editor->main->config.GetSection("graphics").GetBool("lensflare"));
	lensFlareCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setLensFlareEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("lensflare", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&lensFlareCheckBox);

	lightShaftsCheckBox.Create("LightShafts: ");
	lightShaftsCheckBox.SetTooltip("Enable light shaft for directional light sources.");
	lightShaftsCheckBox.SetScriptTip("RenderPath3D::SetLightShaftsEnabled(bool value)");
	lightShaftsCheckBox.SetSize(XMFLOAT2(hei, hei));
	lightShaftsCheckBox.SetPos(XMFLOAT2(x, y += step));
	editor->renderPath->setLightShaftsEnabled(editor->main->config.GetSection("graphics").GetBool("lightshafts"));
	lightShaftsCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setLightShaftsEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("lightshafts", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&lightShaftsCheckBox);

	lightShaftsStrengthStrengthSlider.Create(0, 1, 0.05f, 1000, "Lightshaft.Strength: ");
	lightShaftsStrengthStrengthSlider.SetText("Strength: ");
	lightShaftsStrengthStrengthSlider.SetTooltip("Set light shaft strength.");
	lightShaftsStrengthStrengthSlider.SetSize(XMFLOAT2(mod_wid, hei));
	lightShaftsStrengthStrengthSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("lightshafts_strength"))
	{
		editor->renderPath->setLightShaftsStrength(editor->main->config.GetSection("graphics").GetFloat("lightshafts_strength"));
	}
	lightShaftsStrengthStrengthSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setLightShaftsStrength(args.fValue);
		editor->main->config.GetSection("graphics").Set("lightshafts_strength", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&lightShaftsStrengthStrengthSlider);

	aoComboBox.Create("AO: ");
	aoComboBox.SetTooltip("Choose Ambient Occlusion type. RTAO is only available if hardware supports ray tracing");
	aoComboBox.SetScriptTip("RenderPath3D::SetAO(int value)");
	int ao = editor->main->config.GetSection("graphics").GetInt("ambient_occlusion");
	aoComboBox.SetSize(XMFLOAT2(wid, hei));
	aoComboBox.SetPos(XMFLOAT2(x, y += step));
	aoComboBox.AddItem("Disabled");
	aoComboBox.AddItem("SSAO");
	aoComboBox.AddItem("HBAO");
	if (lb::graphics::GetDevice()->CheckCapability(GraphicsDeviceCapability::RAYTRACING))
	{
		aoComboBox.AddItem("RTAO");
		ao = std::min(ao, 4);
	}
	else
	{
		ao = std::min(ao, 3);
	}
	aoComboBox.OnSelect([=](lb::gui::EventArgs args) {
		editor->renderPath->setAO((lb::RenderPath3D::AO)args.iValue);
		editor->main->config.GetSection("graphics").Set("ambient_occlusion", args.iValue);
		editor->main->config.Commit();
		});
	aoComboBox.SetSelected(ao);
	AddWidget(&aoComboBox);

	aoPowerSlider.Create(0.25f, 8.0f, 2, 1000, "Power: ");
	aoPowerSlider.SetTooltip("Set SSAO Power. Higher values produce darker, more pronounced effect");
	aoPowerSlider.SetSize(XMFLOAT2(mod_wid, hei));
	aoPowerSlider.SetPos(XMFLOAT2(x + 100, y += step));
	if (editor->main->config.GetSection("graphics").Has("ambient_occlusion_power"))
	{
		editor->renderPath->setAOPower(editor->main->config.GetSection("graphics").GetFloat("ambient_occlusion_power"));
	}
	aoPowerSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setAOPower(args.fValue);
		editor->main->config.GetSection("graphics").Set("ambient_occlusion_power", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&aoPowerSlider);

	aoRangeSlider.Create(1.0f, 100.0f, 1, 1000, "AO.Range: ");
	aoRangeSlider.SetText("Range: ");
	aoRangeSlider.SetTooltip("Set AO ray length. Only for SSAO and RTAO");
	aoRangeSlider.SetSize(XMFLOAT2(mod_wid, hei));
	aoRangeSlider.SetPos(XMFLOAT2(x + 100, y += step));
	if (editor->main->config.GetSection("graphics").Has("ambient_occlusion_range"))
	{
		editor->renderPath->setAORange(editor->main->config.GetSection("graphics").GetFloat("ambient_occlusion_range"));
	}
	aoRangeSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setAORange(args.fValue);
		editor->main->config.GetSection("graphics").Set("ambient_occlusion_range", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&aoRangeSlider);

	aoSampleCountSlider.Create(1, 16, 9, 15, "Sample Count: ");
	aoSampleCountSlider.SetTooltip("Set AO ray count. Only for SSAO");
	aoSampleCountSlider.SetSize(XMFLOAT2(mod_wid, hei));
	aoSampleCountSlider.SetPos(XMFLOAT2(x + 100, y += step));
	aoSampleCountSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setAOSampleCount(args.iValue);
		});
	AddWidget(&aoSampleCountSlider);

	ssrCheckBox.Create("SSR: ");
	ssrCheckBox.SetTooltip("Enable Screen Space Reflections. This can not reflect anything that is outside of the screen.");
	ssrCheckBox.SetScriptTip("RenderPath3D::SetSSREnabled(bool value)");
	ssrCheckBox.SetSize(XMFLOAT2(hei, hei));
	ssrCheckBox.SetPos(XMFLOAT2(x, y += step));
	editor->renderPath->setSSREnabled(editor->main->config.GetSection("graphics").GetBool("screen_space_reflections"));
	ssrCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setSSREnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("screen_space_reflections", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&ssrCheckBox);

	reflectionsRoughnessCutoffSlider.Create(0, 1, 0.6f, 1000, "Cutoff: ");
	reflectionsRoughnessCutoffSlider.SetTooltip("Set maximum roughness which can be used to apply screen space or raytraced reflections.");
	reflectionsRoughnessCutoffSlider.SetSize(XMFLOAT2(mod_wid, hei));
	reflectionsRoughnessCutoffSlider.SetPos(XMFLOAT2(x + 100, y += step));
	if (editor->main->config.GetSection("graphics").Has("reflection_roughness_cutoff"))
	{
		editor->renderPath->setReflectionRoughnessCutoff(editor->main->config.GetSection("graphics").GetFloat("reflection_roughness_cutoff"));
	}
	reflectionsRoughnessCutoffSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setReflectionRoughnessCutoff(args.fValue);
		editor->main->config.GetSection("graphics").Set("reflection_roughness_cutoff", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&reflectionsRoughnessCutoffSlider);

	raytracedReflectionsCheckBox.Create("RT Reflections: ");
	raytracedReflectionsCheckBox.SetTooltip("Enable Ray Traced Reflections. Only if GPU supports raytracing.");
	raytracedReflectionsCheckBox.SetScriptTip("RenderPath3D::SetRaytracedReflectionsEnabled(bool value)");
	raytracedReflectionsCheckBox.SetSize(XMFLOAT2(hei, hei));
	raytracedReflectionsCheckBox.SetPos(XMFLOAT2(x + 140, y));
	editor->renderPath->setRaytracedReflectionsEnabled(editor->main->config.GetSection("graphics").GetBool("raytraced_reflections"));
	raytracedReflectionsCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setRaytracedReflectionsEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("raytraced_reflections", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&raytracedReflectionsCheckBox);
	raytracedReflectionsCheckBox.SetEnabled(lb::graphics::GetDevice()->CheckCapability(GraphicsDeviceCapability::RAYTRACING));

	raytracedReflectionsRangeSlider.Create(1.0f, 10000.0f, 1, 1000, "RTReflection.Range: ");
	raytracedReflectionsRangeSlider.SetText("Range: ");
	raytracedReflectionsRangeSlider.SetTooltip("Set Reflection ray length for Ray traced reflections.");
	raytracedReflectionsRangeSlider.SetSize(XMFLOAT2(mod_wid, hei));
	raytracedReflectionsRangeSlider.SetPos(XMFLOAT2(x + 100, y += step));
	if (editor->main->config.GetSection("graphics").Has("rtreflection_range"))
	{
		editor->renderPath->setRaytracedReflectionsRange(editor->main->config.GetSection("graphics").GetFloat("rtreflection_range"));
	}
	raytracedReflectionsRangeSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setRaytracedReflectionsRange(args.fValue);
		editor->main->config.GetSection("graphics").Set("rtreflection_range", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&raytracedReflectionsRangeSlider);
	raytracedReflectionsRangeSlider.SetEnabled(lb::graphics::GetDevice()->CheckCapability(GraphicsDeviceCapability::RAYTRACING));

	screenSpaceShadowsCheckBox.Create("Screen Shadows: ");
	screenSpaceShadowsCheckBox.SetTooltip("Enable screen space contact shadows. This can add small shadows details to shadow maps in screen space.");
	screenSpaceShadowsCheckBox.SetSize(XMFLOAT2(hei, hei));
	screenSpaceShadowsCheckBox.SetPos(XMFLOAT2(x, y += step));
	lb::renderer::SetScreenSpaceShadowsEnabled(editor->main->config.GetSection("graphics").GetBool("screen_space_shadows"));
	screenSpaceShadowsCheckBox.OnClick([=](lb::gui::EventArgs args) {
		lb::renderer::SetScreenSpaceShadowsEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("screen_space_shadows", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&screenSpaceShadowsCheckBox);

	screenSpaceShadowsRangeSlider.Create(0.1f, 10.0f, 1, 1000, "ScreenspaceShadows.Range: ");
	screenSpaceShadowsRangeSlider.SetText("Range: ");
	screenSpaceShadowsRangeSlider.SetTooltip("Range of contact shadows");
	screenSpaceShadowsRangeSlider.SetSize(XMFLOAT2(mod_wid, hei));
	screenSpaceShadowsRangeSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("screen_space_shadows_range"))
	{
		editor->renderPath->setScreenSpaceShadowRange(editor->main->config.GetSection("graphics").GetFloat("screen_space_shadows_range"));
	}
	screenSpaceShadowsRangeSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setScreenSpaceShadowRange(args.fValue);
		editor->main->config.GetSection("graphics").Set("screen_space_shadows_range", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&screenSpaceShadowsRangeSlider);

	screenSpaceShadowsStepCountSlider.Create(4, 128, 16, 128 - 4, "Samples: ");
	screenSpaceShadowsStepCountSlider.SetTooltip("Sample count of contact shadows. Higher values are better quality but slower.");
	screenSpaceShadowsStepCountSlider.SetSize(XMFLOAT2(mod_wid, hei));
	screenSpaceShadowsStepCountSlider.SetPos(XMFLOAT2(x + 100, y += step));
	if (editor->main->config.GetSection("graphics").Has("screen_space_shadows_samples"))
	{
		editor->renderPath->setScreenSpaceShadowSampleCount(editor->main->config.GetSection("graphics").GetInt("screen_space_shadows_samples"));
	}
	screenSpaceShadowsStepCountSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setScreenSpaceShadowSampleCount(args.iValue);
		editor->main->config.GetSection("graphics").Set("screen_space_shadows_samples", args.iValue);
		editor->main->config.Commit();
		});
	AddWidget(&screenSpaceShadowsStepCountSlider);

	eyeAdaptionCheckBox.Create("EyeAdaption: ");
	eyeAdaptionCheckBox.SetTooltip("Enable eye adaption for the overall screen luminance");
	eyeAdaptionCheckBox.SetSize(XMFLOAT2(hei, hei));
	eyeAdaptionCheckBox.SetPos(XMFLOAT2(x, y += step));
	editor->renderPath->setEyeAdaptionEnabled(editor->main->config.GetSection("graphics").GetBool("eye_adaption"));
	eyeAdaptionCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setEyeAdaptionEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("eye_adaption", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&eyeAdaptionCheckBox);

	eyeAdaptionKeySlider.Create(0.01f, 0.5f, 0.1f, 10000, "Key: ");
	eyeAdaptionKeySlider.SetTooltip("Set the key value for eye adaption.");
	eyeAdaptionKeySlider.SetSize(XMFLOAT2(mod_wid, hei));
	eyeAdaptionKeySlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("eye_adaption_key"))
	{
		editor->renderPath->setEyeAdaptionKey(editor->main->config.GetSection("graphics").GetFloat("eye_adaption_key"));
	}
	eyeAdaptionKeySlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setEyeAdaptionKey(args.fValue);
		editor->main->config.GetSection("graphics").Set("eye_adaption_key", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&eyeAdaptionKeySlider);

	eyeAdaptionRateSlider.Create(0.01f, 4, 0.5f, 10000, "Rate: ");
	eyeAdaptionRateSlider.SetTooltip("Set the eye adaption rate (speed of adjustment)");
	eyeAdaptionRateSlider.SetSize(XMFLOAT2(mod_wid, hei));
	eyeAdaptionRateSlider.SetPos(XMFLOAT2(x + 100, y += step));
	if (editor->main->config.GetSection("graphics").Has("eye_adaption_rate"))
	{
		editor->renderPath->setEyeAdaptionRate(editor->main->config.GetSection("graphics").GetFloat("eye_adaption_rate"));
	}
	eyeAdaptionRateSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setEyeAdaptionRate(args.fValue);
		editor->main->config.GetSection("graphics").Set("eye_adaption_rate", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&eyeAdaptionRateSlider);

	motionBlurCheckBox.Create("MotionBlur: ");
	motionBlurCheckBox.SetTooltip("Enable motion blur for camera movement and animated meshes.");
	motionBlurCheckBox.SetScriptTip("RenderPath3D::SetMotionBlurEnabled(bool value)");
	motionBlurCheckBox.SetSize(XMFLOAT2(hei, hei));
	motionBlurCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("motion_blur"))
	{
		editor->renderPath->setMotionBlurEnabled(editor->main->config.GetSection("graphics").GetBool("motion_blur"));
	}
	motionBlurCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setMotionBlurEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("motion_blur", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&motionBlurCheckBox);

	motionBlurStrengthSlider.Create(0.1f, 400, 100, 10000, "Motionblur.Strength: ");
	motionBlurStrengthSlider.SetText("Strength");
	motionBlurStrengthSlider.SetTooltip("Set the camera shutter speed for motion blur (higher value means stronger blur).");
	motionBlurStrengthSlider.SetScriptTip("RenderPath3D::SetMotionBlurStrength(float value)");
	motionBlurStrengthSlider.SetSize(XMFLOAT2(mod_wid, hei));
	motionBlurStrengthSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("motion_blur_strength"))
	{
		editor->renderPath->setMotionBlurStrength(editor->main->config.GetSection("graphics").GetFloat("motion_blur_strength"));
	}
	motionBlurStrengthSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setMotionBlurStrength(args.fValue);
		editor->main->config.GetSection("graphics").Set("motion_blur_strength", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&motionBlurStrengthSlider);

	depthOfFieldCheckBox.Create("DepthOfField: ");
	depthOfFieldCheckBox.SetTooltip("Enable Depth of field effect. Requires additional camera setup: focal length and aperture size.");
	depthOfFieldCheckBox.SetScriptTip("RenderPath3D::SetDepthOfFieldEnabled(bool value)");
	depthOfFieldCheckBox.SetSize(XMFLOAT2(hei, hei));
	depthOfFieldCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("depth_of_field"))
	{
		editor->renderPath->setDepthOfFieldEnabled(editor->main->config.GetSection("graphics").GetBool("depth_of_field"));
	}
	depthOfFieldCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setDepthOfFieldEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("depth_of_field", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&depthOfFieldCheckBox);

	depthOfFieldScaleSlider.Create(1.0f, 20, 100, 1000, "DepthOfField.Strength: ");
	depthOfFieldScaleSlider.SetText("Strength: ");
	depthOfFieldScaleSlider.SetTooltip("Set depth of field strength. This is used to scale the Camera's ApertureSize setting");
	depthOfFieldScaleSlider.SetScriptTip("RenderPath3D::SetDepthOfFieldStrength(float value)");
	depthOfFieldScaleSlider.SetSize(XMFLOAT2(mod_wid, hei));
	depthOfFieldScaleSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("depth_of_field_strength"))
	{
		editor->renderPath->setDepthOfFieldStrength(editor->main->config.GetSection("graphics").GetFloat("depth_of_field_strength"));
	}
	depthOfFieldScaleSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setDepthOfFieldStrength(args.fValue);
		editor->main->config.GetSection("graphics").Set("depth_of_field_strength", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&depthOfFieldScaleSlider);

	bloomCheckBox.Create("Bloom: ");
	bloomCheckBox.SetTooltip("Enable bloom. The effect adds color bleeding to the brightest parts of the scene.");
	bloomCheckBox.SetScriptTip("RenderPath3D::SetBloomEnabled(bool value)");
	bloomCheckBox.SetSize(XMFLOAT2(hei, hei));
	bloomCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("bloom"))
	{
		editor->renderPath->setBloomEnabled(editor->main->config.GetSection("graphics").GetBool("bloom"));
	}
	bloomCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setBloomEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("bloom", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&bloomCheckBox);

	bloomStrengthSlider.Create(0.0f, 10, 1, 1000, "Bloom.Threshold: ");
	bloomStrengthSlider.SetText("Threshold: ");
	bloomStrengthSlider.SetTooltip("Set bloom threshold. The values below this will not glow on the screen.");
	bloomStrengthSlider.SetSize(XMFLOAT2(mod_wid, hei));
	bloomStrengthSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("bloom_threshold"))
	{
		editor->renderPath->setBloomThreshold(editor->main->config.GetSection("graphics").GetFloat("bloom_threshold"));
	}
	bloomStrengthSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setBloomThreshold(args.fValue);
		editor->main->config.GetSection("graphics").Set("bloom_threshold", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&bloomStrengthSlider);

	fxaaCheckBox.Create("FXAA: ");
	fxaaCheckBox.SetTooltip("Fast Approximate Anti Aliasing. A fast antialiasing method, but can be a bit too blurry.");
	fxaaCheckBox.SetScriptTip("RenderPath3D::SetFXAAEnabled(bool value)");
	fxaaCheckBox.SetSize(XMFLOAT2(hei, hei));
	fxaaCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("fxaa"))
	{
		editor->renderPath->setFXAAEnabled(editor->main->config.GetSection("graphics").GetBool("fxaa"));
	}
	fxaaCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setFXAAEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("fxaa", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&fxaaCheckBox);

	colorGradingCheckBox.Create("Color Grading: ");
	colorGradingCheckBox.SetTooltip("Enable color grading of the final render. An additional lookup texture must be set in the Weather!");
	colorGradingCheckBox.SetSize(XMFLOAT2(hei, hei));
	colorGradingCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("color_grading"))
	{
		editor->renderPath->setColorGradingEnabled(editor->main->config.GetSection("graphics").GetBool("color_grading"));
	}
	colorGradingCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setColorGradingEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("color_grading", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&colorGradingCheckBox);

	ditherCheckBox.Create("Dithering: ");
	ditherCheckBox.SetTooltip("Toggle the full screen dithering effect. This helps to reduce color banding.");
	ditherCheckBox.SetSize(XMFLOAT2(hei, hei));
	ditherCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("dithering"))
	{
		editor->renderPath->setDitherEnabled(editor->main->config.GetSection("graphics").GetBool("dithering"));
	}
	ditherCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setDitherEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("dithering", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&ditherCheckBox);

	sharpenFilterCheckBox.Create("Sharpen Filter: ");
	sharpenFilterCheckBox.SetTooltip("Toggle sharpening post process of the final image.");
	sharpenFilterCheckBox.SetScriptTip("RenderPath3D::SetSharpenFilterEnabled(bool value)");
	sharpenFilterCheckBox.SetSize(XMFLOAT2(hei, hei));
	sharpenFilterCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("sharpen"))
	{
		editor->renderPath->setSharpenFilterEnabled(editor->main->config.GetSection("graphics").GetBool("sharpen"));
	}
	sharpenFilterCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setSharpenFilterEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("sharpen", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&sharpenFilterCheckBox);

	sharpenFilterAmountSlider.Create(0, 4, 1, 1000, "Sharpen.Strength: ");
	sharpenFilterAmountSlider.SetText("Strength: ");
	sharpenFilterAmountSlider.SetTooltip("Set sharpness filter strength.");
	sharpenFilterAmountSlider.SetScriptTip("RenderPath3D::SetSharpenFilterAmount(float value)");
	sharpenFilterAmountSlider.SetSize(XMFLOAT2(mod_wid, hei));
	sharpenFilterAmountSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("sharpen_strength"))
	{
		editor->renderPath->setSharpenFilterAmount(editor->main->config.GetSection("graphics").GetFloat("sharpen_strength"));
	}
	sharpenFilterAmountSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setSharpenFilterAmount(args.fValue);
		editor->main->config.GetSection("graphics").Set("sharpen_strength", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&sharpenFilterAmountSlider);

	outlineCheckBox.Create("Cartoon Outline: ");
	outlineCheckBox.SetTooltip("Toggle the cartoon outline effect. Only those materials will be outlined that have Cartoon Outline enabled.");
	outlineCheckBox.SetSize(XMFLOAT2(hei, hei));
	outlineCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("cartoon_outline"))
	{
		editor->renderPath->setOutlineEnabled(editor->main->config.GetSection("graphics").GetBool("cartoon_outline"));
	}
	outlineCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setOutlineEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("cartoon_outline", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&outlineCheckBox);

	outlineThresholdSlider.Create(0, 1, 0.1f, 1000, "Outline.Threshold: ");
	outlineThresholdSlider.SetText("Threshold: ");
	outlineThresholdSlider.SetTooltip("Outline edge detection threshold. Increase if not enough otlines are detected, decrease if too many outlines are detected.");
	outlineThresholdSlider.SetSize(XMFLOAT2(mod_wid, hei));
	outlineThresholdSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("cartoon_outline_threshold"))
	{
		editor->renderPath->setOutlineThreshold(editor->main->config.GetSection("graphics").GetFloat("cartoon_outline_threshold"));
	}
	outlineThresholdSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setOutlineThreshold(args.fValue);
		editor->main->config.GetSection("graphics").Set("cartoon_outline_threshold", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&outlineThresholdSlider);

	outlineThicknessSlider.Create(0, 4, 1, 1000, "Thickness: ");
	outlineThicknessSlider.SetTooltip("Set outline thickness.");
	outlineThicknessSlider.SetSize(XMFLOAT2(mod_wid, hei));
	outlineThicknessSlider.SetPos(XMFLOAT2(x + 100, y += step));
	if (editor->main->config.GetSection("graphics").Has("cartoon_outline_thickness"))
	{
		editor->renderPath->setOutlineThickness(editor->main->config.GetSection("graphics").GetFloat("cartoon_outline_thickness"));
	}
	outlineThicknessSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setOutlineThickness(args.fValue);
		editor->main->config.GetSection("graphics").Set("cartoon_outline_thickness", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&outlineThicknessSlider);

	chromaticaberrationCheckBox.Create("Chromatic Aberration: ");
	chromaticaberrationCheckBox.SetTooltip("Toggle the full screen chromatic aberration effect. This simulates lens distortion at screen edges.");
	chromaticaberrationCheckBox.SetSize(XMFLOAT2(hei, hei));
	chromaticaberrationCheckBox.SetPos(XMFLOAT2(x, y += step));
	if (editor->main->config.GetSection("graphics").Has("chromatic_aberration"))
	{
		editor->renderPath->setChromaticAberrationEnabled(editor->main->config.GetSection("graphics").GetBool("chromatic_aberration"));
	}
	chromaticaberrationCheckBox.OnClick([=](lb::gui::EventArgs args) {
		editor->renderPath->setChromaticAberrationEnabled(args.bValue);
		editor->main->config.GetSection("graphics").Set("chromatic_aberration", args.bValue);
		editor->main->config.Commit();
		});
	AddWidget(&chromaticaberrationCheckBox);

	chromaticaberrationSlider.Create(0, 40, 1.0f, 1000, "Strength: ");
	chromaticaberrationSlider.SetTooltip("The lens distortion amount.");
	chromaticaberrationSlider.SetSize(XMFLOAT2(mod_wid, hei));
	chromaticaberrationSlider.SetPos(XMFLOAT2(x + 100, y));
	if (editor->main->config.GetSection("graphics").Has("chromatic_aberration_strength"))
	{
		editor->renderPath->setChromaticAberrationAmount(editor->main->config.GetSection("graphics").GetFloat("chromatic_aberration_strength"));
	}
	chromaticaberrationSlider.OnSlide([=](lb::gui::EventArgs args) {
		editor->renderPath->setChromaticAberrationAmount(args.fValue);
		editor->main->config.GetSection("graphics").Set("chromatic_aberration_strength", args.fValue);
		editor->main->config.Commit();
		});
	AddWidget(&chromaticaberrationSlider);

	SetVisible(false);
}

void GraphicsWindow::UpdateSwapChainFormats(lb::graphics::SwapChain* swapChain)
{
	swapchainComboBox.OnSelect(nullptr);
	swapchainComboBox.ClearItems();
	swapchainComboBox.AddItem("SDR 8bit", static_cast<uint64_t>(lb::graphics::Format::R8G8B8A8_UNORM));
	swapchainComboBox.AddItem("SDR 10bit", static_cast<uint64_t>(lb::graphics::Format::R10G10B10A2_UNORM));
	if (lb::graphics::GetDevice()->IsSwapChainSupportsHDR(swapChain))
	{
		swapchainComboBox.AddItem("HDR 10bit", static_cast<uint64_t>(lb::graphics::Format::R10G10B10A2_UNORM));
		swapchainComboBox.AddItem("HDR 16bit", static_cast<uint64_t>(lb::graphics::Format::R16G16B16A16_FLOAT));

		switch (swapChain->desc.format)
		{
		default:
		case lb::graphics::Format::R8G8B8A8_UNORM:
			swapchainComboBox.SetSelected(0);
			break;
		case lb::graphics::Format::R10G10B10A2_UNORM:
			if (swapChain->desc.allow_hdr)
			{
				swapchainComboBox.SetSelected(2);
			}
			else
			{
				swapchainComboBox.SetSelected(1);
			}
			break;
		case lb::graphics::Format::R16G16B16A16_FLOAT:
			swapchainComboBox.SetSelected(4);
			break;
		}
	}
	else
	{
		switch (swapChain->desc.format)
		{
		default:
		case lb::graphics::Format::R8G8B8A8_UNORM:
			swapchainComboBox.SetSelected(0);
			break;
		case lb::graphics::Format::R10G10B10A2_UNORM:
			swapchainComboBox.SetSelected(1);
			break;
		case lb::graphics::Format::R16G16B16A16_FLOAT:
			swapchainComboBox.SetSelected(1);
			break;
		}
	}

	swapchainComboBox.OnSelect([=](lb::gui::EventArgs args) {

		swapChain->desc.format = (lb::graphics::Format)args.userdata;
		switch (args.iValue)
		{
		default:
		case 0:
		case 1:
			editor->main->allow_hdr = false;
			break;
		case 2:
		case 3:
			editor->main->allow_hdr = true;
			break;
		}
		swapChain->desc.allow_hdr = editor->main->allow_hdr;

		bool success = lb::graphics::GetDevice()->CreateSwapChain(&swapChain->desc, nullptr, swapChain);
		assert(success);
		});
}

void GraphicsWindow::Update()
{
	if (vsyncCheckBox.GetCheck() != editor->main->swapChain.desc.vsync)
	{
		lb::eventhandler::SetVSync(vsyncCheckBox.GetCheck());
	}
	if (editor->resolutionScale != editor->renderPath->resolutionScale)
	{
		editor->renderPath->resolutionScale = editor->resolutionScale;
		editor->ResizeBuffers();
	}
	if (MSAAComboBox.GetItemUserData(MSAAComboBox.GetSelected()) != editor->renderPath->getMSAASampleCount())
	{
		editor->renderPath->setMSAASampleCount((uint32_t)MSAAComboBox.GetItemUserData(MSAAComboBox.GetSelected()));
		editor->ResizeBuffers();
	}

	if (IsCollapsed())
		return;

	const lb::scene::Scene& scene = editor->GetCurrentScene();

	ddgiX.SetValue(std::to_string(scene.ddgi.grid_dimensions.x));
	ddgiY.SetValue(std::to_string(scene.ddgi.grid_dimensions.y));
	ddgiZ.SetValue(std::to_string(scene.ddgi.grid_dimensions.z));

	hdrcalibrationSlider.SetValue(editor->renderPath->getHDRCalibration());
	occlusionCullingCheckBox.SetCheck(lb::renderer::GetOcclusionCullingEnabled());
	GIBoostSlider.SetValue(lb::renderer::GetGIBoost());
	visibilityComputeShadingCheckBox.SetCheck(editor->renderPath->getVisibilityComputeShadingEnabled());
	meshShaderCheckBox.SetCheck(lb::renderer::IsMeshShaderAllowed());
	meshletOcclusionCullingCheckBox.SetCheck(lb::renderer::IsMeshletOcclusionCullingEnabled());
	resolutionScaleSlider.SetValue(editor->resolutionScale);
	streamingSlider.SetValue(lb::resourcemanager::GetStreamingMemoryThreshold());
	MSAAComboBox.SetSelectedByUserdataWithoutCallback(editor->renderPath->getMSAASampleCount());
	tonemapCombo.SetSelectedByUserdataWithoutCallback((int)editor->renderPath->getTonemap());
	exposureSlider.SetValue(editor->renderPath->getExposure());
	brightnessSlider.SetValue(editor->renderPath->getBrightness());
	contrastSlider.SetValue(editor->renderPath->getContrast());
	saturationSlider.SetValue(editor->renderPath->getSaturation());
	lensFlareCheckBox.SetCheck(editor->renderPath->getLensFlareEnabled());
	lightShaftsCheckBox.SetCheck(editor->renderPath->getLightShaftsEnabled());
	lightShaftsStrengthStrengthSlider.SetValue(editor->renderPath->getLightShaftsStrength());
	aoComboBox.SetSelectedWithoutCallback(editor->renderPath->getAO());
	aoPowerSlider.SetValue((float)editor->renderPath->getAOPower());

	switch (editor->renderPath->getAO())
	{
	case lb::RenderPath3D::AO_SSAO:
		aoRangeSlider.SetEnabled(true);
		aoSampleCountSlider.SetEnabled(true);
		break;
	case lb::RenderPath3D::AO_RTAO:
		aoRangeSlider.SetEnabled(true);
		aoSampleCountSlider.SetEnabled(false);
		break;
	default:
		aoRangeSlider.SetEnabled(false);
		aoSampleCountSlider.SetEnabled(false);
		break;
	}

	aoRangeSlider.SetValue((float)editor->renderPath->getAORange());
	aoSampleCountSlider.SetValue((float)editor->renderPath->getAOSampleCount());
	ssrCheckBox.SetCheck(editor->renderPath->getSSREnabled());
	reflectionsRoughnessCutoffSlider.SetValue(editor->renderPath->getReflectionRoughnessCutoff());
	raytracedReflectionsCheckBox.SetCheck(editor->renderPath->getRaytracedReflectionEnabled());
	raytracedReflectionsRangeSlider.SetValue(editor->renderPath->getRaytracedReflectionsRange());
	screenSpaceShadowsCheckBox.SetCheck(lb::renderer::GetScreenSpaceShadowsEnabled());
	screenSpaceShadowsRangeSlider.SetValue((float)editor->renderPath->getScreenSpaceShadowRange());
	screenSpaceShadowsStepCountSlider.SetValue((float)editor->renderPath->getScreenSpaceShadowSampleCount());
	eyeAdaptionCheckBox.SetCheck(editor->renderPath->getEyeAdaptionEnabled());
	eyeAdaptionKeySlider.SetValue(editor->renderPath->getEyeAdaptionKey());
	eyeAdaptionRateSlider.SetValue(editor->renderPath->getEyeAdaptionRate());
	motionBlurCheckBox.SetCheck(editor->renderPath->getMotionBlurEnabled());
	motionBlurStrengthSlider.SetValue(editor->renderPath->getMotionBlurStrength());
	depthOfFieldCheckBox.SetCheck(editor->renderPath->getDepthOfFieldEnabled());
	depthOfFieldScaleSlider.SetValue(editor->renderPath->getDepthOfFieldStrength());
	bloomCheckBox.SetCheck(editor->renderPath->getBloomEnabled());
	bloomStrengthSlider.SetValue(editor->renderPath->getBloomThreshold());
	fxaaCheckBox.SetCheck(editor->renderPath->getFXAAEnabled());
	colorGradingCheckBox.SetCheck(editor->renderPath->getColorGradingEnabled());
	ditherCheckBox.SetCheck(editor->renderPath->getDitherEnabled());
	sharpenFilterCheckBox.SetCheck(editor->renderPath->getSharpenFilterEnabled());
	sharpenFilterAmountSlider.SetValue(editor->renderPath->getSharpenFilterAmount());
	outlineCheckBox.SetCheck(editor->renderPath->getOutlineEnabled());
	outlineThresholdSlider.SetValue(editor->renderPath->getOutlineThreshold());
	outlineThicknessSlider.SetValue(editor->renderPath->getOutlineThickness());
	chromaticaberrationCheckBox.SetCheck(editor->renderPath->getChromaticAberrationEnabled());
	chromaticaberrationSlider.SetValue(editor->renderPath->getChromaticAberrationAmount());
	shadowTypeComboBox.SetSelectedWithoutCallback(lb::renderer::GetRaytracedShadowsEnabled() ? 1 : 0);
}

void GraphicsWindow::ChangeRenderPath(RENDERPATH path)
{
	switch (path)
	{
	case RENDERPATH_DEFAULT:
		editor->renderPath = std::make_unique<lb::RenderPath3D>();
		break;
	case RENDERPATH_PATHTRACING:
		editor->renderPath = std::make_unique<lb::RenderPath3D_PathTracing>();
		break;
	default:
		assert(0);
		break;
	}

	if (editor->scenes.empty())
	{
		editor->NewScene();
	}
	else
	{
		editor->SetCurrentScene(editor->current_scene);
	}

	editor->renderPath->resolutionScale = editor->resolutionScale;
	editor->renderPath->setBloomThreshold(3.0f);

	editor->renderPath->Load();
	editor->ResizeBuffers();
}

void GraphicsWindow::ResizeLayout()
{
	lb::gui::Window::ResizeLayout();
	const float padding = 4;
	const float width = GetWidgetAreaSize().x;
	float y = padding;
	float jump = 20;

	auto add = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_left = 155;
		const float margin_right = 50;
		widget.SetPos(XMFLOAT2(margin_left, y));
		widget.SetSize(XMFLOAT2(width - margin_left - margin_right, widget.GetScale().y));
		y += widget.GetSize().y;
		y += padding;
	};
	auto add_right = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_right = 50;
		widget.SetPos(XMFLOAT2(width - margin_right - widget.GetSize().x, y));
		y += widget.GetSize().y;
		y += padding;
	};
	auto add_fullwidth = [&](lb::gui::Widget& widget) {
		if (!widget.IsVisible())
			return;
		const float margin_left = padding;
		const float margin_right = padding;
		widget.SetPos(XMFLOAT2(margin_left, y));
		widget.SetSize(XMFLOAT2(width - margin_left - margin_right, widget.GetScale().y));
		y += widget.GetSize().y;
		y += padding;
	};

	RENDERPATH renderpath = (RENDERPATH)renderPathComboBox.GetItemUserData(renderPathComboBox.GetSelected());

	add_right(vsyncCheckBox);
	add(swapchainComboBox);
	add(hdrcalibrationSlider);
	add(renderPathComboBox);
	add(resolutionScaleSlider);
	add(streamingSlider);
	add(speedMultiplierSlider);
	add(textureQualityComboBox);
	add(mipLodBiasSlider);

	if (renderpath == RENDERPATH_PATHTRACING)
	{
		shadowTypeComboBox.SetVisible(false);
		shadowProps2DComboBox.SetVisible(false);
		shadowPropsCubeComboBox.SetVisible(false);
		MSAAComboBox.SetVisible(false);
		temporalAADebugCheckBox.SetVisible(false);
		temporalAACheckBox.SetVisible(false);
		variableRateShadingClassificationDebugCheckBox.SetVisible(false);
		variableRateShadingClassificationCheckBox.SetVisible(false);
		debugLightCullingCheckBox.SetVisible(false);
		advancedLightCullingCheckBox.SetVisible(false);
		occlusionCullingCheckBox.SetVisible(false);
		visibilityComputeShadingCheckBox.SetVisible(false);
		meshShaderCheckBox.SetVisible(false);
		meshletOcclusionCullingCheckBox.SetVisible(false);
		tessellationCheckBox.SetVisible(false);
	}
	else
	{
		shadowTypeComboBox.SetVisible(true);
		shadowProps2DComboBox.SetVisible(true);
		shadowPropsCubeComboBox.SetVisible(true);
		MSAAComboBox.SetVisible(true);
		temporalAADebugCheckBox.SetVisible(true);
		temporalAACheckBox.SetVisible(true);
		variableRateShadingClassificationDebugCheckBox.SetVisible(true);
		variableRateShadingClassificationCheckBox.SetVisible(true);
		debugLightCullingCheckBox.SetVisible(true);
		advancedLightCullingCheckBox.SetVisible(true);
		occlusionCullingCheckBox.SetVisible(true);
		visibilityComputeShadingCheckBox.SetVisible(true);
		meshShaderCheckBox.SetVisible(true);
		meshletOcclusionCullingCheckBox.SetVisible(true);
		tessellationCheckBox.SetVisible(true);

		add(shadowTypeComboBox);
		add(shadowProps2DComboBox);
		add(shadowPropsCubeComboBox);
		add(MSAAComboBox);
		add_right(temporalAADebugCheckBox);
		temporalAACheckBox.SetPos(XMFLOAT2(temporalAADebugCheckBox.GetPos().x - temporalAACheckBox.GetSize().x - 70, temporalAADebugCheckBox.GetPos().y));
		add_right(variableRateShadingClassificationDebugCheckBox);
		variableRateShadingClassificationCheckBox.SetPos(XMFLOAT2(variableRateShadingClassificationDebugCheckBox.GetPos().x - variableRateShadingClassificationCheckBox.GetSize().x - 70, variableRateShadingClassificationDebugCheckBox.GetPos().y));
		add_right(debugLightCullingCheckBox);
		advancedLightCullingCheckBox.SetPos(XMFLOAT2(debugLightCullingCheckBox.GetPos().x - advancedLightCullingCheckBox.GetSize().x - 70, debugLightCullingCheckBox.GetPos().y));
		add_right(occlusionCullingCheckBox);
		add_right(visibilityComputeShadingCheckBox);
		add_right(meshShaderCheckBox);
		add_right(meshletOcclusionCullingCheckBox);
		add_right(tessellationCheckBox);
	}

	y += jump;

	add(raytraceBounceCountSlider);

	if (renderpath == RENDERPATH_PATHTRACING)
	{
		pathTraceTargetSlider.SetVisible(true);
		pathTraceStatisticsLabel.SetVisible(true);
		add(pathTraceTargetSlider);
		add_fullwidth(pathTraceStatisticsLabel);

		GIBoostSlider.SetVisible(false);
		ddgiDebugCheckBox.SetVisible(false);
		ddgiCheckBox.SetVisible(false);
		ddgiZ.SetVisible(false);
		ddgiY.SetVisible(false);
		ddgiX.SetVisible(false);
		ddgiRayCountSlider.SetVisible(false);
		ddgiBlendSpeedSlider.SetVisible(false);
		ddgiSmoothBackfaceSlider.SetVisible(false);
		vxgiDebugCombo.SetVisible(false);
		vxgiCheckBox.SetVisible(false);
		vxgiReflectionsCheckBox.SetVisible(false);
		vxgiVoxelSizeSlider.SetVisible(false);
		vxgiRayStepSizeSlider.SetVisible(false);
		vxgiMaxDistanceSlider.SetVisible(false);
	}
	else
	{
		pathTraceTargetSlider.SetVisible(false);
		pathTraceStatisticsLabel.SetVisible(false);


		GIBoostSlider.SetVisible(true);
		ddgiDebugCheckBox.SetVisible(true);
		ddgiCheckBox.SetVisible(true);
		ddgiZ.SetVisible(true);
		ddgiY.SetVisible(true);
		ddgiX.SetVisible(true);
		ddgiRayCountSlider.SetVisible(true);
		ddgiBlendSpeedSlider.SetVisible(true);
		ddgiSmoothBackfaceSlider.SetVisible(true);
		ddgiSmoothBackfaceSlider.SetValue(editor->GetCurrentScene().ddgi.smooth_backface);
		vxgiDebugCombo.SetVisible(true);
		vxgiCheckBox.SetVisible(true);
		vxgiReflectionsCheckBox.SetVisible(true);
		vxgiVoxelSizeSlider.SetVisible(true);
		vxgiVoxelSizeSlider.SetValue(editor->GetCurrentScene().vxgi.clipmaps[0].voxelsize);
		vxgiRayStepSizeSlider.SetVisible(true);
		vxgiRayStepSizeSlider.SetValue(editor->GetCurrentScene().vxgi.rayStepSize);
		vxgiMaxDistanceSlider.SetVisible(true);
		vxgiMaxDistanceSlider.SetValue(editor->GetCurrentScene().vxgi.maxDistance);

		add(GIBoostSlider);

		y += jump;

		add_right(ddgiCheckBox);
		add_right(ddgiDebugCheckBox);
		add_right(ddgiZ);
		ddgiY.SetPos(XMFLOAT2(ddgiZ.GetPos().x - ddgiY.GetSize().x - padding, ddgiZ.GetPos().y));
		ddgiX.SetPos(XMFLOAT2(ddgiY.GetPos().x - ddgiX.GetSize().x - padding, ddgiY.GetPos().y));
		add(ddgiRayCountSlider);
		add(ddgiBlendSpeedSlider);
		add(ddgiSmoothBackfaceSlider);

		y += jump;

		add_right(vxgiDebugCombo);
		vxgiCheckBox.SetPos(XMFLOAT2(vxgiDebugCombo.GetPos().x - vxgiCheckBox.GetSize().x - padding, vxgiDebugCombo.GetPos().y));
		add_right(vxgiReflectionsCheckBox);
		add(vxgiVoxelSizeSlider);
		add(vxgiRayStepSizeSlider);
		add(vxgiMaxDistanceSlider);
	}


	y += jump;

	add(tonemapCombo);
	add(exposureSlider);
	add(brightnessSlider);
	add(contrastSlider);
	add(saturationSlider);
	add_right(lensFlareCheckBox);
	add_right(lightShaftsStrengthStrengthSlider);
	lightShaftsCheckBox.SetPos(XMFLOAT2(lightShaftsStrengthStrengthSlider.GetPos().x - lightShaftsCheckBox.GetSize().x - 80, lightShaftsStrengthStrengthSlider.GetPos().y));
	add(aoComboBox);
	add(aoPowerSlider);
	add(aoRangeSlider);
	add(aoSampleCountSlider);
	add_right(reflectionsRoughnessCutoffSlider);
	ssrCheckBox.SetPos(XMFLOAT2(reflectionsRoughnessCutoffSlider.GetPos().x - ssrCheckBox.GetSize().x - 80, reflectionsRoughnessCutoffSlider.GetPos().y));
	add_right(raytracedReflectionsRangeSlider);
	raytracedReflectionsCheckBox.SetPos(XMFLOAT2(raytracedReflectionsRangeSlider.GetPos().x - raytracedReflectionsCheckBox.GetSize().x - 80, raytracedReflectionsRangeSlider.GetPos().y));
	add_right(screenSpaceShadowsStepCountSlider);
	screenSpaceShadowsCheckBox.SetPos(XMFLOAT2(screenSpaceShadowsStepCountSlider.GetPos().x - screenSpaceShadowsCheckBox.GetSize().x - 80, screenSpaceShadowsStepCountSlider.GetPos().y));
	add_right(screenSpaceShadowsRangeSlider);
	add_right(eyeAdaptionKeySlider);
	eyeAdaptionCheckBox.SetPos(XMFLOAT2(eyeAdaptionKeySlider.GetPos().x - eyeAdaptionCheckBox.GetSize().x - 80, eyeAdaptionKeySlider.GetPos().y));
	add_right(eyeAdaptionRateSlider);
	add_right(motionBlurStrengthSlider);
	motionBlurCheckBox.SetPos(XMFLOAT2(motionBlurStrengthSlider.GetPos().x - motionBlurCheckBox.GetSize().x - 80, motionBlurStrengthSlider.GetPos().y));
	add_right(depthOfFieldScaleSlider);
	depthOfFieldCheckBox.SetPos(XMFLOAT2(depthOfFieldScaleSlider.GetPos().x - depthOfFieldCheckBox.GetSize().x - 80, depthOfFieldScaleSlider.GetPos().y));
	add_right(bloomStrengthSlider);
	bloomCheckBox.SetPos(XMFLOAT2(bloomStrengthSlider.GetPos().x - bloomCheckBox.GetSize().x - 80, bloomStrengthSlider.GetPos().y));
	add_right(fxaaCheckBox);
	add_right(colorGradingCheckBox);
	add_right(ditherCheckBox);
	add_right(sharpenFilterAmountSlider);
	sharpenFilterCheckBox.SetPos(XMFLOAT2(sharpenFilterAmountSlider.GetPos().x - sharpenFilterCheckBox.GetSize().x - 80, sharpenFilterAmountSlider.GetPos().y));
	add_right(outlineThresholdSlider);
	outlineCheckBox.SetPos(XMFLOAT2(outlineThresholdSlider.GetPos().x - outlineCheckBox.GetSize().x - 80, outlineThresholdSlider.GetPos().y));
	add_right(outlineThicknessSlider);
	add_right(chromaticaberrationSlider);
	chromaticaberrationCheckBox.SetPos(XMFLOAT2(chromaticaberrationSlider.GetPos().x - chromaticaberrationCheckBox.GetSize().x - 80, chromaticaberrationSlider.GetPos().y));


}

void GraphicsWindow::ApplySamplerSettings()
{
	if (editor->main->config.GetSection("graphics").Has("texture_quality"))
	{
		textureQualityComboBox.SetSelected(editor->main->config.GetSection("graphics").GetInt("texture_quality"));
	}
	else
	{
		textureQualityComboBox.SetSelected(3);
	}
}
