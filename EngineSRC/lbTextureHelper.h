#pragma once
#include "CommonInclude.h"
#include "lbGraphicsDevice.h"
#include "lbColor.h"

namespace lb::texturehelper
{
	void Initialize();

	const lb::graphics::Texture* getLogo();
	const lb::graphics::Texture* getRandom64x64();
	const lb::graphics::Texture* getColorGradeDefault();
	const lb::graphics::Texture* getNormalMapDefault();
	const lb::graphics::Texture* getBlackCubeMap();
	const lb::graphics::Texture* getUINT4();
	const lb::graphics::Texture* getBlueNoise();
	const lb::graphics::Texture* getWaterRipple();
	const lb::graphics::Texture* getCheckerBoard();

	const lb::graphics::Texture* getWhite();
	const lb::graphics::Texture* getBlack();
	const lb::graphics::Texture* getTransparent();

	bool CreateTexture(
		lb::graphics::Texture& texture,
		const void* data,
		uint32_t width,
		uint32_t height,
		lb::graphics::Format format = lb::graphics::Format::R8G8B8A8_UNORM,
		lb::graphics::Swizzle swizzle = {}
	);

	enum class GradientType
	{
		Linear,
		Circular,
		Angular,
	};
	enum class GradientFlags
	{
		None = 0,
		Inverse = 1 << 0,		// inverts resulting gradient
		Smoothstep = 1 << 1,	// applies smoothstep function to resulting gradient
		PerlinNoise = 1 << 2,	// applies perlin noise to gradient
		R16Unorm = 1 << 3,		// the texture will be created in R16_UNORM format instead of R8_UNORM
	};
	lb::graphics::Texture CreateGradientTexture(
		GradientType type,
		uint32_t width,
		uint32_t height,
		const XMFLOAT2& uv_start = XMFLOAT2(0, 0),
		const XMFLOAT2& uv_end = XMFLOAT2(1, 0),
		GradientFlags gradient_flags = GradientFlags::None,
		lb::graphics::Swizzle swizzle = { lb::graphics::ComponentSwizzle::R, lb::graphics::ComponentSwizzle::R, lb::graphics::ComponentSwizzle::R, lb::graphics::ComponentSwizzle::R },
		float perlin_scale = 1,
		uint32_t perlin_seed = 1234u,
		int perlin_octaves = 8,
		float perlin_persistence = 0.5f
	);

	// Similar to CreateGradientTexture() with GradientType::Angular type but different parameters
	lb::graphics::Texture CreateCircularProgressGradientTexture(
		uint32_t width,
		uint32_t height,
		const XMFLOAT2& direction = XMFLOAT2(0, 1),
		bool counter_clockwise = false,
		lb::graphics::Swizzle swizzle = { lb::graphics::ComponentSwizzle::R, lb::graphics::ComponentSwizzle::R, lb::graphics::ComponentSwizzle::R, lb::graphics::ComponentSwizzle::R }
	);

	// Create a lens distortion normal map (16-bit precision)
	//	width		: texture width in pixels
	//	height		: texture height in pixels
	//	uv_start	: center of lens in uv-space [0,1]
	//	radius		: radius of lens in uv_space [0,1]
	//	squish		: squish the lens (higher value is more squished down)
	//	blend		: blend out the distortion by a constant amount
	//	edge_smoothness : smoothen the edge of the circle
	lb::graphics::Texture CreateLensDistortionNormalMap(
		uint32_t width,
		uint32_t height,
		const XMFLOAT2& uv_start = XMFLOAT2(0.5f, 0.5f),
		float radius = 0.5f,
		float squish = 1,
		float blend = 1,
		float edge_smoothness = 0.04f
	);
};

template<>
struct enable_bitmask_operators<lb::texturehelper::GradientFlags> {
	static const bool enable = true;
};
