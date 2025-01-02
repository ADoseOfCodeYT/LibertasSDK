#ifndef LIBERTASENGINE
#define LIBERTASENGINE
#define LIBERTAS_ENGINE

// NOTE:
// The purpose of this file is to expose all engine features.
// It should be included in the engine's implementing application not the engine itself!
// It should be included in the precompiled header if available.

#include "CommonInclude.h"

// High-level interface:
#include "lbApplication.h"
#include "lbRenderPath.h"
#include "lbRenderPath2D.h"
#include "lbRenderPath3D.h"
#include "lbRenderPath3D_PathTracing.h"
#include "lbLoadingScreen.h"

// Engine-level systems
#include "lbVersion.h"
#include "lbPlatform.h"
#include "lbConsole.h"
#include "lbConvars.h"
#include "lbPrimitive.h"
#include "lbImage.h"
#include "lbFont.h"
#include "lbSprite.h"
#include "lbSpriteFont.h"
#include "lbScene.h"
#include "lbECS.h"
#include "lbEmittedParticle.h"
#include "lbHairParticle.h"
#include "lbRenderer.h"
#include "lbMath.h"
#include "lbAudio.h"
#include "lbResourceManager.h"
#include "lbTimer.h"
#include "lbHelper.h"
#include "lbInput.h"
#include "lbRawInput.h"
#include "lbXInput.h"
#include "lbSDLInput.h"
#include "lbTextureHelper.h"
#include "lbRandom.h"
#include "lbColor.h"
#include "lbPhysics.h"
#include "lbEnums.h"
#include "lbInitializer.h"
#include "lbLua.h"
#include "lbGraphics.h"
#include "lbGraphicsDevice.h"
#include "lbGUI.h"
#include "lbArchive.h"
#include "lbSpinLock.h"
#include "lbRectPacker.h"
#include "lbProfiler.h"
#include "lbOcean.h"
#include "lbFFTGenerator.h"
#include "lbArguments.h"
#include "lbGPUBVH.h"
#include "lbGPUSortLib.h"
#include "lbJobSystem.h"
#include "lbNetwork.h"
#include "lbEventHandler.h"
#include "lbShaderCompiler.h"
#include "lbCanvas.h"
#include "lbUnorderedMap.h"
#include "lbUnorderedSet.h"
#include "lbVector.h"
#include "lbNoise.h"
#include "lbConfig.h"
#include "lbTerrain.h"
#include "lbLocalization.h"
#include "lbVideo.h"
#include "lbVoxelGrid.h"
#include "lbPathQuery.h"
#include "lbTrailRenderer.h"

#ifdef PLATFORM_WINDOWS_DESKTOP
#pragma comment(lib,"LibertasEngine_Windows.lib")
#endif // PLATFORM_WINDOWS_DESKTOP

#ifdef PLATFORM_XBOX
#pragma comment(lib,"LibertasEngine_XBOX.lib")
#endif // PLATFORM_XBOX

#ifdef PLATFORM_PS5
#pragma comment(lib,"LibertasEngine_PS5.a")
#endif // PLATFORM_PS5

#endif // LIBERTASENGINE
