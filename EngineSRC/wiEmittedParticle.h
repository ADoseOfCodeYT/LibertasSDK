#pragma once
#include "CommonInclude.h"
#include "wiGraphicsDevice.h"
#include "wiPrimitive.h"
#include "shaders/ShaderInterop_EmittedParticle.h"
#include "wiEnums.h"
#include "wiMath.h"
#include "wiECS.h"
#include "wiScene_Decl.h"
#include "wiScene_Components.h"

namespace lb
{
	class Archive;
}

namespace lb
{
	class EmittedParticleSystem
	{
	public:

		// This is serialized, order of enums shouldn't change!
		enum PARTICLESHADERTYPE
		{
			SOFT,
			SOFT_DISTORTION,
			SIMPLE,
			SOFT_LIGHTING,
			PARTICLESHADERTYPE_COUNT,
			ENUM_FORCE_UINT32 = 0xFFFFFFFF,
		};

		ParticleCounters statistics = {};
		lb::graphics::GPUBuffer statisticsReadbackBuffer[lb::graphics::GraphicsDevice::GetBufferCount()];

		lb::graphics::GPUBuffer particleBuffer;
		lb::graphics::GPUBuffer aliveList[2];
		lb::graphics::GPUBuffer deadList;
		lb::graphics::GPUBuffer distanceBuffer; // for sorting
		lb::graphics::GPUBuffer sphGridCells;  // for SPH
		lb::graphics::GPUBuffer sphParticleCells;  // for SPH
		lb::graphics::GPUBuffer densityBuffer; // for SPH
		lb::graphics::GPUBuffer counterBuffer;
		lb::graphics::GPUBuffer indirectBuffers; // kickoffUpdate, simulation, draw
		lb::graphics::GPUBuffer constantBuffer;
		lb::graphics::GPUBuffer generalBuffer;
		lb::scene::MeshComponent::BufferView vb_pos;
		lb::scene::MeshComponent::BufferView vb_nor;
		lb::scene::MeshComponent::BufferView vb_uvs;
		lb::scene::MeshComponent::BufferView vb_col;
		lb::graphics::GPUBuffer primitiveBuffer; // raytracing
		lb::graphics::GPUBuffer culledIndirectionBuffer; // rasterization
		lb::graphics::GPUBuffer culledIndirectionBuffer2; // rasterization
		lb::graphics::Texture opacityCurveTex;

		lb::graphics::RaytracingAccelerationStructure BLAS;

	private:
		void CreateSelfBuffers();

		float emit = 0.0f;
		int burst = 0;
		float dt = 0;
		uint32_t active_frames = 0;

		uint32_t MAX_PARTICLES = 1000;

		mutable lb::vector<EmitLocation> emit_locations;

	public:
		void UpdateCPU(const lb::scene::TransformComponent& transform, float dt);
		void Burst(int num);
		void Burst(int num, const XMFLOAT3& position, const lb::Color& color = lb::Color::White());
		void Burst(int num, const XMFLOAT4X4& transform, const lb::Color& color = lb::Color::White());
		void Restart();

		// Must have a transform and material component, but mesh is optional
		void UpdateGPU(uint32_t instanceIndex, const lb::scene::MeshComponent* mesh, lb::graphics::CommandList cmd) const;
		void Draw(const lb::scene::MaterialComponent& material, lb::graphics::CommandList cmd, const PARTICLESHADERTYPE* shadertype_override = nullptr) const;

		void CreateRaytracingRenderData();

		ParticleCounters GetStatistics() { return statistics; }

		enum FLAGS
		{
			FLAG_EMPTY = 0,
			FLAG_DEBUG = 1 << 0,
			FLAG_PAUSED = 1 << 1,
			FLAG_SORTING = 1 << 2,
			FLAG_DEPTHCOLLISION = 1 << 3,
			FLAG_SPH_FLUIDSIMULATION = 1 << 4,
			FLAG_HAS_VOLUME = 1 << 5,
			FLAG_FRAME_BLENDING = 1 << 6,
			FLAG_COLLIDERS_DISABLED = 1 << 7,
			FLAG_USE_RAIN_BLOCKER = 1 << 8,
			FLAG_TAKE_COLOR_FROM_MESH = 1 << 9,
		};
		uint32_t _flags = FLAG_EMPTY;

		PARTICLESHADERTYPE shaderType = SOFT;

		lb::ecs::Entity meshID = lb::ecs::INVALID_ENTITY;

		float FIXED_TIMESTEP = -1.0f; // -1 : variable timestep; >=0 : fixed timestep

		float size = 1.0f;
		float random_factor = 1.0f;
		float normal_factor = 1.0f;
		float count = 0.0f;
		float life = 1.0f;
		float random_life = 1.0f;
		float scaleX = 1.0f;
		float scaleY = 1.0f;
		float rotation = 0.0f;
		float motionBlurAmount = 0.0f;
		float mass = 1.0f;
		float random_color = 0;
		float opacityCurveControlPeakStart = 0.1f; // peak start of the opacity relative to the particle lifeteime 
		float opacityCurveControlPeakEnd = 0.5f; // peak end of the opacity relative to the particle lifeteime 

		XMFLOAT3 velocity = {}; // starting velocity of all new particles
		XMFLOAT3 gravity = {}; // constant gravity force
		float drag = 1.0f; // constant drag (per frame velocity multiplier, reducing it will make particles slow down over time)
		float restitution = 0.98f; // if the particles have collision enabled, then after collision this is a multiplier for their bouncing velocities

		float SPH_h = 1.0f;		// smoothing radius
		float SPH_K = 250.0f;	// pressure constant
		float SPH_p0 = 1.0f;	// reference density
		float SPH_e = 0.018f;	// viscosity constant

		// Sprite sheet properties:
		uint32_t framesX = 1;
		uint32_t framesY = 1;
		uint32_t frameCount = 1;
		uint32_t frameStart = 0;
		float frameRate = 0; // frames per second

		void SetMaxParticleCount(uint32_t value);
		uint32_t GetMaxParticleCount() const { return MAX_PARTICLES; }
		uint64_t GetMemorySizeInBytes() const;

		bool IsInactive() const { return active_frames == 0; }

		// Non-serialized attributes:
		XMFLOAT3 center;
		uint32_t layerMask = ~0u;
		XMFLOAT4X4 worldMatrix = lb::math::IDENTITY_MATRIX;

		inline bool IsDebug() const { return _flags & FLAG_DEBUG; }
		inline bool IsPaused() const { return _flags & FLAG_PAUSED; }
		inline bool IsSorted() const { return _flags & FLAG_SORTING; }
		inline bool IsDepthCollisionEnabled() const { return _flags & FLAG_DEPTHCOLLISION; }
		inline bool IsSPHEnabled() const { return _flags & FLAG_SPH_FLUIDSIMULATION; }
		inline bool IsVolumeEnabled() const { return _flags & FLAG_HAS_VOLUME; }
		inline bool IsFrameBlendingEnabled() const { return _flags & FLAG_FRAME_BLENDING; }
		inline bool IsCollidersDisabled() const { return _flags & FLAG_COLLIDERS_DISABLED; }
		inline bool IsTakeColorFromMesh() const { return _flags & FLAG_TAKE_COLOR_FROM_MESH; }

		inline void SetDebug(bool value) { if (value) { _flags |= FLAG_DEBUG; } else { _flags &= ~FLAG_DEBUG; } }
		inline void SetPaused(bool value) { if (value) { _flags |= FLAG_PAUSED; } else { _flags &= ~FLAG_PAUSED; } }
		inline void SetSorted(bool value) { if (value) { _flags |= FLAG_SORTING; } else { _flags &= ~FLAG_SORTING; } }
		inline void SetDepthCollisionEnabled(bool value) { if (value) { _flags |= FLAG_DEPTHCOLLISION; } else { _flags &= ~FLAG_DEPTHCOLLISION; } }
		inline void SetSPHEnabled(bool value) { if (value) { _flags |= FLAG_SPH_FLUIDSIMULATION; } else { _flags &= ~FLAG_SPH_FLUIDSIMULATION; } }
		inline void SetVolumeEnabled(bool value) { if (value) { _flags |= FLAG_HAS_VOLUME; } else { _flags &= ~FLAG_HAS_VOLUME; } }
		inline void SetFrameBlendingEnabled(bool value) { if (value) { _flags |= FLAG_FRAME_BLENDING; } else { _flags &= ~FLAG_FRAME_BLENDING; } }
		inline void SetCollidersDisabled(bool value) { if (value) { _flags |= FLAG_COLLIDERS_DISABLED; } else { _flags &= ~FLAG_COLLIDERS_DISABLED; } }
		inline void SetTakeColorFromMesh(bool value) { if (value) { _flags |= FLAG_TAKE_COLOR_FROM_MESH; } else { _flags &= ~FLAG_TAKE_COLOR_FROM_MESH; } }

		// Set the opacity curve parameters
		//	peak : start peak of the opacity relative to particle lifetime [0,1]
		void SetOpacityCurveControl(float peakStart, float peakEnd);
		const lb::graphics::Texture* GetOpacityCurveTex() const { return &opacityCurveTex; }

		void Serialize(lb::Archive& archive, lb::ecs::EntitySerializer& seri);

		static void Initialize();
	};

}

