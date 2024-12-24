#pragma once
#include "CommonInclude.h"
#include "lbGraphicsDevice.h"
#include "lbEnums.h"
#include "lbMath.h"
#include "lbECS.h"
#include "lbPrimitive.h"
#include "lbVector.h"
#include "lbScene_Decl.h"
#include "lbScene_Components.h"

namespace lb
{
	class Archive;
}

namespace lb
{
	class HairParticleSystem
	{
	public:
		lb::graphics::GPUBuffer constantBuffer;
		lb::graphics::GPUBuffer generalBuffer;
		lb::scene::MeshComponent::BufferView simulation_view;
		lb::scene::MeshComponent::BufferView vb_pos[2];
		lb::scene::MeshComponent::BufferView vb_nor;
		lb::scene::MeshComponent::BufferView vb_pos_raytracing;
		lb::scene::MeshComponent::BufferView vb_uvs;
		lb::scene::MeshComponent::BufferView wetmap;
		lb::scene::MeshComponent::BufferView ib_culled;
		lb::scene::MeshComponent::BufferView indirect_view;
		lb::graphics::GPUBuffer primitiveBuffer;

		lb::graphics::GPUBuffer indexBuffer;
		lb::graphics::GPUBuffer vertexBuffer_length;

		lb::graphics::RaytracingAccelerationStructure BLAS;

		void CreateFromMesh(const lb::scene::MeshComponent& mesh);
		void CreateRenderData();
		void CreateRaytracingRenderData();

		void UpdateCPU(
			const lb::scene::TransformComponent& transform,
			const lb::scene::MeshComponent& mesh,
			float dt
		);

		struct UpdateGPUItem
		{
			const HairParticleSystem* hair = nullptr;
			uint32_t instanceIndex = 0;
			const lb::scene::MeshComponent* mesh = nullptr;
			const lb::scene::MaterialComponent* material = nullptr;
		};
		// Update a batch of hair particles by GPU
		static void UpdateGPU(
			const UpdateGPUItem* items,
			uint32_t itemCount,
			lb::graphics::CommandList cmd
		);

		mutable bool gpu_initialized = false;
		void InitializeGPUDataIfNeeded(lb::graphics::CommandList cmd);

		void Draw(
			const lb::scene::MaterialComponent& material,
			lb::enums::RENDERPASS renderPass,
			lb::graphics::CommandList cmd
		) const;

		enum FLAGS
		{
			EMPTY = 0,
			_DEPRECATED_REGENERATE_FRAME = 1 << 0,
			REBUILD_BUFFERS = 1 << 1,
			DIRTY = 1 << 2,
		};
		uint32_t _flags = EMPTY;

		lb::ecs::Entity meshID = lb::ecs::INVALID_ENTITY;

		uint32_t strandCount = 0;
		uint32_t segmentCount = 1;
		uint32_t randomSeed = 1;
		float length = 1.0f;
		float stiffness = 10.0f;
		float randomness = 0.2f;
		float viewDistance = 200;
		lb::vector<float> vertex_lengths;
		float width = 1;
		float uniformity = 1;

		struct AtlasRect
		{
			XMFLOAT4 texMulAdd = XMFLOAT4(1, 1, 0, 0);
			float size = 1;
		};
		lb::vector<AtlasRect> atlas_rects;

		// Non-serialized attributes:
		XMFLOAT4X4 world;
		lb::primitive::AABB aabb;
		lb::vector<uint32_t> indices; // it is dependent on vertex_lengths and contains triangles with non-zero lengths
		uint32_t layerMask = ~0u;
		mutable bool regenerate_frame = true;
		lb::graphics::Format position_format = lb::graphics::Format::R16G16B16A16_UNORM;
		mutable bool must_rebuild_blas = true;

		void Serialize(lb::Archive& archive, lb::ecs::EntitySerializer& seri);

		static void Initialize();

		constexpr uint32_t GetParticleCount() const { return strandCount * segmentCount; }
		uint64_t GetMemorySizeInBytes() const;

		constexpr bool IsDirty() const { return _flags & DIRTY; }
		constexpr void SetDirty(bool value = true) { if (value) { _flags |= DIRTY; } else { _flags &= ~DIRTY; } }

		void ConvertFromOLDSpriteSheet(uint32_t framesX, uint32_t framesY, uint32_t frameCount, uint32_t frameStart);
	};
}
