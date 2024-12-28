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


// After version 0.59.11, namespaces were refactored into nested namespaces under the lb:: root namespace.
// To allow compatibility with older user code, the backwards compatibility definitions are included below.
// If you need backwards compatibility features, define the following before including this file:
//#define LIBERTASENGINE_BACKWARDS_COMPATIBILITY_0_59
#ifdef LIBERTASENGINE_BACKWARDS_COMPATIBILITY_0_59

using namespace lb;
using namespace lb::enums;

namespace lbGraphics = lb::graphics;
namespace lbShaderCompiler = lb::shadercompiler;
namespace lbFFTGenerator = lb::fftgenerator;
namespace lbFont = lb::font;
namespace lbImage = lb::image;
namespace lbGPUSortLib = lb::gpusortlib;
namespace lbRenderer = lb::renderer;
namespace lbTextureHelper = lb::texturehelper;
namespace lbHelper = lb::helper;
namespace lbMath = lb::math;
namespace lbRandom = lb::random;
namespace lbRectPacker = lb::rectpacker;
namespace lbResourceManager = lb::resourcemanager;
namespace lbStartupArguments = lb::arguments;
namespace lbInput = lb::input;
namespace lbXInput = lb::input::xinput;
namespace lbRawInput = lb::input::rawinput;
namespace lbSDLInput = lb::input::sdlinput;
namespace lbAudio = lb::audio;
namespace lbNetwork = lb::network;
namespace lbPhysicsEngine = lb::physics;
namespace lbLua = lb::lua;
namespace lbECS = lb::ecs;
namespace lbEvent = lb::eventhandler;
namespace lbInitializer = lb::initializer;
namespace lbJobSystem = lb::jobsystem;
namespace lbPlatform = lb::platform;
namespace lbScene = lb::scene;
namespace lbBackLog = lb::console;
namespace lbProfiler = lb::profiler;
namespace lbVersion = lb::version;

using MainComponent = lb::Application;
using wiFontParams = lb::font::Params;
using wiImageParams = lb::image::Params;
using wiGPUBVH = lb::GPUBVH;
using wiEmittedParticle = lb::EmittedParticleSystem;
using wiHairParticle = lb::HairParticleSystem;
using wiOcean = lb::Ocean;
using wiSprite = lb::Sprite;
using wiSpriteFont = lb::SpriteFont;
using wiGUI = lb::gui::GUI;
using wiEventArgs = lb::gui::EventArgs;
using wiWidget = lb::gui::Widget;
using wiButton = lb::gui::Button;
using wiLabel = lb::gui::Label;
using wiTextInputField = lb::gui::TextInputField;
using wiSlider = lb::gui::Slider;
using wiCheckBox = lb::gui::CheckBox;
using wiComboBox = lb::gui::ComboBox;
using wiWindow = lb::gui::Window;
using wiColorPicker = lb::gui::ColorPicker;
using wiTreeList = lb::gui::TreeList;
using wiArchive = lb::Archive;
using wiColor = lb::Color;
using wiFadeManager = lb::FadeManager;
using wiResource = lb::Resource;
using wiSpinLock = lb::SpinLock;
using wiTimer = lb::Timer;
using wiCanvas = lb::Canvas;
using AABB = lb::primitive::AABB;
using SPHERE = lb::primitive::Sphere;
using CAPSULE = lb::primitive::Capsule;
using RAY = lb::primitive::Ray;
using Frustum = lb::primitive::Frustum;
using Hitbox2D = lb::primitive::Hitbox2D;

using lb::image::STENCILMODE;
using lb::image::STENCILREFMODE;
using lb::image::SAMPLEMODE;
using lb::image::QUALITY;
using lb::font::Alignment;

static constexpr int SYSTEM_EVENT_THREAD_SAFE_POINT = lb::eventhandler::EVENT_THREAD_SAFE_POINT;
static constexpr int SYSTEM_EVENT_RELOAD_SHADERS = lb::eventhandler::EVENT_RELOAD_SHADERS;
static constexpr int SYSTEM_EVENT_SET_VSYNC = lb::eventhandler::EVENT_SET_VSYNC;
static constexpr XMFLOAT4X4 IDENTITYMATRIX = lb::math::IDENTITY_MATRIX;

#endif // LIBERTASENGINE_BACKWARDS_COMPATIBILITY_0_59



#endif // LIBERTASENGINE
