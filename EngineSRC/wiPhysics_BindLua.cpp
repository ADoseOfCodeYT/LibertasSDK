#include "wiPhysics_BindLua.h"
#include "wiPhysics.h"
#include "wiScene_BindLua.h"
#include "wiMath_BindLua.h"
#include "wiPrimitive_BindLua.h"

namespace lb::lua
{
	Luna<Physics_BindLua>::FunctionType Physics_BindLua::methods[] = {
		lunamethod(Physics_BindLua, SetEnabled),
		lunamethod(Physics_BindLua, IsEnabled),
		lunamethod(Physics_BindLua, SetSimulationEnabled),
		lunamethod(Physics_BindLua, IsSimulationEnabled),
		lunamethod(Physics_BindLua, SetInterpolationEnabled),
		lunamethod(Physics_BindLua, IsInterpolationEnabled),
		lunamethod(Physics_BindLua, SetDebugDrawEnabled),
		lunamethod(Physics_BindLua, IsDebugDrawEnabled),
		lunamethod(Physics_BindLua, SetAccuracy),
		lunamethod(Physics_BindLua, GetAccuracy),
		lunamethod(Physics_BindLua, SetFrameRate),
		lunamethod(Physics_BindLua, GetFrameRate),
		lunamethod(Physics_BindLua, SetLinearVelocity),
		lunamethod(Physics_BindLua, SetAngularVelocity),
		lunamethod(Physics_BindLua, ApplyForceAt),
		lunamethod(Physics_BindLua, ApplyForce),
		lunamethod(Physics_BindLua, ApplyForceAt),
		lunamethod(Physics_BindLua, ApplyImpulse),
		lunamethod(Physics_BindLua, ApplyImpulseAt),
		lunamethod(Physics_BindLua, ApplyTorque),
		lunamethod(Physics_BindLua, SetActivationState),
		lunamethod(Physics_BindLua, ActivateAllRigidBodies),
		lunamethod(Physics_BindLua, Intersects),
		lunamethod(Physics_BindLua, PickDrag),
		{ NULL, NULL }
	};
	Luna<Physics_BindLua>::PropertyType Physics_BindLua::properties[] = {
		{ NULL, NULL }
	};


	int Physics_BindLua::SetEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::physics::SetEnabled(lb::lua::SGetBool(L, 1));
		}
		else
			lb::lua::SError(L, "SetEnabled(bool value) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::IsEnabled(lua_State* L)
	{
		lb::lua::SSetBool(L, lb::physics::IsEnabled());
		return 1;
	}
	int Physics_BindLua::SetSimulationEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::physics::SetSimulationEnabled(lb::lua::SGetBool(L, 1));
		}
		else
			lb::lua::SError(L, "SetSimulationEnabled(bool value) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::IsSimulationEnabled(lua_State* L)
	{
		lb::lua::SSetBool(L, lb::physics::IsSimulationEnabled());
		return 1;
	}
	int Physics_BindLua::SetInterpolationEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::physics::SetInterpolationEnabled(lb::lua::SGetBool(L, 1));
		}
		else
			lb::lua::SError(L, "SetInterpolationEnabled(bool value) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::IsInterpolationEnabled(lua_State* L)
	{
		lb::lua::SSetBool(L, lb::physics::IsInterpolationEnabled());
		return 1;
	}
	int Physics_BindLua::SetDebugDrawEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::physics::SetDebugDrawEnabled(lb::lua::SGetBool(L, 1));
		}
		else
			lb::lua::SError(L, "SetDebugDrawEnabled(bool value) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::IsDebugDrawEnabled(lua_State* L)
	{
		lb::lua::SSetBool(L, lb::physics::IsDebugDrawEnabled());
		return 1;
	}
	int Physics_BindLua::SetAccuracy(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::physics::SetAccuracy(lb::lua::SGetInt(L, 1));
		}
		else
			lb::lua::SError(L, "SetAccuracy(int value) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::GetAccuracy(lua_State* L)
	{
		lb::lua::SSetInt(L, lb::physics::GetAccuracy());
		return 1;
	}
	int Physics_BindLua::SetFrameRate(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::physics::SetFrameRate(lb::lua::SGetFloat(L, 1));
		}
		else
			lb::lua::SError(L, "SetFrameRate(float value) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::GetFrameRate(lua_State* L)
	{
		lb::lua::SSetFloat(L, lb::physics::GetFrameRate());
		return 1;
	}

	int Physics_BindLua::SetLinearVelocity(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			scene::RigidBodyPhysicsComponent_BindLua* component = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (component == nullptr)
			{
				lb::lua::SError(L, "SetLinearVelocity(RigidBodyPhysicsComponent component, Vector velocity) first argument is not a RigidBodyPhysicsComponent!");
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "SetLinearVelocity(RigidBodyPhysicsComponent component, Vector velocity) second argument is not a Vector!");
				return 0;
			}
			lb::physics::SetLinearVelocity(
				*component->component,
				*(XMFLOAT3*)vec
			);
		}
		else
			lb::lua::SError(L, "SetLinearVelocity(RigidBodyPhysicsComponent component, Vector velocity) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::SetAngularVelocity(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			scene::RigidBodyPhysicsComponent_BindLua* component = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (component == nullptr)
			{
				lb::lua::SError(L, "SetAngularVelocity(RigidBodyPhysicsComponent component, Vector velocity) first argument is not a RigidBodyPhysicsComponent!");
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "SetAngularVelocity(RigidBodyPhysicsComponent component, Vector velocity) second argument is not a Vector!");
				return 0;
			}
			lb::physics::SetAngularVelocity(
				*component->component,
				*(XMFLOAT3*)vec
			);
		}
		else
			lb::lua::SError(L, "SetAngularVelocity(RigidBodyPhysicsComponent component, Vector velocity) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::ApplyForce(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			scene::RigidBodyPhysicsComponent_BindLua* component = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (component == nullptr)
			{
				lb::lua::SError(L, "ApplyForce(RigidBodyPhysicsComponent component, Vector force) first argument is not a RigidBodyPhysicsComponent!");
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "ApplyForce(RigidBodyPhysicsComponent component, Vector force) second argument is not a Vector!");
				return 0;
			}
			lb::physics::ApplyForce(
				*component->component,
				*(XMFLOAT3*)vec
			);
		}
		else
			lb::lua::SError(L, "ApplyForce(RigidBodyPhysicsComponent component, Vector force) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::ApplyForceAt(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 2)
		{
			scene::RigidBodyPhysicsComponent_BindLua* component = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (component == nullptr)
			{
				lb::lua::SError(L, "ApplyForceAt(RigidBodyPhysicsComponent component, Vector force, Vector at) first argument is not a RigidBodyPhysicsComponent!");
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "ApplyForceAt(RigidBodyPhysicsComponent component, Vector force, Vector at) second argument is not a Vector!");
				return 0;
			}
			Vector_BindLua* vec2 = Luna<Vector_BindLua>::lightcheck(L, 3);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "ApplyForceAt(RigidBodyPhysicsComponent component, Vector force, Vector at) third argument is not a Vector!");
				return 0;
			}
			bool at_local = true;
			if (argc > 3)
				at_local = lb::lua::SGetBool(L, 4);

			lb::physics::ApplyForceAt(
				*component->component,
				*(XMFLOAT3*)vec,
				*(XMFLOAT3*)vec2,
				at_local
			);
		}
		else
			lb::lua::SError(L, "ApplyForceAt(RigidBodyPhysicsComponent component, Vector force, Vector at) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::ApplyImpulse(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			scene::RigidBodyPhysicsComponent_BindLua* component = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (component == nullptr)
			{
				scene::HumanoidComponent_BindLua* humanoid = Luna<scene::HumanoidComponent_BindLua>::lightcheck(L, 1);
				if (humanoid == nullptr)
				{
					lb::lua::SError(L, "ApplyImpulse(RigidBodyPhysicsComponent component, Vector impulse) first argument is not a RigidBodyPhysicsComponent!");
					lb::lua::SError(L, "ApplyImpulse(HumanoidComponent component, HumanoidBone bone, Vector impulse) first argument is not a HumanoidComponent!");
					return 0;
				}
				lb::scene::HumanoidComponent::HumanoidBone bone = (lb::scene::HumanoidComponent::HumanoidBone)lb::lua::SGetInt(L, 2);
				Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 3);
				if (vec == nullptr)
				{
					lb::lua::SError(L, "ApplyImpulse(HumanoidComponent component, HumanoidBone bone, Vector impulse) third argument is not a Vector!");
					return 0;
				}
				lb::physics::ApplyImpulse(
					*humanoid->component,
					bone,
					*(XMFLOAT3*)vec
				);
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "ApplyImpulse(RigidBodyPhysicsComponent component, Vector impulse) second argument is not a Vector!");
				return 0;
			}
			lb::physics::ApplyImpulse(
				*component->component,
				*(XMFLOAT3*)vec
			);
		}
		else
			lb::lua::SError(L, "ApplyImpulse(RigidBodyPhysicsComponent component, Vector impulse) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::ApplyImpulseAt(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 2)
		{
			scene::RigidBodyPhysicsComponent_BindLua* component = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (component == nullptr)
			{
				scene::HumanoidComponent_BindLua* humanoid = Luna<scene::HumanoidComponent_BindLua>::lightcheck(L, 1);
				if (humanoid == nullptr)
				{
					lb::lua::SError(L, "ApplyImpulseAt(RigidBodyPhysicsComponent component, Vector impulse, Vector at) first argument is not a RigidBodyPhysicsComponent!");
					lb::lua::SError(L, "ApplyImpulseAt(HumanoidComponent component, HumanoidBone bone, Vector impulse, Vector at) first argument is not a HumanoidComponent!");
					return 0;
				}
				lb::scene::HumanoidComponent::HumanoidBone bone = (lb::scene::HumanoidComponent::HumanoidBone)lb::lua::SGetInt(L, 2);
				Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 3);
				if (vec == nullptr)
				{
					lb::lua::SError(L, "ApplyImpulseAt(HumanoidComponent component, HumanoidBone bone, Vector impulse, Vector at) third argument is not a Vector!");
					return 0;
				}
				Vector_BindLua* vec2 = Luna<Vector_BindLua>::lightcheck(L, 4);
				if (vec2 == nullptr)
				{
					lb::lua::SError(L, "ApplyImpulseAt(HumanoidComponent component, HumanoidBone bone, Vector impulse, Vector at) fourth argument is not a Vector!");
					return 0;
				}
				bool at_local = true;
				if (argc > 4)
					at_local = lb::lua::SGetBool(L, 5);

				lb::physics::ApplyImpulseAt(
					*humanoid->component,
					bone,
					*(XMFLOAT3*)vec,
					*(XMFLOAT3*)vec2,
					at_local
				);
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "ApplyImpulseAt(RigidBodyPhysicsComponent component, Vector impulse, Vector at) second argument is not a Vector!");
				return 0;
			}
			Vector_BindLua* vec2 = Luna<Vector_BindLua>::lightcheck(L, 3);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "ApplyImpulseAt(RigidBodyPhysicsComponent component, Vector impulse, Vector at) third argument is not a Vector!");
				return 0;
			}
			bool at_local = true;
			if (argc > 3)
				at_local = lb::lua::SGetBool(L, 4);

			lb::physics::ApplyImpulseAt(
				*component->component,
				*(XMFLOAT3*)vec,
				*(XMFLOAT3*)vec2,
				at_local
			);
		}
		else
			lb::lua::SError(L, "ApplyImpulseAt(RigidBodyPhysicsComponent component, Vector impulse, Vector at) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::ApplyTorque(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			scene::RigidBodyPhysicsComponent_BindLua* component = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (component == nullptr)
			{
				lb::lua::SError(L, "ApplyTorque(RigidBodyPhysicsComponent component, Vector torque) first argument is not a RigidBodyPhysicsComponent!");
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "ApplyTorque(RigidBodyPhysicsComponent component, Vector torque) second argument is not a Vector!");
				return 0;
			}
			lb::physics::ApplyTorque(
				*component->component,
				*(XMFLOAT3*)vec
			);
		}
		else
			lb::lua::SError(L, "ApplyTorque(RigidBodyPhysicsComponent component, Vector torque) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::SetActivationState(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			scene::RigidBodyPhysicsComponent_BindLua* rigid = Luna<scene::RigidBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
			if (rigid == nullptr)
			{
				scene::SoftBodyPhysicsComponent_BindLua* soft = Luna<scene::SoftBodyPhysicsComponent_BindLua>::lightcheck(L, 1);
				if (soft == nullptr)
				{
					lb::lua::SError(L, "SetActivationState(RigidBodyPhysicsComponent | SoftBodyPhysicsComponent component, int state) first argument is not a RigidBodyPhysicsComponent or SoftBodyPhysicsComponent!");
					return 0;
				}
				lb::physics::SetActivationState(
					*soft->component,
					(lb::physics::ActivationState)lb::lua::SGetInt(L, 2)
				);
				return 0;
			}
			lb::physics::SetActivationState(
				*rigid->component,
				(lb::physics::ActivationState)lb::lua::SGetInt(L, 2)
			);
		}
		else
			lb::lua::SError(L, "SetActivationState(RigidBodyPhysicsComponent | SoftBodyPhysicsComponent component, int state) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::ActivateAllRigidBodies(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			scene::Scene_BindLua* scene = Luna<scene::Scene_BindLua>::lightcheck(L, 1);
			if (scene == nullptr)
			{
				lb::lua::SError(L, "ActivateAllRigidBodies(Scene scene) first argument is not a Scene!");
				return 0;
			}
			lb::physics::ActivateAllRigidBodies(*scene->scene);
		}
		else
			lb::lua::SError(L, "ActivateAllRigidBodies(Scene scene) not enough arguments!");
		return 0;
	}

	int Physics_BindLua::Intersects(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			scene::Scene_BindLua* scene = Luna<scene::Scene_BindLua>::lightcheck(L, 1);
			if (scene == nullptr)
			{
				lb::lua::SError(L, "Intersects(Scene, Ray) first argument is not a Scene!");
				return 0;
			}
			primitive::Ray_BindLua* ray = Luna<primitive::Ray_BindLua>::lightcheck(L, 2);
			if (ray == nullptr)
			{
				lb::lua::SError(L, "Intersects(Scene, Ray) second argument is not a Ray!");
				return 0;
			}

			lb::physics::RayIntersectionResult result = lb::physics::Intersects(*scene->scene, ray->ray);
			lb::lua::SSetLongLong(L, result.entity);
			Luna<Vector_BindLua>::push(L, result.position);
			Luna<Vector_BindLua>::push(L, result.normal);
			lb::lua::SSetLongLong(L, result.humanoid_ragdoll_entity);
			lb::lua::SSetInt(L, (int)result.humanoid_bone);
			Luna<Vector_BindLua>::push(L, result.position_local);
			return 6;
		}
		lb::lua::SError(L, "Intersects(Scene, Ray) not enough arguments!");
		return 0;
	}
	int Physics_BindLua::PickDrag(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 2)
		{
			scene::Scene_BindLua* scene = Luna<scene::Scene_BindLua>::lightcheck(L, 1);
			if (scene == nullptr)
			{
				lb::lua::SError(L, "Intersects(Scene, Ray, PickDragOperation) first argument is not a Scene!");
				return 0;
			}
			primitive::Ray_BindLua* ray = Luna<primitive::Ray_BindLua>::lightcheck(L, 2);
			if (ray == nullptr)
			{
				lb::lua::SError(L, "Intersects(Scene, Ray, PickDragOperation) second argument is not a Ray!");
				return 0;
			}
			PickDragOperation_BindLua* op = Luna<PickDragOperation_BindLua>::lightcheck(L, 3);
			if (op == nullptr)
			{
				lb::lua::SError(L, "Intersects(Scene, Ray, PickDragOperation) third argument is not a PickDragOperation!");
				return 0;
			}

			lb::physics::PickDrag(*scene->scene, ray->ray, op->op);
			return 0;
		}
		lb::lua::SError(L, "Intersects(Scene, Ray, PickDragOperation) not enough arguments!");
		return 0;
	}

	void Physics_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<PickDragOperation_BindLua>::Register(lb::lua::GetLuaState());
			Luna<Physics_BindLua>::Register(lb::lua::GetLuaState());
			Luna<Physics_BindLua>::push_global(lb::lua::GetLuaState(), "physics");

			lb::lua::RunText(R"(
ACTIVATION_STATE_ACTIVE = 0
ACTIVATION_STATE_INACTIVE = 1
)");
		}
	}




	Luna<PickDragOperation_BindLua>::FunctionType PickDragOperation_BindLua::methods[] = {
		lunamethod(PickDragOperation_BindLua, Finish),
		{ NULL, NULL }
	};
	Luna<PickDragOperation_BindLua>::PropertyType PickDragOperation_BindLua::properties[] = {
		{ NULL, NULL }
	};

	int PickDragOperation_BindLua::Finish(lua_State* L)
	{
		op = {};
		return 0;
	}
}
