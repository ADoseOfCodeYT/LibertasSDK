#ifndef LIBERTASENGINE
#define LIBERTASENGINE
#define LIBERTAS_ENGINE

// NOTE:
// The purpose of this file is to expose all engine features.
// It should be included in the engine's implementing application not the engine itself!
// It should be included in the precompiled header if available.

#include "CommonInclude.h"

// High-level interface:
#include "wiApplication.h"
#include "wiRenderPath.h"
#include "wiRenderPath2D.h"
#include "wiRenderPath3D.h"
#include "wiRenderPath3D_PathTracing.h"
#include "wiLoadingScreen.h"

// Engine-level systems
#include "wiVersion.h"
#include "wiPlatform.h"
#include "wiBacklog.h"
#include "wiPrimitive.h"
#include "wiImage.h"
#include "wiFont.h"
#include "wiSprite.h"
#include "wiSpriteFont.h"
#include "wiScene.h"
#include "wiECS.h"
#include "wiEmittedParticle.h"
#include "wiHairParticle.h"
#include "wiRenderer.h"
#include "wiMath.h"
#include "wiAudio.h"
#include "wiResourceManager.h"
#include "wiTimer.h"
#include "wiHelper.h"
#include "wiInput.h"
#include "wiRawInput.h"
#include "wiXInput.h"
#include "wiSDLInput.h"
#include "wiTextureHelper.h"
#include "wiRandom.h"
#include "wiColor.h"
#include "wiPhysics.h"
#include "wiEnums.h"
#include "wiInitializer.h"
#include "wiLua.h"
#include "wiGraphics.h"
#include "wiGraphicsDevice.h"
#include "wiGUI.h"
#include "wiArchive.h"
#include "wiSpinLock.h"
#include "wiRectPacker.h"
#include "wiProfiler.h"
#include "wiOcean.h"
#include "wiFFTGenerator.h"
#include "wiArguments.h"
#include "wiGPUBVH.h"
#include "wiGPUSortLib.h"
#include "wiJobSystem.h"
#include "wiNetwork.h"
#include "wiEventHandler.h"
#include "wiShaderCompiler.h"
#include "wiCanvas.h"
#include "wiUnorderedMap.h"
#include "wiUnorderedSet.h"
#include "wiVector.h"
#include "wiNoise.h"
#include "wiConfig.h"
#include "wiTerrain.h"
#include "wiLocalization.h"
#include "wiVideo.h"
#include "wiVoxelGrid.h"
#include "wiPathQuery.h"
#include "wiTrailRenderer.h"

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
namespace lbBackLog = lb::backlog;
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
