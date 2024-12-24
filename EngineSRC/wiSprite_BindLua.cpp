#include "wiSprite_BindLua.h"
#include "wiImageParams_BindLua.h"
#include "wiSpriteAnim_BindLua.h"
#include "wiTexture_BindLua.h"

namespace lb::lua
{

	Luna<Sprite_BindLua>::FunctionType Sprite_BindLua::methods[] = {
		lunamethod(Sprite_BindLua, SetParams),
		lunamethod(Sprite_BindLua, GetParams),
		lunamethod(Sprite_BindLua, SetAnim),
		lunamethod(Sprite_BindLua, GetAnim),
		lunamethod(Sprite_BindLua, SetTexture),
		lunamethod(Sprite_BindLua, GetTexture),
		lunamethod(Sprite_BindLua, SetMaskTexture),
		lunamethod(Sprite_BindLua, GetMaskTexture),
		lunamethod(Sprite_BindLua, SetBackgroundTexture),
		lunamethod(Sprite_BindLua, GetBackgroundTexture),
		lunamethod(Sprite_BindLua, SetHidden),
		lunamethod(Sprite_BindLua, IsHidden),

		{ NULL, NULL }
	};
	Luna<Sprite_BindLua>::PropertyType Sprite_BindLua::properties[] = {
		lunaproperty(Sprite_BindLua, Params),
		lunaproperty(Sprite_BindLua, Anim),
		{ NULL, NULL }
	};

	Sprite_BindLua::Sprite_BindLua(const lb::Sprite& sprite) :sprite(sprite)
	{
	}

	Sprite_BindLua::Sprite_BindLua(lua_State* L)
	{
		std::string name = "", mask = "";
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			name = lb::lua::SGetString(L, 1);
			if (argc > 1)
			{
				mask = lb::lua::SGetString(L, 2);
			}
		}
		sprite = lb::Sprite(name, mask);
	}

	int Sprite_BindLua::SetParams(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::lua::ImageParams_BindLua* params = Luna<lb::lua::ImageParams_BindLua>::check(L, 1);
			if (params != nullptr)
			{
				sprite.params = params->params;
			}
		}
		else
		{
			lb::lua::SError(L, "SetParams(ImageEffects effects) not enough arguments!");
		}
		return 0;
	}
	int Sprite_BindLua::GetParams(lua_State* L)
	{
		Luna<lb::lua::ImageParams_BindLua>::push(L, sprite.params);
		return 1;
	}
	int Sprite_BindLua::SetAnim(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			SpriteAnim_BindLua* anim = Luna<SpriteAnim_BindLua>::check(L, 1);
			if (anim != nullptr)
			{
				sprite.anim = anim->anim;
			}
		}
		else
		{
			lb::lua::SError(L, "SetAnim(SpriteAnim anim) not enough arguments!");
		}
		return 0;
	}
	int Sprite_BindLua::GetAnim(lua_State* L)
	{
		Luna<SpriteAnim_BindLua>::push(L, sprite.anim);
		return 1;
	}
	int Sprite_BindLua::SetTexture(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Texture_BindLua* tex = Luna<Texture_BindLua>::check(L, 1);
			if (tex != nullptr)
			{
				sprite.textureResource = tex->resource;
			}
		}
		else
		{
			lb::lua::SError(L, "SetTexture(Texture texture) not enough arguments!");
		}
		return 0;
	}
	int Sprite_BindLua::GetTexture(lua_State* L)
	{
		Luna<Texture_BindLua>::push(L, sprite.textureResource);
		return 1;
	}
	int Sprite_BindLua::SetMaskTexture(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Texture_BindLua* tex = Luna<Texture_BindLua>::check(L, 1);
			if (tex != nullptr)
			{
				sprite.maskResource = tex->resource;
				sprite.params.setMaskMap(&tex->resource.GetTexture());
			}
		}
		else
		{
			lb::lua::SError(L, "SetMaskTexture(Texture texture) not enough arguments!");
		}
		return 0;
	}
	int Sprite_BindLua::GetMaskTexture(lua_State* L)
	{
		Luna<Texture_BindLua>::push(L, sprite.maskResource);
		return 1;
	}
	int Sprite_BindLua::SetBackgroundTexture(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Texture_BindLua* tex = Luna<Texture_BindLua>::check(L, 1);
			if (tex != nullptr)
			{
				sprite.backgroundResource = tex->resource;
				sprite.params.setBackgroundMap(&tex->resource.GetTexture());
			}
		}
		else
		{
			lb::lua::SError(L, "SetBackgroundTexture(Texture texture) not enough arguments!");
		}
		return 0;
	}
	int Sprite_BindLua::GetBackgroundTexture(lua_State* L)
	{
		Luna<Texture_BindLua>::push(L, sprite.backgroundResource);
		return 1;
	}
	int Sprite_BindLua::SetHidden(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "SetHidden(bool value) not enough arguments!");
			return 0;
		}
		sprite.SetHidden(lb::lua::SGetBool(L, 1));
		return 0;
	}
	int Sprite_BindLua::IsHidden(lua_State* L)
	{
		lb::lua::SSetBool(L, sprite.IsHidden());
		return 1;
	}

	void Sprite_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<Sprite_BindLua>::Register(lb::lua::GetLuaState());
		}
	}

}
