#pragma once
#include "lbLua.h"
#include "lbLuna.h"
#include "lbGraphics.h"
#include "lbResourceManager.h"

namespace lb::lua
{

	class Texture_BindLua
	{
	public:
		lb::Resource resource;

		inline static constexpr char className[] = "Texture";
		static Luna<Texture_BindLua>::FunctionType methods[];
		static Luna<Texture_BindLua>::PropertyType properties[];

		Texture_BindLua() = default;
		Texture_BindLua(lb::Resource resource) :resource(resource) {}
		Texture_BindLua(lb::graphics::Texture texture) { resource.SetTexture(texture); }
		Texture_BindLua(lua_State* L);

		int GetLogo(lua_State* L);
		int CreateGradientTexture(lua_State* L);
		int CreateLensDistortionNormalMap(lua_State* L);
		int Save(lua_State* L);

		int IsValid(lua_State* L);
		int GetWidth(lua_State* L);
		int GetHeight(lua_State* L);
		int GetDepth(lua_State* L);
		int GetArraySize(lua_State* L);

		static void Bind();
	};

}
