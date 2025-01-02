#include "lbConsole_BindLua.h"
#include "lbConsole.h"
#include "lbLua.h"
#include "lbMath_BindLua.h"

#include <string>

namespace lb::lua::console
{
	int console_clear(lua_State* L)
	{
		lb::console::clear();
		return 0;
	}
	int console_post(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);

		std::string ss;

		for (int i = 1; i <= argc; i++)
		{
			if (lb::lua::SIsString(L, i))
			{
				ss += lb::lua::SGetString(L, i);
			}
			else if (lb::lua::SIsNumber(L, i))
			{
				double arg = lb::lua::SGetDouble(L, i);
				ss += std::to_string(arg);
			}
			else if (lb::lua::SIsBool(L, i)) {
				bool arg = lb::lua::SGetBool(L, i);
				ss += arg ? "true" : "false";
			}
			else if (lb::lua::SIsNil(L, i)) {
				ss += "nil";
			}
			else
			{
				Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, i);
				if (vec != nullptr)
				{
					ss += "Vector(" + std::to_string(vec->data.x) + ", " + std::to_string(vec->data.y) + ", " + std::to_string(vec->data.z) + ", " + std::to_string(vec->data.w) + ")";
				}
				else
				{
					Matrix_BindLua* mat = Luna<Matrix_BindLua>::lightcheck(L, i);
					if (mat != nullptr)
					{
						ss += "Matrix(\n";
						ss += "\t" + std::to_string(mat->data._11) + ", " + std::to_string(mat->data._12) + ", " + std::to_string(mat->data._13) + ", " + std::to_string(mat->data._14) + "\n";
						ss += "\t" + std::to_string(mat->data._21) + ", " + std::to_string(mat->data._22) + ", " + std::to_string(mat->data._23) + ", " + std::to_string(mat->data._24) + "\n";
						ss += "\t" + std::to_string(mat->data._31) + ", " + std::to_string(mat->data._32) + ", " + std::to_string(mat->data._33) + ", " + std::to_string(mat->data._34) + "\n";
						ss += "\t" + std::to_string(mat->data._41) + ", " + std::to_string(mat->data._42) + ", " + std::to_string(mat->data._43) + ", " + std::to_string(mat->data._44) + "\n";
						ss += ")";
					}
				}
			}
		}

		if (!ss.empty())
		{
			lb::console::Post(ss);
		}

		return 0;
	}
	int console_isactive(lua_State* L)
	{
		lb::lua::SSetBool(L, lb::console::IsActive());
		return 1;
	}
	int console_setlevel(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::console::SetLogLevel((lb::console::LogLevel)lb::lua::SGetInt(L, 1));
		}
		else
			lb::lua::SError(L, "console_setlevel(int val) not enough arguments!");
		return 0;
	}
	int console_lock(lua_State* L)
	{
		lb::console::Lock();
		return 0;
	}
	int console_unlock(lua_State* L)
	{
		lb::console::Unlock();
		return 0;
	}

	void Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			lb::lua::RegisterFunc("console_clear", console_clear);
			lb::lua::RegisterFunc("console_post", console_post);
			lb::lua::RegisterFunc("console_isactive", console_isactive);
			lb::lua::RegisterFunc("console_setlevel", console_setlevel);
			lb::lua::RegisterFunc("console_lock", console_lock);
			lb::lua::RegisterFunc("console_unlock", console_unlock);
		}
	}
}
