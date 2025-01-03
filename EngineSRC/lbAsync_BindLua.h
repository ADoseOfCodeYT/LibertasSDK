#pragma once
#include "lbLua.h"
#include "lbLuna.h"
#include "lbJobSystem.h"

namespace lb::lua
{
	class Async_BindLua
	{
	public:
		lb::jobsystem::context ctx;
		inline static constexpr char className[] = "Async";
		static Luna<Async_BindLua>::FunctionType methods[];
		static Luna<Async_BindLua>::PropertyType properties[];

		Async_BindLua() = default;
		Async_BindLua(lua_State* L) {}

		int Wait(lua_State* L);
		int IsCompleted(lua_State* L);

		static void Bind();
	};
}
