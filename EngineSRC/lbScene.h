#pragma once
#include "CommonInclude.h"
#include "lbJobSystem.h"
#include "lbSpinLock.h"
#include "lbGPUBVH.h"
#include "lbSprite.h"
#include "lbSpriteFont.h"
#include "lbMath.h"
#include "lbECS.h"
#include "lbScene_Components.h"
#include "lbEmittedParticle.h"
#include "lbHairParticle.h"
#include "lbTerrain.h"
#include "lbBVH.h"
#include "lbUnorderedSet.h"
#include "lbVoxelGrid.h"
#include "lbPathQuery.h"

#include <string>
#include <memory>
#include <limits>

namespace lb::scene
{
	struct Scene
	{
		virtual ~Scene() = default;

		lb::ecs::ComponentLibrary componentLibrary;

		lb::ecs::ComponentManager<NameComponent>& names = componentLibrary.Register<NameComponent>("lb::scene::Scene::names");
		lb::ecs::ComponentManager<LayerComponent>& layers = componentLibrary.Register<LayerComponent>("lb::scene::Scene::layers");
		lb::ecs::ComponentManager<TransformComponent>& transforms = componentLibrary.Register<TransformComponent>("lb::scene::Scene::transforms");
		lb::ecs::ComponentManager<HierarchyComponent>& hierarchy = componentLibrary.Register<HierarchyComponent>("lb::scene::Scene::hierarchy");
		lb::ecs::ComponentManager<MaterialComponent>& materials = componentLibrary.Register<MaterialComponent>("lb::scene::Scene::materials", 8); // version = 8
		lb::ecs::ComponentManager<MeshComponent>& meshes = componentLibrary.Register<MeshComponent>("lb::scene::Scene::meshes", 4); // version = 4
		lb::ecs::ComponentManager<ImpostorComponent>& impostors = componentLibrary.Register<ImpostorComponent>("lb::scene::Scene::impostors");
		lb::ecs::ComponentManager<ObjectComponent>& objects = componentLibrary.Register<ObjectComponent>("lb::scene::Scene::objects", 4); // version = 4
		lb::ecs::ComponentManager<RigidBodyPhysicsComponent>& rigidbodies = componentLibrary.Register<RigidBodyPhysicsComponent>("lb::scene::Scene::rigidbodies", 4); // version = 4
		lb::ecs::ComponentManager<SoftBodyPhysicsComponent>& softbodies = componentLibrary.Register<SoftBodyPhysicsComponent>("lb::scene::Scene::softbodies", 3); // version = 3
		lb::ecs::ComponentManager<ArmatureComponent>& armatures = componentLibrary.Register<ArmatureComponent>("lb::scene::Scene::armatures");
		lb::ecs::ComponentManager<LightComponent>& lights = componentLibrary.Register<LightComponent>("lb::scene::Scene::lights", 3); // version = 3
		lb::ecs::ComponentManager<CameraComponent>& cameras = componentLibrary.Register<CameraComponent>("lb::scene::Scene::cameras", 1); // version = 1
		lb::ecs::ComponentManager<EnvironmentProbeComponent>& probes = componentLibrary.Register<EnvironmentProbeComponent>("lb::scene::Scene::probes", 1); // version = 1
		lb::ecs::ComponentManager<ForceFieldComponent>& forces = componentLibrary.Register<ForceFieldComponent>("lb::scene::Scene::forces", 1); // version = 1
		lb::ecs::ComponentManager<DecalComponent>& decals = componentLibrary.Register<DecalComponent>("lb::scene::Scene::decals", 1); // version = 1
		lb::ecs::ComponentManager<AnimationComponent>& animations = componentLibrary.Register<AnimationComponent>("lb::scene::Scene::animations", 2); // version = 2
		lb::ecs::ComponentManager<AnimationDataComponent>& animation_datas = componentLibrary.Register<AnimationDataComponent>("lb::scene::Scene::animation_datas");
		lb::ecs::ComponentManager<EmittedParticleSystem>& emitters = componentLibrary.Register<EmittedParticleSystem>("lb::scene::Scene::emitters", 2); // version = 2
		lb::ecs::ComponentManager<HairParticleSystem>& hairs = componentLibrary.Register<HairParticleSystem>("lb::scene::Scene::hairs", 1); // version = 1
		lb::ecs::ComponentManager<WeatherComponent>& weathers = componentLibrary.Register<WeatherComponent>("lb::scene::Scene::weathers", 6); // version = 6
		lb::ecs::ComponentManager<SoundComponent>& sounds = componentLibrary.Register<SoundComponent>("lb::scene::Scene::sounds", 1); // version = 1
		lb::ecs::ComponentManager<VideoComponent>& videos = componentLibrary.Register<VideoComponent>("lb::scene::Scene::videos");
		lb::ecs::ComponentManager<InverseKinematicsComponent>& inverse_kinematics = componentLibrary.Register<InverseKinematicsComponent>("lb::scene::Scene::inverse_kinematics");
		lb::ecs::ComponentManager<SpringComponent>& springs = componentLibrary.Register<SpringComponent>("lb::scene::Scene::springs", 1); // version = 1
		lb::ecs::ComponentManager<ColliderComponent>& colliders = componentLibrary.Register<ColliderComponent>("lb::scene::Scene::colliders", 2); // version = 2
		lb::ecs::ComponentManager<ScriptComponent>& scripts = componentLibrary.Register<ScriptComponent>("lb::scene::Scene::scripts");
		lb::ecs::ComponentManager<ExpressionComponent>& expressions = componentLibrary.Register<ExpressionComponent>("lb::scene::Scene::expressions");
		lb::ecs::ComponentManager<HumanoidComponent>& humanoids = componentLibrary.Register<HumanoidComponent>("lb::scene::Scene::humanoids", 1); // version = 1
		lb::ecs::ComponentManager<lb::terrain::Terrain>& terrains = componentLibrary.Register<lb::terrain::Terrain>("lb::scene::Scene::terrains", 5); // version = 5
		lb::ecs::ComponentManager<lb::Sprite>& sprites = componentLibrary.Register<lb::Sprite>("lb::scene::Scene::sprites", 2); // version = 2
		lb::ecs::ComponentManager<lb::SpriteFont>& fonts = componentLibrary.Register<lb::SpriteFont>("lb::scene::Scene::fonts");
		lb::ecs::ComponentManager<lb::VoxelGrid>& voxel_grids = componentLibrary.Register<lb::VoxelGrid>("lb::scene::Scene::voxel_grids");
		lb::ecs::ComponentManager<MetadataComponent>& metadatas = componentLibrary.Register<MetadataComponent>("lb::scene::Scene::metadatas");
		lb::ecs::ComponentManager<CharacterComponent>& characters = componentLibrary.Register<CharacterComponent>("lb::scene::Scene::characters");

		// Non-serialized attributes:
		float dt = 0;
		enum FLAGS
		{
			EMPTY = 0,
		};
		uint32_t flags = EMPTY;

		float time = 0;
		CameraComponent camera; // for LOD and 3D sound update
		std::shared_ptr<void> physics_scene;
		lb::SpinLock locker;
		lb::primitive::AABB bounds;
		lb::vector<lb::primitive::AABB> parallel_bounds;
		WeatherComponent weather;
		lb::graphics::RaytracingAccelerationStructure TLAS;
		lb::graphics::GPUBuffer TLAS_instancesUpload[lb::graphics::GraphicsDevice::GetBufferCount()];
		void* TLAS_instancesMapped = nullptr;
		lb::GPUBVH BVH; // this is for non-hardware accelerated raytracing
		mutable bool acceleration_structure_update_requested = false;
		void SetAccelerationStructureUpdateRequested(bool value = true) { acceleration_structure_update_requested = value; }
		bool IsAccelerationStructureUpdateRequested() const { return acceleration_structure_update_requested; }
		bool IsLightmapUpdateRequested() const { return lightmap_request_allocator.load() > 0; }
		lb::Archive optimized_instatiation_data;
		lb::vector<lb::primitive::Capsule> character_capsules;

		// AABB culling streams:
		lb::vector<lb::primitive::AABB> aabb_objects;
		lb::vector<lb::primitive::AABB> aabb_lights;
		lb::vector<lb::primitive::AABB> aabb_probes;
		lb::vector<lb::primitive::AABB> aabb_decals;

		// Separate stream of world matrices:
		lb::vector<XMFLOAT4X4> matrix_objects;
		lb::vector<XMFLOAT4X4> matrix_objects_prev;

		// Shader visible scene parameters:
		ShaderScene shaderscene;

		// Instances for bindless visiblity indexing:
		//	contains in order:
		//		1) objects
		//		2) hair particles
		//		3) emitted particles
		//		4) impostors
		lb::graphics::GPUBuffer instanceUploadBuffer[lb::graphics::GraphicsDevice::GetBufferCount()];
		ShaderMeshInstance* instanceArrayMapped = nullptr;
		size_t instanceArraySize = 0;
		lb::graphics::GPUBuffer instanceBuffer;

		// Geometries for bindless visiblity indexing:
		//	contains in order:
		//		1) meshes * mesh.subsetCount
		//		2) hair particles * 1
		//		3) emitted particles * 1
		//		4) impostors * 1
		lb::graphics::GPUBuffer geometryUploadBuffer[lb::graphics::GraphicsDevice::GetBufferCount()];
		ShaderGeometry* geometryArrayMapped = nullptr;
		size_t geometryArraySize = 0;
		lb::graphics::GPUBuffer geometryBuffer;
		std::atomic<uint32_t> geometryAllocator{ 0 };

		// Materials for bindless visibility indexing:
		lb::graphics::GPUBuffer materialUploadBuffer[lb::graphics::GraphicsDevice::GetBufferCount()];
		ShaderMaterial* materialArrayMapped = nullptr;
		size_t materialArraySize = 0;
		lb::graphics::GPUBuffer materialBuffer;
		lb::graphics::GPUBuffer textureStreamingFeedbackBuffer;
		lb::graphics::GPUBuffer textureStreamingFeedbackBuffer_readback[lb::graphics::GraphicsDevice::GetBufferCount()];
		const uint32_t* textureStreamingFeedbackMapped = nullptr;

		// Meshlets:
		lb::graphics::GPUBuffer meshletBuffer;
		std::atomic<uint32_t> meshletAllocator{ 0 };

		// Skinning GPU data containining all bones, all morph descriptions:
		lb::graphics::GPUBuffer skinningUploadBuffer[lb::graphics::GraphicsDevice::GetBufferCount()];
		void* skinningDataMapped = nullptr;
		size_t skinningDataSize = 0;
		lb::graphics::GPUBuffer skinningBuffer;
		std::atomic<uint32_t> skinningAllocator{ 0 };

		// Occlusion query state:
		struct OcclusionResult
		{
			int occlusionQueries[lb::graphics::GraphicsDevice::GetBufferCount()];
			// occlusion result history bitfield (32 bit->32 frame history)
			uint32_t occlusionHistory = ~0u;

			constexpr bool IsOccluded() const
			{
				// Perform a conservative occlusion test:
				// If it is visible in any frames in the history, it is determined visible in this frame
				// But if all queries failed in the history, it is occluded.
				// If it pops up for a frame after occluded, it is visible again for some frames
				return occlusionHistory == 0;
			}
		};
		mutable lb::vector<OcclusionResult> occlusion_results_objects;
		lb::graphics::GPUQueryHeap queryHeap;
		lb::graphics::GPUBuffer queryResultBuffer[arraysize(OcclusionResult::occlusionQueries)];
		lb::graphics::GPUBuffer queryPredicationBuffer;
		uint32_t queryheap_idx = 0;
		mutable std::atomic<uint32_t> queryAllocator{ 0 };

		// DDGI resources:
		struct DDGI
		{
			uint frame_index = 0;
			uint3 grid_dimensions = uint3(32, 8, 32); // The scene extents will be subdivided into a grid of this resolution, each grid cell will have one probe
			float3 grid_min = float3(-1, -1, -1);
			float3 grid_max = float3(1, 1, 1);
			float smooth_backface = 0.01f; // smoothness of backface test
			lb::graphics::GPUBuffer ray_buffer;
			lb::graphics::GPUBuffer variance_buffer;
			lb::graphics::GPUBuffer raycount_buffer;
			lb::graphics::GPUBuffer rayallocation_buffer;
			lb::graphics::GPUBuffer sparse_tile_pool;
			lb::graphics::Texture color_texture;
			lb::graphics::Texture color_texture_rw; // alias of color_texture
			lb::graphics::Texture depth_texture;
			lb::graphics::Texture offset_texture;

			void Serialize(lb::Archive& archive);
		} ddgi;

		// Voxel GI resources:
		struct VXGI
		{
			uint32_t res = 64;
			float rayStepSize = 1;
			float maxDistance = 100.0f;
			struct ClipMap
			{
				float voxelsize = 0.125;
				XMFLOAT3 center = XMFLOAT3(0, 0, 0);
				XMINT3 offsetfromPrevFrame = XMINT3(0, 0, 0);
				XMFLOAT3 extents = XMFLOAT3(0, 0, 0);
			} clipmaps[VXGI_CLIPMAP_COUNT];
			uint32_t clipmap_to_update = 0;

			lb::graphics::Texture radiance;
			lb::graphics::Texture prev_radiance;
			lb::graphics::Texture render_atomic;
			lb::graphics::Texture sdf;
			lb::graphics::Texture sdf_temp;
			mutable bool pre_clear = true;
		} vxgi;

		EnvironmentProbeComponent global_dynamic_probe; // when no envprobes are placed, this will be the fallback

		// Impostor state:
		static constexpr uint32_t maxImpostorCount = 8;
		static constexpr uint32_t impostorTextureDim = 128;
		lb::graphics::Texture impostorDepthStencil;
		lb::graphics::Texture impostorRenderTarget_Albedo_MSAA;
		lb::graphics::Texture impostorRenderTarget_Normal_MSAA;
		lb::graphics::Texture impostorRenderTarget_Surface_MSAA;
		lb::graphics::Texture impostorRenderTarget_Albedo;
		lb::graphics::Texture impostorRenderTarget_Normal;
		lb::graphics::Texture impostorRenderTarget_Surface;
		lb::graphics::Texture impostorArray;
		lb::graphics::GPUBuffer impostorBuffer;
		uint32_t allocated_impostor_capacity = 0;
		MeshComponent::BufferView impostor_ib32;
		MeshComponent::BufferView impostor_ib16;
		MeshComponent::BufferView impostor_vb_pos;
		MeshComponent::BufferView impostor_vb_nor;
		MeshComponent::BufferView impostor_data;
		MeshComponent::BufferView impostor_indirect;
		lb::graphics::Format impostor_ib_format = lb::graphics::Format::R32_UINT;
		uint32_t impostorInstanceOffset = ~0u;
		uint32_t impostorGeometryOffset = ~0u;
		uint32_t impostorMaterialOffset = ~0u;

		lb::EmittedParticleSystem rainEmitter;
		MaterialComponent rainMaterial;
		uint32_t rainInstanceOffset = ~0u;
		uint32_t rainGeometryOffset = ~0u;
		uint32_t rainMaterialOffset = ~0u;
		LightComponent rain_blocker_dummy_light;

		std::atomic<uint32_t> lightmap_request_allocator{ 0 };
		lb::vector<uint32_t> lightmap_requests;
		lb::vector<TransformComponent> transforms_temp;

		// CPU/GPU Colliders:
		std::atomic<uint32_t> collider_allocator_cpu{ 0 };
		std::atomic<uint32_t> collider_allocator_gpu{ 0 };
		lb::vector<uint8_t> collider_deinterleaved_data;
		uint32_t collider_count_cpu = 0;
		uint32_t collider_count_gpu = 0;
		lb::primitive::AABB* aabb_colliders_cpu = nullptr;
		ColliderComponent* colliders_cpu = nullptr;
		ColliderComponent* colliders_gpu = nullptr;
		lb::BVH collider_bvh;

		// Ocean GPU state:
		lb::Ocean ocean;
		void OceanRegenerate() { ocean.Create(weather.oceanParameters); }

		// Simple water ripple sprites:
		mutable lb::vector<lb::Sprite> waterRipples;
		void PutWaterRipple(const XMFLOAT3& pos);
		void PutWaterRipple(const std::string& image, const XMFLOAT3& pos);

		lb::graphics::GPUBuffer voxelgrid_gpu; // primary CPU voxelgrid uploaded to GPU

		// Animation processing optimizer:
		struct AnimationQueue
		{
			// The animations within one queue must be processed on the same thread in order
			lb::vector<AnimationComponent*> animations; // pointers for one frame only!
			lb::unordered_set<lb::ecs::Entity> entities;
		};
		lb::vector<AnimationQueue> animation_queues; // different animation queues can be processed in different threads in any order
		size_t animation_queue_count = 0; // to avoid resizing animation queues downwards because the internals for them needs to be reallocated in that case
		lb::jobsystem::context animation_dependency_scan_workload;
		void ScanAnimationDependencies();

		lb::vector<SpringComponent*> spring_queues; // these indicate which chains can be updated on separate threads
		lb::jobsystem::context spring_dependency_scan_workload;
		void ScanSpringDependencies();
		void UpdateSpringsTopDownRecursive(SpringComponent* parent_spring, SpringComponent& spring);

		float wetmap_fadeout_time = 0;
		bool IsWetmapProcessingRequired() const;

		// Update all components by a given timestep (in seconds):
		//	This is an expensive function, prefer to call it only once per frame!
		virtual void Update(float dt);
		// Remove everything from the scene that it owns:
		virtual void Clear();
		// Merge an other scene into this.
		//	The contents of the other scene will be lost (and moved to this)!
		//  Any references to entities or components from the other scene will now reference them in this scene.
		virtual void Merge(Scene& other);
		// Similar to merge but skipping some things that are safe to skip within the Update look
		void MergeFastInternal(Scene& other);
		// Create a copy of prefab and merge it into this.
		//	prefab		: source scene to be copied from
		//	attached	: if true, everything from prefab will be attached to a root entity
		//	returns new root entity if attached is set to true, otherwise returns INVALID_ENTITY
		virtual lb::ecs::Entity Instantiate(Scene& prefab, bool attached = false);
		// Finds all entities in the scene that have any components attached
		void FindAllEntities(lb::unordered_set<lb::ecs::Entity>& entities) const;

		// Removes (deletes) a specific entity from the scene (if it exists):
		//	recursive	: also removes children if true
		//	keep_sorted	: remove all components while keeping sorted order (slow)
		void Entity_Remove(lb::ecs::Entity entity, bool recursive = true, bool keep_sorted = false);
		// Finds the first entity by the name (if it exists, otherwise returns INVALID_ENTITY):
		//	ancestor : you can specify an ancestor entity if you only want to find entities that are descendants of ancestor entity
		lb::ecs::Entity Entity_FindByName(const std::string& name, lb::ecs::Entity ancestor = lb::ecs::INVALID_ENTITY);
		// Duplicates all of an entity's components and creates a new entity with them (recursively keeps hierarchy):
		lb::ecs::Entity Entity_Duplicate(lb::ecs::Entity entity);
		// Check whether entity is a descendant of ancestor
		//	returns true if entity is in the hierarchy tree of ancestor, false otherwise
		bool Entity_IsDescendant(lb::ecs::Entity entity, lb::ecs::Entity ancestor) const;

		enum class EntitySerializeFlags
		{
			NONE = 0,
			RECURSIVE = 1 << 0, // children entities will be also serialized
			KEEP_INTERNAL_ENTITY_REFERENCES = 1 << 1, // entity handles inside components will be kept intact, they won't use remapping of lb::ecs::EntitySerializer
		};
		// Serializes entity and all of its components to archive:
		//	archive		: archive used for serializing data
		//	seri		: serializer state for entity component system
		//	entity		: if archive is in write mode, this is the entity to serialize. If archive is in read mode, it should be INVALID_ENTITY
		//	flags		: specify options as EntitySerializeFlags bits to control internal behaviour
		// 
		//	Returns either the new entity that was read, or the original entity that was written
		lb::ecs::Entity Entity_Serialize(
			lb::Archive& archive,
			lb::ecs::EntitySerializer& seri,
			lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY,
			EntitySerializeFlags flags = EntitySerializeFlags::RECURSIVE
		);

		lb::ecs::Entity Entity_CreateTransform(
			const std::string& name
		);
		lb::ecs::Entity Entity_CreateMaterial(
			const std::string& name
		);
		lb::ecs::Entity Entity_CreateObject(
			const std::string& name
		);
		lb::ecs::Entity Entity_CreateMesh(
			const std::string& name
		);
		lb::ecs::Entity Entity_CreateLight(
			const std::string& name, 
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0), 
			const XMFLOAT3& color = XMFLOAT3(1, 1, 1), 
			float intensity = 1, 
			float range = 10,
			LightComponent::LightType type = LightComponent::POINT,
			float outerConeAngle = XM_PIDIV4,
			float innerConeAngle = 0
		);
		lb::ecs::Entity Entity_CreateForce(
			const std::string& name,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0)
		);
		lb::ecs::Entity Entity_CreateEnvironmentProbe(
			const std::string& name,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0)
		);
		lb::ecs::Entity Entity_CreateDecal(
			const std::string& name,
			const std::string& textureName,
			const std::string& normalMapName = ""
		);
		lb::ecs::Entity Entity_CreateCamera(
			const std::string& name,
			float width, float height, float nearPlane = 0.01f, float farPlane = 1000.0f, float fov = XM_PIDIV4
		);
		lb::ecs::Entity Entity_CreateEmitter(
			const std::string& name,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0)
		);
		lb::ecs::Entity Entity_CreateHair(
			const std::string& name,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0)
		);
		lb::ecs::Entity Entity_CreateSound(
			const std::string& name,
			const std::string& filename,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0)
		);
		lb::ecs::Entity Entity_CreateVideo(
			const std::string& name,
			const std::string& filename
		);
		lb::ecs::Entity Entity_CreateCube(
			const std::string& name
		);
		lb::ecs::Entity Entity_CreatePlane(
			const std::string& name
		);
		lb::ecs::Entity Entity_CreateSphere(
			const std::string& name,
			float radius = 1,
			uint32_t latitudeBands = 64,
			uint32_t longitudeBands = 64
		);
		lb::ecs::Entity Entity_CreateMeshFromData(
			const std::string& name,
			size_t index_count,
			const uint32_t* indices,
			size_t vertex_count,
			const XMFLOAT3* positions
		);

		// Attaches an entity to a parent:
		//	child_already_in_local_space	:	child won't be transformed from world space to local space
		void Component_Attach(lb::ecs::Entity entity, lb::ecs::Entity parent, bool child_already_in_local_space = false);
		// Detaches the entity from its parent (if it is attached):
		void Component_Detach(lb::ecs::Entity entity);
		// Detaches all children from an entity (if there are any):
		void Component_DetachChildren(lb::ecs::Entity parent);

		// Read/write whole scene into an archive
		void Serialize(lb::Archive& archive);

		void RunAnimationUpdateSystem(lb::jobsystem::context& ctx);
		void RunTransformUpdateSystem(lb::jobsystem::context& ctx);
		void RunHierarchyUpdateSystem(lb::jobsystem::context& ctx);
		void RunExpressionUpdateSystem(lb::jobsystem::context& ctx);
		void RunProceduralAnimationUpdateSystem(lb::jobsystem::context& ctx);
		void RunArmatureUpdateSystem(lb::jobsystem::context& ctx);
		void RunMeshUpdateSystem(lb::jobsystem::context& ctx);
		void RunMaterialUpdateSystem(lb::jobsystem::context& ctx);
		void RunImpostorUpdateSystem(lb::jobsystem::context& ctx);
		void RunObjectUpdateSystem(lb::jobsystem::context& ctx);
		void RunCameraUpdateSystem(lb::jobsystem::context& ctx);
		void RunDecalUpdateSystem(lb::jobsystem::context& ctx);
		void RunProbeUpdateSystem(lb::jobsystem::context& ctx);
		void RunForceUpdateSystem(lb::jobsystem::context& ctx);
		void RunLightUpdateSystem(lb::jobsystem::context& ctx);
		void RunParticleUpdateSystem(lb::jobsystem::context& ctx);
		void RunWeatherUpdateSystem(lb::jobsystem::context& ctx);
		void RunSoundUpdateSystem(lb::jobsystem::context& ctx);
		void RunVideoUpdateSystem(lb::jobsystem::context& ctx);
		void RunScriptUpdateSystem(lb::jobsystem::context& ctx);
		void RunSpriteUpdateSystem(lb::jobsystem::context& ctx);
		void RunFontUpdateSystem(lb::jobsystem::context& ctx);
		void RunCharacterUpdateSystem(lb::jobsystem::context& ctx);


		struct RayIntersectionResult
		{
			lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
			XMFLOAT3 position = XMFLOAT3(0, 0, 0);
			XMFLOAT3 normal = XMFLOAT3(0, 0, 0);
			XMFLOAT4 uv = XMFLOAT4(0, 0, 0, 0);
			XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
			float distance = std::numeric_limits<float>::max();
			int subsetIndex = -1;
			int vertexID0 = -1;
			int vertexID1 = -1;
			int vertexID2 = -1;
			XMFLOAT2 bary = XMFLOAT2(0, 0);
			XMFLOAT4X4 orientation = lb::math::IDENTITY_MATRIX;
			HumanoidComponent::HumanoidBone humanoid_bone = HumanoidComponent::HumanoidBone::Count;

			constexpr bool operator==(const RayIntersectionResult& other) const
			{
				return entity == other.entity;
			}
		};
		// Given a ray, finds the closest intersection point against all mesh instances or collliders
		//	ray				:	the incoming ray that will be traced
		//	filterMask		:	filter based on type
		//	layerMask		:	filter based on layer
		//	lod				:	specify min level of detail for meshes
		RayIntersectionResult Intersects(const lb::primitive::Ray& ray, uint32_t filterMask = lb::enums::FILTER_OPAQUE, uint32_t layerMask = ~0, uint32_t lod = 0) const;

		// Given a ray, finds the first intersection point against all mesh instances or colliders
		//	returns true immediately if intersection was found, false otherwise
		//	ray				:	the incoming ray that will be traced
		//	filterMask		:	filter based on type
		//	layerMask		:	filter based on layer
		//	lod				:	specify min level of detail for meshes
		bool IntersectsFirst(const lb::primitive::Ray& ray, uint32_t filterMask = lb::enums::FILTER_OPAQUE, uint32_t layerMask = ~0, uint32_t lod = 0) const;

		struct SphereIntersectionResult
		{
			lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
			XMFLOAT3 position = XMFLOAT3(0, 0, 0);
			XMFLOAT3 normal = XMFLOAT3(0, 0, 0);
			XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
			float depth = 0;
			int subsetIndex = -1;
			XMFLOAT4X4 orientation = lb::math::IDENTITY_MATRIX;
			HumanoidComponent::HumanoidBone humanoid_bone = HumanoidComponent::HumanoidBone::Count;
		};
		SphereIntersectionResult Intersects(const lb::primitive::Sphere& sphere, uint32_t filterMask = lb::enums::FILTER_OPAQUE, uint32_t layerMask = ~0, uint32_t lod = 0) const;

		using CapsuleIntersectionResult = SphereIntersectionResult;
		CapsuleIntersectionResult Intersects(const lb::primitive::Capsule& capsule, uint32_t filterMask = lb::enums::FILTER_OPAQUE, uint32_t layerMask = ~0, uint32_t lod = 0) const;

		// Goes through the hierarchy backwards and computes parent's world space matrix:
		XMMATRIX ComputeParentMatrixRecursive(lb::ecs::Entity entity) const;

		// Retargets an animation from a Humanoid to an other Humanoid such that the new animation will play back on the destination humanoid
		//	dst			:	destination humanoid that the animation will be fit onto
		//	src			:	the animation to copy, it should already target humanoid bones
		//	bake_data	:	if true, the retargeted data will be baked into a new animation data.
		//					if false, it will reuse the source animation data without creating a new one and retargeting will be applied at runtime on every Update
		//	src_scene	:	(optional) specify if you want to retarget from an other scene. This scene must be kept alive while you use the resulting animation!
		//					If you use a separate scene, then you mustn't serialize the scene while there are animations referencing the src_scene!
		//
		//	returns entity ID of the new animation or INVALID_ENTITY if retargeting was not successful
		lb::ecs::Entity RetargetAnimation(lb::ecs::Entity dst, lb::ecs::Entity src, bool bake_data, const Scene* src_scene = nullptr);

		// If you don't know which armature the bone is contained int, this function can be used to find the first such armature and return the bone's rest matrix
		//	If not found, and entity has a transform, it returns transform matrix
		//	Otherwise, returns identity matrix
		XMMATRIX GetRestPose(lb::ecs::Entity entity) const;
		XMMATRIX FindBoneRestPose(lb::ecs::Entity bone) { return GetRestPose(bone); }; // back-compat of GetRestPose

		// Returns 1 if humanoid's default facing direction is forward, -1 if it's backward
		float GetHumanoidDefaultFacing(const HumanoidComponent& humanoid, lb::ecs::Entity humanoidEntity) const;

		// All triangles of the object will be injected into the voxel grid
		//	subtract: if false (default), voxels will be added, if true then voxels will be removed
		void VoxelizeObject(size_t objectIndex, lb::VoxelGrid& grid, bool subtract = false, uint32_t lod = 0);

		// Voxelize all meshes that match the filters into a voxel grid
		void VoxelizeScene(lb::VoxelGrid& voxelgrid, bool subtract = false, uint32_t filterMask = lb::enums::FILTER_ALL, uint32_t layerMask = ~0, uint32_t lod = 0);

		// Get the current position on the surface of an object, tracked by the triangle barycentrics
		XMFLOAT3 GetPositionOnSurface(lb::ecs::Entity objectEntity, int vertexID0, int vertexID1, int vertexID2, const XMFLOAT2& bary) const;

		// Resets pose of the specified entity to bind pose
		//	this will search for all armatures that are descendants of the entity and set all bone matrices to the their bind matrix
		void ResetPose(lb::ecs::Entity entity);

		// Returns the approximate position on the ocean surface seen from a position in world space.
		//	If current weather doesn't have ocean enabled, returns the world position itself.
		//	The result position is approximate because it involves reading back from GPU to the CPU, so the result can be delayed compared to the current GPU simulation.
		//	Note that the input position to this function will be taken on the XZ plane and modified by the displacement map's XZ value, and the Y (vertical) position will be taken from the ocean water height and displacement map only.
		XMFLOAT3 GetOceanPosAt(const XMFLOAT3& worldPosition) const;

	private:
		void UpdateHumanoidFacings();

	};

	// Returns skinned vertex position
	//	N : normal (out, optional)
	XMVECTOR SkinVertex(const MeshComponent& mesh, const lb::vector<ShaderTransform>& boneData, uint32_t index, XMVECTOR* N = nullptr);
	// Returns skinned vertex position in armature local space
	//	N : normal (out, optional)
	XMVECTOR SkinVertex(const MeshComponent& mesh, const ArmatureComponent& armature, uint32_t index, XMVECTOR* N = nullptr);
	// Returns skinned vertex position of soft body in world space
	//	N : normal (out, optional)
	XMVECTOR SkinVertex(const MeshComponent& mesh, const SoftBodyPhysicsComponent& softbody, uint32_t index, XMVECTOR* N = nullptr);


	// Helper that manages a global scene
	//	(You don't need to use it, but it's an option for simplicity)
	inline Scene& GetScene()
	{
		static Scene scene;
		return scene;
	}

	// Helper that manages a global camera
	//	(You don't need to use it, but it's an option for simplicity)
	inline CameraComponent& GetCamera()
	{
		static CameraComponent camera;
		return camera;
	}

	// Helper function to open a wiscene file and add the contents to the global scene
	//	fileName		:	file path
	//	transformMatrix	:	everything will be transformed by this matrix (optional)
	//	attached		:	if true, everything will be attached to a base entity
	//
	//	returns INVALID_ENTITY if attached argument was false, else it returns the base entity handle
	lb::ecs::Entity LoadModel(const std::string& fileName, const XMMATRIX& transformMatrix = XMMatrixIdentity(), bool attached = false);

	// Helper function to open a wiscene file and add the contents to the specified scene. This is thread safe as it doesn't modify global scene
	//	scene			:	the scene that will contain the model
	//	fileName		:	file path
	//	transformMatrix	:	everything will be transformed by this matrix (optional)
	//	attached		:	if true, everything will be attached to a base entity
	//
	//	returns INVALID_ENTITY if attached argument was false, else it returns the base entity handle
	lb::ecs::Entity LoadModel(Scene& scene, const std::string& fileName, const XMMATRIX& transformMatrix = XMMatrixIdentity(), bool attached = false);

	// Helper function to open a wiscene file and add the contents to the global scene
	//	fileName		:	file path
	//	transformMatrix	:	everything will be transformed by this matrix (optional)
	//	rootEntity		:	specify entity to attach whole scene to (optional)
	void LoadModel2(const std::string& fileName, const XMMATRIX& transformMatrix = XMMatrixIdentity(), lb::ecs::Entity rootEntity = lb::ecs::INVALID_ENTITY);

	// Helper function to open a wiscene file and add the contents to the specified scene. This is thread safe as it doesn't modify global scene
	//	scene			:	the scene that will contain the model
	//	fileName		:	file path
	//	transformMatrix	:	everything will be transformed by this matrix (optional)
	//	rootEntity		:	specify entity to attach whole scene to (optional)
	void LoadModel2(Scene& scene, const std::string& fileName, const XMMATRIX& transformMatrix = XMMatrixIdentity(), lb::ecs::Entity rootEntity = lb::ecs::INVALID_ENTITY);

	// Deprecated, use Scene::Intersects() function instead
	using PickResult = Scene::RayIntersectionResult;
	PickResult Pick(const lb::primitive::Ray& ray, uint32_t filterMask = lb::enums::FILTER_OPAQUE, uint32_t layerMask = ~0, const Scene& scene = GetScene(), uint32_t lod = 0);

	// Deprecated, use Scene::Intersects() function instead
	using SceneIntersectSphereResult = Scene::SphereIntersectionResult;
	SceneIntersectSphereResult SceneIntersectSphere(const lb::primitive::Sphere& sphere, uint32_t filterMask = lb::enums::FILTER_OPAQUE, uint32_t layerMask = ~0, const Scene& scene = GetScene(), uint32_t lod = 0);

	// Deprecated, use Scene::Intersects() function instead
	using SceneIntersectCapsuleResult = Scene::SphereIntersectionResult;
	SceneIntersectCapsuleResult SceneIntersectCapsule(const lb::primitive::Capsule& capsule, uint32_t filterMask = lb::enums::FILTER_OPAQUE, uint32_t layerMask = ~0, const Scene& scene = GetScene(), uint32_t lod = 0);

}

template<>
struct enable_bitmask_operators<lb::scene::Scene::EntitySerializeFlags> {
	static const bool enable = true;
};
