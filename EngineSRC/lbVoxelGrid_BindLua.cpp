#include "lbVoxelGrid_BindLua.h"
#include "lbMath_BindLua.h"
#include "lbPrimitive_BindLua.h"

namespace lb::lua
{
	Luna<VoxelGrid_BindLua>::FunctionType VoxelGrid_BindLua::methods[] = {
		lunamethod(VoxelGrid_BindLua, Init),
		lunamethod(VoxelGrid_BindLua, ClearData),
		lunamethod(VoxelGrid_BindLua, FromAABB),
		lunamethod(VoxelGrid_BindLua, InjectTriangle),
		lunamethod(VoxelGrid_BindLua, InjectAABB),
		lunamethod(VoxelGrid_BindLua, InjectSphere),
		lunamethod(VoxelGrid_BindLua, InjectCapsule),
		lunamethod(VoxelGrid_BindLua, WorldToCoord),
		lunamethod(VoxelGrid_BindLua, CoordToWorld),
		lunamethod(VoxelGrid_BindLua, CheckVoxel),
		lunamethod(VoxelGrid_BindLua, SetVoxel),
		lunamethod(VoxelGrid_BindLua, GetCenter),
		lunamethod(VoxelGrid_BindLua, SetCenter),
		lunamethod(VoxelGrid_BindLua, GetVoxelSize),
		lunamethod(VoxelGrid_BindLua, SetVoxelSize),
		lunamethod(VoxelGrid_BindLua, GetDebugColor),
		lunamethod(VoxelGrid_BindLua, SetDebugColor),
		lunamethod(VoxelGrid_BindLua, GetDebugColorExtent),
		lunamethod(VoxelGrid_BindLua, SetDebugColorExtent),
		lunamethod(VoxelGrid_BindLua, GetMemorySize),
		lunamethod(VoxelGrid_BindLua, Add),
		lunamethod(VoxelGrid_BindLua, Subtract),
		lunamethod(VoxelGrid_BindLua, IsVisible),
		lunamethod(VoxelGrid_BindLua, FloodFill),
		{ NULL, NULL }
	};
	Luna<VoxelGrid_BindLua>::PropertyType VoxelGrid_BindLua::properties[] = {
		{ NULL, NULL }
	};

	VoxelGrid_BindLua::VoxelGrid_BindLua(lua_State* L)
	{
		voxelgrid = &owning;
		int argc = lb::lua::SGetArgCount(L);
		if (argc >= 3)
		{
			Init(L);
		}
	}

	int VoxelGrid_BindLua::Init(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 3)
		{
			lb::lua::SError(L, "VoxelGrid::Init(int dimX,dimY,dimZ) not enough arguments!");
			return 0;
		}
		uint32_t x = (uint32_t)lb::lua::SGetInt(L, 1);
		uint32_t y = (uint32_t)lb::lua::SGetInt(L, 2);
		uint32_t z = (uint32_t)lb::lua::SGetInt(L, 3);
		voxelgrid->init(x, y, z);
		return 0;
	}
	int VoxelGrid_BindLua::ClearData(lua_State* L)
	{
		voxelgrid->cleardata();
		return 0;
	}
	int VoxelGrid_BindLua::FromAABB(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::FromAABB(AABB aabb) not enough arguments!");
			return 0;
		}
		primitive::AABB_BindLua* aabb = Luna<primitive::AABB_BindLua>::lightcheck(L, 1);
		if (aabb == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::FromAABB(AABB aabb) argument is not an AABB!");
			return 0;
		}
		voxelgrid->from_aabb(aabb->aabb);
		return 0;
	}
	int VoxelGrid_BindLua::InjectTriangle(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 3)
		{
			lb::lua::SError(L, "VoxelGrid::InjectTriangle(Vector a,b,c, opt bool subtract = false) not enough arguments!");
			return 0;
		}
		Vector_BindLua* a = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (a == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::InjectTriangle(Vector a,b,c, opt bool subtract = false) first argument is not a Vector!");
			return 0;
		}
		Vector_BindLua* b = Luna<Vector_BindLua>::lightcheck(L, 2);
		if (a == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::InjectTriangle(Vector a,b,c, opt bool subtract = false) second argument is not a Vector!");
			return 0;
		}
		Vector_BindLua* c = Luna<Vector_BindLua>::lightcheck(L, 3);
		if (a == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::InjectTriangle(Vector a,b,c, opt bool subtract = false) third argument is not a Vector!");
			return 0;
		}
		bool subtract = false;
		if (argc > 3)
		{
			subtract = lb::lua::SGetBool(L, 4);
		}
		voxelgrid->inject_triangle(XMLoadFloat4(&a->data), XMLoadFloat4(&b->data), XMLoadFloat4(&c->data), subtract);
		return 0;
	}
	int VoxelGrid_BindLua::InjectAABB(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::InjectAABB(AABB aabb, opt bool subtract = false) not enough arguments!");
			return 0;
		}
		primitive::AABB_BindLua* aabb = Luna<primitive::AABB_BindLua>::lightcheck(L, 1);
		if (aabb == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::InjectAABB(AABB aabb, opt bool subtract = false) first argument is not a Vector!");
			return 0;
		}
		bool subtract = false;
		if (argc > 1)
		{
			subtract = lb::lua::SGetBool(L, 2);
		}
		voxelgrid->inject_aabb(aabb->aabb, subtract);
		return 0;
	}
	int VoxelGrid_BindLua::InjectSphere(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::InjectSphere(Sphere sphere, opt bool subtract = false) not enough arguments!");
			return 0;
		}
		primitive::Sphere_BindLua* sphere = Luna<primitive::Sphere_BindLua>::lightcheck(L, 1);
		if (sphere == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::InjectSphere(Sphere sphere, opt bool subtract = false) first argument is not a Vector!");
			return 0;
		}
		bool subtract = false;
		if (argc > 1)
		{
			subtract = lb::lua::SGetBool(L, 2);
		}
		voxelgrid->inject_sphere(sphere->sphere, subtract);
		return 0;
	}
	int VoxelGrid_BindLua::InjectCapsule(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::InjectCapsule(Capsule capsule, opt bool subtract = false) not enough arguments!");
			return 0;
		}
		primitive::Capsule_BindLua* capsule = Luna<primitive::Capsule_BindLua>::lightcheck(L, 1);
		if (capsule == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::InjectCapsule(Capsule capsule, opt bool subtract = false) first argument is not a Vector!");
			return 0;
		}
		bool subtract = false;
		if (argc > 1)
		{
			subtract = lb::lua::SGetBool(L, 2);
		}
		voxelgrid->inject_capsule(capsule->capsule, subtract);
		return 0;
	}
	int VoxelGrid_BindLua::WorldToCoord(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::WorldToCoord(Vector pos) not enough arguments!");
			return 0;
		}
		Vector_BindLua* pos = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (pos == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::WorldToCoord(Vector pos) first argument is not a Vector!");
			return 0;
		}
		XMUINT3 coord = voxelgrid->world_to_coord(pos->GetFloat3());
		lb::lua::SSetInt(L, int(coord.x));
		lb::lua::SSetInt(L, int(coord.y));
		lb::lua::SSetInt(L, int(coord.z));
		return 3;
	}
	int VoxelGrid_BindLua::CoordToWorld(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 3)
		{
			lb::lua::SError(L, "VoxelGrid::CoordToWorld(int x,y,z) not enough arguments!");
			return 0;
		}
		int x = lb::lua::SGetInt(L, 1);
		int y = lb::lua::SGetInt(L, 2);
		int z = lb::lua::SGetInt(L, 3);
		Luna<Vector_BindLua>::push(L, voxelgrid->coord_to_world(XMUINT3(uint32_t(x), uint32_t(y), uint32_t(z))));
		return 1;
	}
	int VoxelGrid_BindLua::CheckVoxel(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::CheckVoxel(Vector pos) not enough arguments!");
			lb::lua::SError(L, "VoxelGrid::CheckVoxel(int x,y,z) not enough arguments!");
			return 0;
		}
		Vector_BindLua* pos = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (pos == nullptr)
		{
			if (argc < 3)
			{
				lb::lua::SError(L, "VoxelGrid::CheckVoxel(int x,y,z) not enough arguments!");
				return 0;
			}
			int x = lb::lua::SGetInt(L, 1);
			int y = lb::lua::SGetInt(L, 2);
			int z = lb::lua::SGetInt(L, 3);
			lb::lua::SSetBool(L, voxelgrid->check_voxel(XMUINT3(uint32_t(x), uint32_t(y), uint32_t(z))));
			return 1;
		}
		lb::lua::SSetBool(L, voxelgrid->check_voxel(pos->GetFloat3()));
		return 1;
	}
	int VoxelGrid_BindLua::SetVoxel(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 2)
		{
			lb::lua::SError(L, "VoxelGrid::SetVoxel(Vector pos, bool value) not enough arguments!");
			lb::lua::SError(L, "VoxelGrid::SetVoxel(int x,y,z, bool value) not enough arguments!");
			return 0;
		}
		Vector_BindLua* pos = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (pos == nullptr)
		{
			if (argc < 4)
			{
				lb::lua::SError(L, "VoxelGrid::SetVoxel(int x,y,z, bool value) not enough arguments!");
				return 0;
			}
			int x = lb::lua::SGetInt(L, 1);
			int y = lb::lua::SGetInt(L, 2);
			int z = lb::lua::SGetInt(L, 3);
			int value = lb::lua::SGetBool(L, 4);
			voxelgrid->set_voxel(XMUINT3(uint32_t(x), uint32_t(y), uint32_t(z)), value);
			return 0;
		}
		int value = lb::lua::SGetBool(L, 2);
		voxelgrid->set_voxel(pos->GetFloat3(), value);
		return 0;
	}
	int VoxelGrid_BindLua::GetCenter(lua_State* L)
	{
		Luna<Vector_BindLua>::push(L, voxelgrid->center);
		return 1;
	}
	int VoxelGrid_BindLua::SetCenter(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::SetCenter(Vector pos) not enough arguments!");
			return 0;
		}
		Vector_BindLua* pos = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (pos == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::SetCenter(Vector pos) first argument is not a Vector!");
			return 0;
		}
		voxelgrid->center = pos->GetFloat3();
		return 0;
	}
	int VoxelGrid_BindLua::GetVoxelSize(lua_State* L)
	{
		Luna<Vector_BindLua>::push(L, voxelgrid->voxelSize);
		return 1;
	}
	int VoxelGrid_BindLua::SetVoxelSize(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::SetVoxelSize(Vector voxelsize) not enough arguments!");
			lb::lua::SError(L, "VoxelGrid::SetVoxelSize(float voxelsize) not enough arguments!");
			return 0;
		}
		Vector_BindLua* size = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (size == nullptr)
		{
			voxelgrid->set_voxelsize(lb::lua::SGetFloat(L, 1));
			return 0;
		}
		voxelgrid->set_voxelsize(size->GetFloat3());
		return 0;
	}
	int VoxelGrid_BindLua::GetDebugColor(lua_State* L)
	{
		Luna<Vector_BindLua>::push(L, voxelgrid->debug_color);
		return 1;
	}
	int VoxelGrid_BindLua::SetDebugColor(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::SetDebugColor(Vector color) not enough arguments!");
			return 0;
		}
		Vector_BindLua* color = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (color == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::SetDebugColor(Vector color) first argument is not a Vector!");
			return 0;
		}
		voxelgrid->debug_color = color->data;
		return 0;
	}
	int VoxelGrid_BindLua::GetDebugColorExtent(lua_State* L)
	{
		Luna<Vector_BindLua>::push(L, voxelgrid->debug_color);
		return 1;
	}
	int VoxelGrid_BindLua::SetDebugColorExtent(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "VoxelGrid::SetDebugColorExtent(Vector color) not enough arguments!");
			return 0;
		}
		Vector_BindLua* color = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (color == nullptr)
		{
			lb::lua::SError(L, "VoxelGrid::SetDebugColorExtent(Vector color) first argument is not a Vector!");
			return 0;
		}
		voxelgrid->debug_color_extent = color->data;
		return 0;
	}
	int VoxelGrid_BindLua::GetMemorySize(lua_State* L)
	{
		lb::lua::SSetLongLong(L, (long long)voxelgrid->get_memory_size());
		return 1;
	}
	int VoxelGrid_BindLua::Add(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "Add(VoxelGrid other) not enough arguments!");
			return 0;
		}
		VoxelGrid_BindLua* other = Luna<VoxelGrid_BindLua>::lightcheck(L, 1);
		if (other == nullptr)
		{
			lb::lua::SError(L, "Add(VoxelGrid other) first argument is not a VoxelGrid!");
			return 0;
		}
		voxelgrid->add(*other->voxelgrid);
		return 0;
	}
	int VoxelGrid_BindLua::Subtract(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "Subtract(VoxelGrid other) not enough arguments!");
			return 0;
		}
		VoxelGrid_BindLua* other = Luna<VoxelGrid_BindLua>::lightcheck(L, 1);
		if (other == nullptr)
		{
			lb::lua::SError(L, "Subtract(VoxelGrid other) first argument is not a VoxelGrid!");
			return 0;
		}
		voxelgrid->subtract(*other->voxelgrid);
		return 0;
	}
	int VoxelGrid_BindLua::IsVisible(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 2)
		{
			lb::lua::SError(L, "VoxelGrid::IsVisible(Vector observer, Vector subject) not enough arguments!");
			lb::lua::SError(L, "VoxelGrid::IsVisible(Vector observer, AABB subject) not enough arguments!");
			return 0;
		}
		Vector_BindLua* observer = Luna<Vector_BindLua>::lightcheck(L, 1);
		if (observer == nullptr)
		{
			if (argc < 6)
			{
				lb::lua::SError(L, "VoxelGrid::IsVisible(int observer_x,observer_y,observer_z, subject_x,subject_y,subject_z) not enough arguments!");
				return 0;
			}
			int observer_x = lb::lua::SGetInt(L, 1);
			int observer_y = lb::lua::SGetInt(L, 2);
			int observer_z = lb::lua::SGetInt(L, 3);
			int subject_x = lb::lua::SGetInt(L, 4);
			int subject_y = lb::lua::SGetInt(L, 5);
			int subject_z = lb::lua::SGetInt(L, 6);
			lb::lua::SSetBool(L, voxelgrid->is_visible(XMUINT3(uint32_t(observer_x), uint32_t(observer_y), uint32_t(observer_z)), XMUINT3(uint32_t(subject_x), uint32_t(subject_y), uint32_t(subject_z))));
			return 1;
		}
		Vector_BindLua* subject_vec = Luna<Vector_BindLua>::lightcheck(L, 2);
		if (subject_vec == nullptr)
		{
			primitive::AABB_BindLua* aabb = Luna<primitive::AABB_BindLua>::lightcheck(L, 2);
			if (aabb == nullptr)
			{
				lb::lua::SError(L, "VoxelGrid::IsVisible(Vector observer, Vector subject) second argument is not a Vector!");
				lb::lua::SError(L, "VoxelGrid::IsVisible(Vector observer, AABB subject) second argument is not an AABB!");
				return 0;
			}
			lb::lua::SSetBool(L, voxelgrid->is_visible(observer->GetFloat3(), aabb->aabb));
			return 1;
		}
		lb::lua::SSetBool(L, voxelgrid->is_visible(observer->GetFloat3(), subject_vec->GetFloat3()));
		return 1;
	}
	int VoxelGrid_BindLua::FloodFill(lua_State* L)
	{
		voxelgrid->flood_fill();
		return 0;
	}

	void VoxelGrid_BindLua::Bind()
	{
		Luna<VoxelGrid_BindLua>::Register(lb::lua::GetLuaState());
	}
}
