#include "LibertasEngine.h"

#include <iostream>
#include <iomanip>
#include <mutex>
#include <string>
#include <cstdlib>

std::mutex locker;
struct ShaderEntry
{
	std::string name;
	lb::graphics::ShaderStage stage = lb::graphics::ShaderStage::Count;
	lb::graphics::ShaderModel minshadermodel = lb::graphics::ShaderModel::SM_5_0;
	struct Permutation
	{
		lb::vector<std::string> defines;
	};
	lb::vector<Permutation> permutations;
};
lb::vector<ShaderEntry> shaders = {
	{"hairparticle_simulateCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_simulateCS", lb::graphics::ShaderStage::CS},
	{"generateMIPChainCubeCS_float4", lb::graphics::ShaderStage::CS},
	{"generateMIPChainCubeCS_unorm4", lb::graphics::ShaderStage::CS},
	{"generateMIPChainCubeArrayCS_float4", lb::graphics::ShaderStage::CS},
	{"generateMIPChainCubeArrayCS_unorm4", lb::graphics::ShaderStage::CS},
	{"generateMIPChain3DCS_float4", lb::graphics::ShaderStage::CS},
	{"generateMIPChain3DCS_unorm4", lb::graphics::ShaderStage::CS},
	{"generateMIPChain2DCS_float4", lb::graphics::ShaderStage::CS},
	{"generateMIPChain2DCS_unorm4", lb::graphics::ShaderStage::CS},
	{"blockcompressCS_BC1", lb::graphics::ShaderStage::CS},
	{"blockcompressCS_BC3", lb::graphics::ShaderStage::CS},
	{"blockcompressCS_BC4", lb::graphics::ShaderStage::CS},
	{"blockcompressCS_BC5", lb::graphics::ShaderStage::CS},
	{"blockcompressCS_BC6H", lb::graphics::ShaderStage::CS},
	{"blockcompressCS_BC6H_cubemap", lb::graphics::ShaderStage::CS},
	{"blur_gaussian_float4CS", lb::graphics::ShaderStage::CS},
	{"bloomseparateCS", lb::graphics::ShaderStage::CS},
	{"depthoffield_mainCS", lb::graphics::ShaderStage::CS},
	{"depthoffield_neighborhoodMaxCOCCS", lb::graphics::ShaderStage::CS},
	{"depthoffield_prepassCS", lb::graphics::ShaderStage::CS},
	{"depthoffield_upsampleCS", lb::graphics::ShaderStage::CS},
	{"depthoffield_tileMaxCOC_verticalCS", lb::graphics::ShaderStage::CS},
	{"depthoffield_tileMaxCOC_horizontalCS", lb::graphics::ShaderStage::CS},
	{"vxgi_offsetprevCS", lb::graphics::ShaderStage::CS},
	{"vxgi_temporalCS", lb::graphics::ShaderStage::CS},
	{"vxgi_sdf_jumpfloodCS", lb::graphics::ShaderStage::CS},
	{"vxgi_resolve_diffuseCS", lb::graphics::ShaderStage::CS},
	{"vxgi_resolve_specularCS", lb::graphics::ShaderStage::CS},
	{"upsample_bilateral_float1CS", lb::graphics::ShaderStage::CS},
	{"upsample_bilateral_float4CS", lb::graphics::ShaderStage::CS},
	{"upsample_bilateral_unorm1CS", lb::graphics::ShaderStage::CS},
	{"upsample_bilateral_unorm4CS", lb::graphics::ShaderStage::CS},
	{"temporalaaCS", lb::graphics::ShaderStage::CS},
	{"tonemapCS", lb::graphics::ShaderStage::CS},
	{"underwaterCS", lb::graphics::ShaderStage::CS},
	{"ssaoCS", lb::graphics::ShaderStage::CS},
	{"rtreflectionCS", lb::graphics::ShaderStage::CS, lb::graphics::ShaderModel::SM_6_5},
	{"ssr_tileMaxRoughness_horizontalCS", lb::graphics::ShaderStage::CS},
	{"ssr_tileMaxRoughness_verticalCS", lb::graphics::ShaderStage::CS},
	{"ssr_depthHierarchyCS", lb::graphics::ShaderStage::CS},
	{"ssr_resolveCS", lb::graphics::ShaderStage::CS},
	{"ssr_temporalCS", lb::graphics::ShaderStage::CS},
	{"ssr_upsampleCS", lb::graphics::ShaderStage::CS},
	{"ssr_raytraceCS", lb::graphics::ShaderStage::CS},
	{"ssr_raytraceCS_cheap", lb::graphics::ShaderStage::CS},
	{"ssr_raytraceCS_earlyexit", lb::graphics::ShaderStage::CS},
	{"sharpenCS", lb::graphics::ShaderStage::CS},
	{"skinningCS", lb::graphics::ShaderStage::CS},
	{"resolveMSAADepthStencilCS", lb::graphics::ShaderStage::CS},
	{"raytraceCS", lb::graphics::ShaderStage::CS},
	{"raytraceCS_rtapi", lb::graphics::ShaderStage::CS, lb::graphics::ShaderModel::SM_6_5},
	{"paint_textureCS", lb::graphics::ShaderStage::CS},
	{"oceanUpdateDisplacementMapCS", lb::graphics::ShaderStage::CS},
	{"oceanUpdateGradientFoldingCS", lb::graphics::ShaderStage::CS},
	{"oceanSimulatorCS", lb::graphics::ShaderStage::CS},
	{"motionblur_neighborhoodMaxVelocityCS", lb::graphics::ShaderStage::CS},
	{"motionblur_tileMaxVelocity_horizontalCS", lb::graphics::ShaderStage::CS},
	{"motionblur_tileMaxVelocity_verticalCS", lb::graphics::ShaderStage::CS},
	{"luminancePass2CS", lb::graphics::ShaderStage::CS},
	{"motionblurCS", lb::graphics::ShaderStage::CS},
	{"motionblurCS_cheap", lb::graphics::ShaderStage::CS},
	{"motionblurCS_earlyexit", lb::graphics::ShaderStage::CS},
	{"luminancePass1CS", lb::graphics::ShaderStage::CS},
	{"lightShaftsCS", lb::graphics::ShaderStage::CS},
	{"lightCullingCS_ADVANCED_DEBUG", lb::graphics::ShaderStage::CS},
	{"lightCullingCS_DEBUG", lb::graphics::ShaderStage::CS},
	{"lightCullingCS", lb::graphics::ShaderStage::CS},
	{"lightCullingCS_ADVANCED", lb::graphics::ShaderStage::CS},
	{"hbaoCS", lb::graphics::ShaderStage::CS},
	{"gpusortlib_sortInnerCS", lb::graphics::ShaderStage::CS},
	{"gpusortlib_sortStepCS", lb::graphics::ShaderStage::CS},
	{"gpusortlib_kickoffSortCS", lb::graphics::ShaderStage::CS},
	{"gpusortlib_sortCS", lb::graphics::ShaderStage::CS},
	{"fxaaCS", lb::graphics::ShaderStage::CS},
	{"filterEnvMapCS", lb::graphics::ShaderStage::CS},
	{"fft_512x512_c2c_CS", lb::graphics::ShaderStage::CS},
	{"fft_512x512_c2c_v2_CS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_sphpartitionCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_sphcellallocationCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_sphbinningCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_simulateCS_SORTING", lb::graphics::ShaderStage::CS},
	{"emittedparticle_simulateCS_SORTING_DEPTHCOLLISIONS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_sphdensityCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_sphforceCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_kickoffUpdateCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_simulateCS_DEPTHCOLLISIONS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_emitCS", lb::graphics::ShaderStage::CS},
	{"emittedparticle_emitCS_FROMMESH", lb::graphics::ShaderStage::CS},
	{"emittedparticle_emitCS_volume", lb::graphics::ShaderStage::CS},
	{"emittedparticle_finishUpdateCS", lb::graphics::ShaderStage::CS},
	{"downsample4xCS", lb::graphics::ShaderStage::CS},
	{"lineardepthCS", lb::graphics::ShaderStage::CS},
	{"depthoffield_prepassCS_earlyexit", lb::graphics::ShaderStage::CS},
	{"depthoffield_mainCS_cheap", lb::graphics::ShaderStage::CS},
	{"depthoffield_mainCS_earlyexit", lb::graphics::ShaderStage::CS },
	{"depthoffield_postfilterCS", lb::graphics::ShaderStage::CS },
	{"copytexture2D_float4_borderexpandCS", lb::graphics::ShaderStage::CS },
	{"copytexture2D_unorm4_borderexpandCS", lb::graphics::ShaderStage::CS },
	{"copytexture2D_unorm4CS", lb::graphics::ShaderStage::CS },
	{"copytexture2D_float4CS", lb::graphics::ShaderStage::CS },
	{"chromatic_aberrationCS", lb::graphics::ShaderStage::CS },
	{"bvh_hierarchyCS", lb::graphics::ShaderStage::CS },
	{"bvh_primitivesCS", lb::graphics::ShaderStage::CS },
	{"bvh_propagateaabbCS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_wide_unorm1CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_wide_unorm4CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_unorm1CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_unorm4CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_wide_float1CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_wide_float3CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_wide_float4CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_wide_unorm4CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_float1CS", lb::graphics::ShaderStage::CS },
	{"blur_gaussian_float3CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_unorm4CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_wide_float1CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_wide_float3CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_wide_float4CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_wide_unorm1CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_float1CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_float3CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_float4CS", lb::graphics::ShaderStage::CS },
	{"blur_bilateral_unorm1CS", lb::graphics::ShaderStage::CS },
	{"normalsfromdepthCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_curlnoiseCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_detailnoiseCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_renderCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_renderCS_capture", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_renderCS_capture_MSAA", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_reprojectCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_shadow_filterCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_shadow_renderCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_shapenoiseCS", lb::graphics::ShaderStage::CS },
	{"volumetricCloud_upsamplePS", lb::graphics::ShaderStage::PS },
	{"volumetricCloud_weathermapCS", lb::graphics::ShaderStage::CS },
	{"shadingRateClassificationCS", lb::graphics::ShaderStage::CS },
	{"shadingRateClassificationCS_DEBUG", lb::graphics::ShaderStage::CS },
	{"aerialPerspectiveCS", lb::graphics::ShaderStage::CS },
	{"aerialPerspectiveCS_capture", lb::graphics::ShaderStage::CS },
	{"aerialPerspectiveCS_capture_MSAA", lb::graphics::ShaderStage::CS },
	{"skyAtmosphere_cameraVolumeLutCS", lb::graphics::ShaderStage::CS },
	{"skyAtmosphere_transmittanceLutCS", lb::graphics::ShaderStage::CS },
	{"skyAtmosphere_skyViewLutCS", lb::graphics::ShaderStage::CS },
	{"skyAtmosphere_multiScatteredLuminanceLutCS", lb::graphics::ShaderStage::CS },
	{"skyAtmosphere_skyLuminanceLutCS", lb::graphics::ShaderStage::CS },
	{"screenspaceshadowCS", lb::graphics::ShaderStage::CS },
	{"rtshadowCS", lb::graphics::ShaderStage::CS, lb::graphics::ShaderModel::SM_6_5 },
	{"rtshadow_denoise_tileclassificationCS", lb::graphics::ShaderStage::CS },
	{"rtshadow_denoise_filterCS", lb::graphics::ShaderStage::CS },
	{"rtshadow_denoise_temporalCS", lb::graphics::ShaderStage::CS },
	{"rtshadow_upsampleCS", lb::graphics::ShaderStage::CS },
	{"rtaoCS", lb::graphics::ShaderStage::CS, lb::graphics::ShaderModel::SM_6_5 },
	{"rtao_denoise_tileclassificationCS", lb::graphics::ShaderStage::CS },
	{"rtao_denoise_filterCS", lb::graphics::ShaderStage::CS },
	{"visibility_resolveCS", lb::graphics::ShaderStage::CS },
	{"visibility_resolveCS_MSAA", lb::graphics::ShaderStage::CS },
	{"visibility_velocityCS", lb::graphics::ShaderStage::CS },
	{"visibility_skyCS", lb::graphics::ShaderStage::CS },
	{"ddgi_rayallocationCS", lb::graphics::ShaderStage::CS },
	{"ddgi_indirectprepareCS", lb::graphics::ShaderStage::CS },
	{"ddgi_raytraceCS", lb::graphics::ShaderStage::CS },
	{"ddgi_raytraceCS_rtapi", lb::graphics::ShaderStage::CS, lb::graphics::ShaderModel::SM_6_5 },
	{"ddgi_updateCS", lb::graphics::ShaderStage::CS },
	{"ddgi_updateCS_depth", lb::graphics::ShaderStage::CS },
	{"terrainVirtualTextureUpdateCS", lb::graphics::ShaderStage::CS },
	{"terrainVirtualTextureUpdateCS_normalmap", lb::graphics::ShaderStage::CS },
	{"terrainVirtualTextureUpdateCS_surfacemap", lb::graphics::ShaderStage::CS },
	{"terrainVirtualTextureUpdateCS_emissivemap", lb::graphics::ShaderStage::CS },
	{"meshlet_prepareCS", lb::graphics::ShaderStage::CS },
	{"impostor_prepareCS", lb::graphics::ShaderStage::CS },
	{"virtualTextureTileRequestsCS", lb::graphics::ShaderStage::CS },
	{"virtualTextureTileAllocateCS", lb::graphics::ShaderStage::CS },
	{"virtualTextureResidencyUpdateCS", lb::graphics::ShaderStage::CS },
	{"windCS", lb::graphics::ShaderStage::CS },
	{"yuv_to_rgbCS", lb::graphics::ShaderStage::CS },
	{"wetmap_updateCS", lb::graphics::ShaderStage::CS },
	{"causticsCS", lb::graphics::ShaderStage::CS },
	{"depth_reprojectCS", lb::graphics::ShaderStage::CS },
	{"depth_pyramidCS", lb::graphics::ShaderStage::CS },
	{"lightmap_expandCS", lb::graphics::ShaderStage::CS },


	{"emittedparticlePS_soft", lb::graphics::ShaderStage::PS },
	{"imagePS", lb::graphics::ShaderStage::PS },
	{"emittedparticlePS_soft_lighting", lb::graphics::ShaderStage::PS },
	{"oceanSurfacePS", lb::graphics::ShaderStage::PS },
	{"hairparticlePS", lb::graphics::ShaderStage::PS },
	{"hairparticlePS_simple", lb::graphics::ShaderStage::PS },
	{"hairparticlePS_prepass", lb::graphics::ShaderStage::PS },
	{"hairparticlePS_prepass_depthonly", lb::graphics::ShaderStage::PS },
	{"hairparticlePS_shadow", lb::graphics::ShaderStage::PS },
	{"volumetricLight_SpotPS", lb::graphics::ShaderStage::PS },
	{"volumetricLight_PointPS", lb::graphics::ShaderStage::PS },
	{"volumetricLight_DirectionalPS", lb::graphics::ShaderStage::PS },
	{"voxelPS", lb::graphics::ShaderStage::PS },
	{"vertexcolorPS", lb::graphics::ShaderStage::PS },
	{"upsample_bilateralPS", lb::graphics::ShaderStage::PS },
	{"sunPS", lb::graphics::ShaderStage::PS },
	{"skyPS_dynamic", lb::graphics::ShaderStage::PS },
	{"skyPS_static", lb::graphics::ShaderStage::PS },
	{"shadowPS_transparent", lb::graphics::ShaderStage::PS },
	{"shadowPS_water", lb::graphics::ShaderStage::PS },
	{"shadowPS_alphatest", lb::graphics::ShaderStage::PS },
	{"paintdecalPS", lb::graphics::ShaderStage::PS },
	{"renderlightmapPS", lb::graphics::ShaderStage::PS },
	{"renderlightmapPS_rtapi", lb::graphics::ShaderStage::PS, lb::graphics::ShaderModel::SM_6_5 },
	{"raytrace_debugbvhPS", lb::graphics::ShaderStage::PS },
	{"outlinePS", lb::graphics::ShaderStage::PS },
	{"oceanSurfaceSimplePS", lb::graphics::ShaderStage::PS },
	{"objectPS_voxelizer", lb::graphics::ShaderStage::PS },
	{"objectPS_hologram", lb::graphics::ShaderStage::PS },
	{"objectPS_paintradius", lb::graphics::ShaderStage::PS },
	{"objectPS_simple", lb::graphics::ShaderStage::PS },
	{"objectPS_debug", lb::graphics::ShaderStage::PS },
	{"objectPS_prepass", lb::graphics::ShaderStage::PS },
	{"objectPS_prepass_alphatest", lb::graphics::ShaderStage::PS },
	{"objectPS_prepass_depthonly", lb::graphics::ShaderStage::PS },
	{"objectPS_prepass_depthonly_alphatest", lb::graphics::ShaderStage::PS },
	{"lightVisualizerPS", lb::graphics::ShaderStage::PS },
	{"lensFlarePS", lb::graphics::ShaderStage::PS },
	{"impostorPS", lb::graphics::ShaderStage::PS },
	{"impostorPS_simple", lb::graphics::ShaderStage::PS },
	{"impostorPS_prepass", lb::graphics::ShaderStage::PS },
	{"impostorPS_prepass_depthonly", lb::graphics::ShaderStage::PS },
	{"forceFieldVisualizerPS", lb::graphics::ShaderStage::PS },
	{"fontPS", lb::graphics::ShaderStage::PS },
	{"envMap_skyPS_static", lb::graphics::ShaderStage::PS },
	{"envMap_skyPS_dynamic", lb::graphics::ShaderStage::PS },
	{"envMapPS", lb::graphics::ShaderStage::PS },
	{"emittedparticlePS_soft_distortion", lb::graphics::ShaderStage::PS },
	{"downsampleDepthBuffer4xPS", lb::graphics::ShaderStage::PS },
	{"emittedparticlePS_simple", lb::graphics::ShaderStage::PS },
	{"cubeMapPS", lb::graphics::ShaderStage::PS },
	{"circlePS", lb::graphics::ShaderStage::PS },
	{"captureImpostorPS", lb::graphics::ShaderStage::PS },
	{"ddgi_debugPS", lb::graphics::ShaderStage::PS },
	{"copyDepthPS", lb::graphics::ShaderStage::PS },
	{"copyStencilBitPS", lb::graphics::ShaderStage::PS },
	{"trailPS", lb::graphics::ShaderStage::PS },


	{"hairparticleVS", lb::graphics::ShaderStage::VS },
	{"emittedparticleVS", lb::graphics::ShaderStage::VS },
	{"imageVS", lb::graphics::ShaderStage::VS },
	{"fontVS", lb::graphics::ShaderStage::VS },
	{"voxelVS", lb::graphics::ShaderStage::VS },
	{"vertexcolorVS", lb::graphics::ShaderStage::VS },
	{"volumetriclight_directionalVS", lb::graphics::ShaderStage::VS },
	{"volumetriclight_pointVS", lb::graphics::ShaderStage::VS },
	{"volumetriclight_spotVS", lb::graphics::ShaderStage::VS },
	{"vSpotLightVS", lb::graphics::ShaderStage::VS },
	{"vPointLightVS", lb::graphics::ShaderStage::VS },
	{"sphereVS", lb::graphics::ShaderStage::VS },
	{"skyVS", lb::graphics::ShaderStage::VS },
	{"postprocessVS", lb::graphics::ShaderStage::VS },
	{"paintdecalVS", lb::graphics::ShaderStage::VS },
	{"renderlightmapVS", lb::graphics::ShaderStage::VS },
	{"raytrace_screenVS", lb::graphics::ShaderStage::VS },
	{"oceanSurfaceVS", lb::graphics::ShaderStage::VS },
	{"objectVS_debug", lb::graphics::ShaderStage::VS },
	{"objectVS_voxelizer", lb::graphics::ShaderStage::VS },
	{"lensFlareVS", lb::graphics::ShaderStage::VS },
	{"impostorVS", lb::graphics::ShaderStage::VS },
	{"forceFieldPointVisualizerVS", lb::graphics::ShaderStage::VS },
	{"forceFieldPlaneVisualizerVS", lb::graphics::ShaderStage::VS },
	{"envMap_skyVS", lb::graphics::ShaderStage::VS },
	{"envMapVS", lb::graphics::ShaderStage::VS },
	{"occludeeVS", lb::graphics::ShaderStage::VS },
	{"ddgi_debugVS", lb::graphics::ShaderStage::VS },
	{"objectGS_primitiveID_emulation", lb::graphics::ShaderStage::GS },
	{"objectGS_primitiveID_emulation_alphatest", lb::graphics::ShaderStage::GS },
	{"voxelGS", lb::graphics::ShaderStage::GS },
	{"objectGS_voxelizer", lb::graphics::ShaderStage::GS },
	{"objectVS_simple", lb::graphics::ShaderStage::VS },
	{"objectVS_common", lb::graphics::ShaderStage::VS },
	{"objectVS_common_tessellation", lb::graphics::ShaderStage::VS },
	{"objectVS_prepass", lb::graphics::ShaderStage::VS },
	{"objectVS_prepass_alphatest", lb::graphics::ShaderStage::VS },
	{"objectVS_prepass_tessellation", lb::graphics::ShaderStage::VS },
	{"objectVS_prepass_alphatest_tessellation", lb::graphics::ShaderStage::VS },
	{"objectVS_simple_tessellation", lb::graphics::ShaderStage::VS },
	{"shadowVS", lb::graphics::ShaderStage::VS },
	{"shadowVS_alphatest", lb::graphics::ShaderStage::VS },
	{"shadowVS_transparent", lb::graphics::ShaderStage::VS },
	{"screenVS", lb::graphics::ShaderStage::VS },
	{"trailVS", lb::graphics::ShaderStage::VS },



	{"objectDS", lb::graphics::ShaderStage::DS },
	{"objectDS_prepass", lb::graphics::ShaderStage::DS },
	{"objectDS_prepass_alphatest", lb::graphics::ShaderStage::DS },
	{"objectDS_simple", lb::graphics::ShaderStage::DS },


	{"objectHS", lb::graphics::ShaderStage::HS },
	{"objectHS_prepass", lb::graphics::ShaderStage::HS },
	{"objectHS_prepass_alphatest", lb::graphics::ShaderStage::HS },
	{"objectHS_simple", lb::graphics::ShaderStage::HS },

	{"emittedparticleMS", lb::graphics::ShaderStage::MS },

	{"objectMS", lb::graphics::ShaderStage::MS },
	{"objectMS_prepass", lb::graphics::ShaderStage::MS },
	{"objectMS_prepass_alphatest", lb::graphics::ShaderStage::MS },
	{"objectMS_simple", lb::graphics::ShaderStage::MS },
	{"shadowMS", lb::graphics::ShaderStage::MS },
	{"shadowMS_alphatest", lb::graphics::ShaderStage::MS },
	{"shadowMS_transparent", lb::graphics::ShaderStage::MS },

	{"objectAS", lb::graphics::ShaderStage::AS },


	//{"rtreflectionLIB", lb::graphics::ShaderStage::LIB },
};

struct Target
{
	lb::graphics::ShaderFormat format;
	std::string dir;
};
lb::vector<Target> targets;
lb::unordered_map<std::string, lb::shadercompiler::CompilerOutput> results;
bool rebuild = false;
bool shaderdump_enabled = false;

using namespace lb::graphics;

int main(int argc, char* argv[])
{
	lb::shadercompiler::Flags compile_flags = lb::shadercompiler::Flags::NONE;
	std::cout << "[Libertas Engine Offline Shader Compiler]\n";
	std::cout << "Available command arguments:\n";
	std::cout << "\thlsl6 : \t\tCompile shaders to hlsl6 (dx12) format (using dxcompiler)\n";
	std::cout << "\tspirv : \t\tCompile shaders to spirv (vulkan) format (using dxcompiler)\n";
	std::cout << "\thlsl6_xs : \t\tCompile shaders to hlsl6 Xbox Series native (dx12) format (requires Xbox SDK)\n";
	std::cout << "\tps5 : \t\t\tCompile shaders to PlayStation 5 native format (requires PlayStation 5 SDK)\n";
	std::cout << "\trebuild : \t\tAll shaders will be rebuilt, regardless if they are outdated or not\n";
	std::cout << "\tdisable_optimization : \tShaders will be compiled without optimizations\n";
	std::cout << "\tstrip_reflection : \tReflection will be stripped from shader binary to reduce file size\n";
	std::cout << "\tshaderdump : \t\tShaders will be saved to wiShaderDump.h C++ header file (rebuild is assumed)\n";
	std::cout << "Command arguments used: ";

	lb::arguments::Parse(argc, argv);

	if (lb::arguments::HasArgument("hlsl6"))
	{
		targets.push_back({ ShaderFormat::HLSL6, "shaders/compiled/hlsl6/" });
		std::cout << "hlsl6 ";
	}
	if (lb::arguments::HasArgument("spirv"))
	{
		targets.push_back({ ShaderFormat::SPIRV, "shaders/compiled/spirv/" });
		std::cout << "spirv ";
	}
	if (lb::arguments::HasArgument("hlsl6_xs"))
	{
		targets.push_back({ ShaderFormat::HLSL6_XS, "shaders/compiled/hlsl6_xs/" });
		std::cout << "hlsl6_xs ";
	}
	if (lb::arguments::HasArgument("ps5"))
	{
		targets.push_back({ ShaderFormat::PS5, "shaders/compiled/ps5/" });
		std::cout << "ps5 ";
	}

	if (lb::arguments::HasArgument("shaderdump"))
	{
		shaderdump_enabled = true;
		rebuild = true;
		std::cout << "shaderdump ";
	}

	if (lb::arguments::HasArgument("rebuild"))
	{
		rebuild = true;
		std::cout << "rebuild ";
	}

	if (lb::arguments::HasArgument("disable_optimization"))
	{
		compile_flags |= lb::shadercompiler::Flags::DISABLE_OPTIMIZATION;
		std::cout << "disable_optimization ";
	}

	if (lb::arguments::HasArgument("strip_reflection"))
	{
		compile_flags |= lb::shadercompiler::Flags::STRIP_REFLECTION;
		std::cout << "strip_reflection ";
	}

	std::cout << "\n";

	if (targets.empty())
	{
		targets = {
			{ ShaderFormat::HLSL6, "shaders/compiled/hlsl6/" },
			{ ShaderFormat::SPIRV, "shaders/compiled/spirv/" },		
		};
		shaderdump_enabled = true;
		rebuild = true;
		std::cout << "No shader formats were specified, assuming command arguments: spirv hlsl6, shaderdump\n";
	}

	// permutations for objectPS:
	shaders.push_back({ "objectPS", lb::graphics::ShaderStage::PS });
	for (auto& x : lb::scene::MaterialComponent::shaderTypeDefines)
	{
		shaders.back().permutations.emplace_back().defines = x;

		// same but with TRANSPARENT:
		shaders.back().permutations.emplace_back().defines = x;
		shaders.back().permutations.back().defines.push_back("TRANSPARENT");
	}

	// permutations for visibility_surfaceCS:
	shaders.push_back({ "visibility_surfaceCS", lb::graphics::ShaderStage::CS });
	for (auto& x : lb::scene::MaterialComponent::shaderTypeDefines)
	{
		shaders.back().permutations.emplace_back().defines = x;
	}

	// permutations for visibility_surfaceCS REDUCED:
	shaders.push_back({ "visibility_surfaceCS", lb::graphics::ShaderStage::CS });
	for (auto& x : lb::scene::MaterialComponent::shaderTypeDefines)
	{
		auto defines = x;
		defines.push_back("REDUCED");
		shaders.back().permutations.emplace_back().defines = defines;
	}

	// permutations for visibility_shadeCS:
	shaders.push_back({ "visibility_shadeCS", lb::graphics::ShaderStage::CS });
	for (auto& x : lb::scene::MaterialComponent::shaderTypeDefines)
	{
		shaders.back().permutations.emplace_back().defines = x;
	}

	lb::jobsystem::Initialize();
	lb::jobsystem::context ctx;

	std::string SHADERSOURCEPATH = lb::renderer::GetShaderSourcePath();
	lb::helper::MakePathAbsolute(SHADERSOURCEPATH);

	std::cout << "[Libertas Engine Offline Shader Compiler] Searching for outdated shaders...\n";
	lb::Timer timer;
	static int errors = 0;

	for (auto& target : targets)
	{
		std::string SHADERPATH = target.dir;
		lb::helper::DirectoryCreate(SHADERPATH);

		for (auto& shader : shaders)
		{
			if (target.format == ShaderFormat::HLSL5)
			{
				if (
					shader.stage == ShaderStage::MS ||
					shader.stage == ShaderStage::AS ||
					shader.stage == ShaderStage::LIB
					)
				{
					// shader stage not applicable to HLSL5
					continue;
				}
			}
			lb::vector<ShaderEntry::Permutation> permutations = shader.permutations;
			if (permutations.empty())
			{
				permutations.emplace_back();
			}

			for (auto permutation : permutations)
			{
				lb::jobsystem::Execute(ctx, [=](lb::jobsystem::JobArgs args) {
					std::string shaderbinaryfilename = SHADERPATH + shader.name;
					for (auto& def : permutation.defines)
					{
						shaderbinaryfilename += "_" + def;
					}
					shaderbinaryfilename += ".cso";
					if (!rebuild && !lb::shadercompiler::IsShaderOutdated(shaderbinaryfilename))
					{
						return;
					}

					lb::shadercompiler::CompilerInput input;
					input.flags = compile_flags;
					input.format = target.format;
					input.stage = shader.stage;
					input.shadersourcefilename = SHADERSOURCEPATH + shader.name + ".hlsl";
					input.include_directories.push_back(SHADERSOURCEPATH);
					input.include_directories.push_back(SHADERSOURCEPATH + lb::helper::GetDirectoryFromPath(shader.name));
					input.minshadermodel = shader.minshadermodel;
					input.defines = permutation.defines;

					if (input.minshadermodel > ShaderModel::SM_5_0 && target.format == ShaderFormat::HLSL5)
					{
						// if shader format cannot support shader model, then we cancel the task without returning error
						return;
					}
					if (target.format == ShaderFormat::PS5 && (input.minshadermodel >= ShaderModel::SM_6_5 || input.stage == ShaderStage::MS || input.stage == ShaderStage::AS))
					{
						// TODO PS5 raytracing, mesh shader
						return;
					}
					if (target.format == ShaderFormat::HLSL6_XS && (input.stage == ShaderStage::MS || input.stage == ShaderStage::AS))
					{
						// TODO Xbox mesh shader
						return;
					}

					lb::shadercompiler::CompilerOutput output;
					lb::shadercompiler::Compile(input, output);

					if (output.IsValid())
					{
						lb::shadercompiler::SaveShaderAndMetadata(shaderbinaryfilename, output);

						locker.lock();
						if (!output.error_message.empty())
						{
							std::cerr << output.error_message << "\n";
						}
						std::cout << "shader compiled: " << shaderbinaryfilename << "\n";
						if (shaderdump_enabled)
						{
							results[shaderbinaryfilename] = output;
						}
						locker.unlock();
					}
					else
					{
						locker.lock();
						std::cerr << "shader compile FAILED: " << shaderbinaryfilename << "\n" << output.error_message;
						errors++;
						locker.unlock();
					}

				});
			}
		}
	}
	lb::jobsystem::Wait(ctx);

	std::cout << "[Libertas Engine Offline Shader Compiler] Finished in " << std::setprecision(4) << timer.elapsed_seconds() << " seconds with " << errors << " errors\n";

	if (shaderdump_enabled)
	{
		std::cout << "[Libertas Engine Offline Shader Compiler] Creating ShaderDump...\n";
		timer.record();
		std::string ss;
		ss += "namespace lbShaderDump {\n";
		for (auto& x : results)
		{
			auto& name = x.first;
			auto& output = x.second;

			std::string name_repl = name;
			std::replace(name_repl.begin(), name_repl.end(), '/', '_');
			std::replace(name_repl.begin(), name_repl.end(), '.', '_');
			std::replace(name_repl.begin(), name_repl.end(), '-', '_');
			ss += "const uint8_t " + name_repl + "[] = {";
			for (size_t i = 0; i < output.shadersize; ++i)
			{
				ss += std::to_string((uint32_t)output.shaderdata[i]) + ",";
			}
			ss += "};\n";
		}
		ss += "struct ShaderDumpEntry{const uint8_t* data; size_t size;};\n";
		ss += "const lb::unordered_map<std::string, ShaderDumpEntry> shaderdump = {\n";
		for (auto& x : results)
		{
			auto& name = x.first;
			auto& output = x.second;

			std::string name_repl = name;
			std::replace(name_repl.begin(), name_repl.end(), '/', '_');
			std::replace(name_repl.begin(), name_repl.end(), '.', '_');
			std::replace(name_repl.begin(), name_repl.end(), '-', '_');
			ss += "{\"" + name + "\", {" + name_repl + ",sizeof(" + name_repl + ")}},\n";
		}
		ss += "};\n"; // map end
		ss += "}\n"; // namespace end
		lb::helper::FileWrite("wiShaderDump.h", (uint8_t*)ss.c_str(), ss.length());
		std::cout << "[Libertas Engine Offline Shader Compiler] ShaderDump written to wiShaderDump.h in " << std::setprecision(4) << timer.elapsed_seconds() << " seconds\n";
	}

	lb::jobsystem::ShutDown();

	return errors;
}
