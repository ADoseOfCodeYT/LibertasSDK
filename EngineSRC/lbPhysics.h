#pragma once
#include "lbECS.h"
#include "lbScene.h"
#include "lbJobSystem.h"
#include "lbPrimitive.h"

#include <memory>

namespace lb::physics
{
	// Initializes the physics engine
	void Initialize();

	// Enable/disable the physics engine all together
	void SetEnabled(bool value);
	bool IsEnabled();

	// Enable/disable the physics simulation.
	//	Physics engine state will be updated but not simulated
	void SetSimulationEnabled(bool value);
	bool IsSimulationEnabled();

	// Enable/disable the physics interpolation.
	//	When enabled, physics simulation's fixed frame rate will be
	//	interpolated to match the variable frame rate of rendering
	void SetInterpolationEnabled(bool value);
	bool IsInterpolationEnabled();

	// Enable/disable debug drawing of physics objects
	void SetDebugDrawEnabled(bool value);
	bool IsDebugDrawEnabled();

	// Set the accuracy of the simulation
	//	This value corresponds to maximum simulation step count
	//	Higher values will be slower but more accurate
	void SetAccuracy(int value);
	int GetAccuracy();

	// Set frames per second value of physics simulation (default = 120 FPS)
	void SetFrameRate(float value);
	float GetFrameRate();

	// Update the physics state, run simulation, etc.
	void RunPhysicsUpdateSystem(
		lb::jobsystem::context& ctx,
		lb::scene::Scene& scene,
		float dt
	);

	// Set linear velocity to rigid body
	void SetLinearVelocity(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		const XMFLOAT3& velocity
	);
	// Set angular velocity to rigid body
	void SetAngularVelocity(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		const XMFLOAT3& velocity
	);

	// Apply force at body center
	void ApplyForce(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		const XMFLOAT3& force
	);
	// Apply force at position
	void ApplyForceAt(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		const XMFLOAT3& force,
		const XMFLOAT3& at,
		bool at_local = true // whether at is in local space of the body or not (global)
	);

	// Apply impulse at body center
	void ApplyImpulse(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		const XMFLOAT3& impulse
	);
	void ApplyImpulse(
		lb::scene::HumanoidComponent& humanoid,
		lb::scene::HumanoidComponent::HumanoidBone bone,
		const XMFLOAT3& impulse
	);
	// Apply impulse at position
	void ApplyImpulseAt(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		const XMFLOAT3& impulse,
		const XMFLOAT3& at,
		bool at_local = true // whether at is in local space of the body or not (global)
	);
	void ApplyImpulseAt(
		lb::scene::HumanoidComponent& humanoid,
		lb::scene::HumanoidComponent::HumanoidBone bone,
		const XMFLOAT3& impulse,
		const XMFLOAT3& at,
		bool at_local = true // whether at is in local space of the body or not (global)
	);

	void ApplyTorque(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		const XMFLOAT3& torque
	);

	enum class ActivationState
	{
		Active,
		Inactive,
	};
	void SetActivationState(
		lb::scene::RigidBodyPhysicsComponent& physicscomponent,
		ActivationState state
	);
	void SetActivationState(
		lb::scene::SoftBodyPhysicsComponent& physicscomponent,
		ActivationState state
	);
	void ActivateAllRigidBodies(lb::scene::Scene& scene);

	XMFLOAT3 GetSoftBodyNodePosition(
		lb::scene::SoftBodyPhysicsComponent& physicscomponent,
		uint32_t physicsIndex
	);

	struct RayIntersectionResult
	{
		lb::ecs::Entity entity = lb::ecs::INVALID_ENTITY;
		XMFLOAT3 position = XMFLOAT3(0, 0, 0);
		XMFLOAT3 position_local = XMFLOAT3(0, 0, 0);
		XMFLOAT3 normal = XMFLOAT3(0, 0, 0);
		lb::ecs::Entity humanoid_ragdoll_entity = lb::ecs::INVALID_ENTITY;
		lb::scene::HumanoidComponent::HumanoidBone humanoid_bone = lb::scene::HumanoidComponent::HumanoidBone::Count;
		int softbody_triangleID = -1;
		const void* physicsobject = nullptr;
		constexpr bool IsValid() const { return entity != lb::ecs::INVALID_ENTITY; }
	};
	RayIntersectionResult Intersects(
		const lb::scene::Scene& scene,
		lb::primitive::Ray ray
	);

	struct PickDragOperation
	{
		std::shared_ptr<void> internal_state;
		inline bool IsValid() const { return internal_state != nullptr; }
	};
	void PickDrag(
		const lb::scene::Scene& scene,
		lb::primitive::Ray ray,
		PickDragOperation& op
	);
}
