#pragma once
#include "CommonInclude.h"
#include "lbLua.h"
#include "lbLuna.h"
#include "lbPathQuery.h"

namespace lb::lua
{
	class PathQuery_BindLua
	{
		lb::PathQuery owning;
	public:
		lb::PathQuery* pathquery = &owning;
		inline static constexpr char className[] = "PathQuery";
		static Luna<PathQuery_BindLua>::FunctionType methods[];
		static Luna<PathQuery_BindLua>::PropertyType properties[];

		PathQuery_BindLua() = default;
		PathQuery_BindLua(lua_State* L) {}
		PathQuery_BindLua(lb::PathQuery* component) :pathquery(component) {}

		int Process(lua_State* L);
		int SearchCover(lua_State* L);
		int IsSuccessful(lua_State* L);
		int GetNextWaypoint(lua_State* L);
		int SetDebugDrawWaypointsEnabled(lua_State* L);
		int SetFlying(lua_State* L);
		int IsFlying(lua_State* L);
		int SetAgentWidth(lua_State* L);
		int GetAgentWidth(lua_State* L);
		int SetAgentHeight(lua_State* L);
		int GetAgentHeight(lua_State* L);
		int GetWaypointCount(lua_State* L);
		int GetWaypoint(lua_State* L);
		int GetGoal(lua_State* L);

		static void Bind();
	};
}
