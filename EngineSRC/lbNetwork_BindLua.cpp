#include "lbNetwork_BindLua.h"
#include "lbNetwork.h"
#include "lbHelper.h"

namespace lb::lua
{

	Luna<Network_BindLua>::FunctionType Network_BindLua::methods[] = {
		{ NULL, NULL }
	};
	Luna<Network_BindLua>::PropertyType Network_BindLua::properties[] = {
		{ NULL, NULL }
	};

	void Network_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<Network_BindLua>::Register(lb::lua::GetLuaState());
			Luna<Network_BindLua>::push_global(lb::lua::GetLuaState(), "network");
		}
	}

}
