#pragma once
#include "lbLua.h"
#include "lbLuna.h"
#include "lbApplication.h"

namespace lb::lua
{

	class Canvas_BindLua
	{
	public:
		lb::Canvas canvas;
		inline static constexpr char className[] = "Canvas";
		static Luna<Canvas_BindLua>::FunctionType methods[];
		static Luna<Canvas_BindLua>::PropertyType properties[];

		Canvas_BindLua(const lb::Canvas& canvas) : canvas(canvas) {}
		Canvas_BindLua(lua_State* L) {}

		int GetDPI(lua_State* L);
		int GetDPIScaling(lua_State* L);
		int GetCustomScaling(lua_State* L);
		int SetCustomScaling(lua_State* L);
		int GetPhysicalWidth(lua_State* L);
		int GetPhysicalHeight(lua_State* L);
		int GetLogicalWidth(lua_State* L);
		int GetLogicalHeight(lua_State* L);

		static void Bind();
	};

	class Application_BindLua
	{
	public:
		Application* component = nullptr;
		inline static constexpr char className[] = "Application";
		static Luna<Application_BindLua>::FunctionType methods[];
		static Luna<Application_BindLua>::PropertyType properties[];

		Application_BindLua(Application* component = nullptr);
		Application_BindLua(lua_State* L);

		int GetActivePath(lua_State* L);
		int SetActivePath(lua_State* L);
		int SetFrameSkip(lua_State* L);
		int SetFullScreen(lua_State* L);
		int SetTargetFrameRate(lua_State* L);
		int SetFrameRateLock(lua_State* L);
		int SetInfoDisplay(lua_State* L);
		int SetWatermarkDisplay(lua_State* L);
		int SetFPSDisplay(lua_State* L);
		int SetResolutionDisplay(lua_State* L);
		int SetLogicalSizeDisplay(lua_State* L);
		int SetColorSpaceDisplay(lua_State* L);
		int SetPipelineCountDisplay(lua_State* L);
		int SetHeapAllocationCountDisplay(lua_State* L);
		int SetVRAMUsageDisplay(lua_State* L);
		int SetColorGradingHelper(lua_State* L);

		int IsHDRSupported(lua_State* L);
		int SetHDR(lua_State* L);

		int GetCanvas(lua_State* L);
		int SetCanvas(lua_State* L);

		int Exit(lua_State* L);
		int IsFaded(lua_State* L);

		static void Bind();
	};

}
