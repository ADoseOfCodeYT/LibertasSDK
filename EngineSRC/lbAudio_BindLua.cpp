#include "lbAudio_BindLua.h"
#include "lbAudio.h"
#include "lbMath_BindLua.h"

namespace lb::lua
{

	Luna<Audio_BindLua>::FunctionType Audio_BindLua::methods[] = {
		lunamethod(Audio_BindLua, CreateSound),
		lunamethod(Audio_BindLua, CreateSoundInstance),
		lunamethod(Audio_BindLua, Play),
		lunamethod(Audio_BindLua, Pause),
		lunamethod(Audio_BindLua, Stop),
		lunamethod(Audio_BindLua, GetVolume),
		lunamethod(Audio_BindLua, SetVolume),
		lunamethod(Audio_BindLua, ExitLoop),
		lunamethod(Audio_BindLua, GetSubmixVolume),
		lunamethod(Audio_BindLua, SetSubmixVolume),
		lunamethod(Audio_BindLua, Update3D),
		lunamethod(Audio_BindLua, SetReverb),
		{ NULL, NULL }
	};
	Luna<Audio_BindLua>::PropertyType Audio_BindLua::properties[] = {
		{ NULL, NULL }
	};

	int Audio_BindLua::CreateSound(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Sound_BindLua* sound = Luna<Sound_BindLua>::lightcheck(L, 2);
			if (sound != nullptr)
			{
				sound->soundResource = lb::resourcemanager::Load(lb::lua::SGetString(L, 1));
				if (!sound->soundResource.IsValid())
				{
					lb::lua::SSetBool(L, false);
					return 1;
				}
				lb::lua::SSetBool(L, sound->soundResource.GetSound().IsValid());
				return 1;
			}
			else
			{
				lb::lua::SError(L, "CreateSound(string filename, Sound sound) second argument is not a Sound!");
			}
		}
		else
			lb::lua::SError(L, "CreateSound(string filename, Sound sound) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::CreateSoundInstance(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Sound_BindLua* s = Luna<Sound_BindLua>::lightcheck(L, 1);
			if (s == nullptr)
			{
				lb::lua::SError(L, "CreateSoundInstance(Sound sound, SoundInstance soundinstance) first argument is not a Sound!");
				return 0;
			}
			if (!s->soundResource.IsValid())
			{
				lb::lua::SError(L, "CreateSoundInstance(Sound sound, SoundInstance soundinstance) first argument Sound resource is not valid, probably it was not loaded successfully!");
				return 0;
			}
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 2);
			if (soundinstance == nullptr)
			{
				lb::lua::SError(L, "CreateSoundInstance(Sound sound, SoundInstance soundinstance) second argument is not a SoundInstance!");
				return 0;
			}

			const lb::audio::Sound& sound = s->soundResource.GetSound();
			bool result = lb::audio::CreateSoundInstance(&sound, &soundinstance->soundinstance);
			lb::lua::SSetBool(L, result);
			return 1;
		}
		else
			lb::lua::SError(L, "CreateSoundInstance(Sound sound, SoundInstance soundinstance) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::Play(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 1);
			if (soundinstance != nullptr)
			{
				lb::audio::Play(&soundinstance->soundinstance);
			}
			else
			{
				lb::lua::SError(L, "Play(SoundInstance soundinstance) argument is not a SoundInstance!");
			}
		}
		else
			lb::lua::SError(L, "Play(SoundInstance soundinstance) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::Pause(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 1);
			if (soundinstance != nullptr)
			{
				lb::audio::Pause(&soundinstance->soundinstance);
			}
			else
			{
				lb::lua::SError(L, "Pause(SoundInstance soundinstance) argument is not a SoundInstance!");
			}
		}
		else
			lb::lua::SError(L, "Pause(SoundInstance soundinstance) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::Stop(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 1);
			if (soundinstance != nullptr)
			{
				lb::audio::Stop(&soundinstance->soundinstance);
			}
			else
			{
				lb::lua::SError(L, "Stop(SoundInstance soundinstance) argument is not a SoundInstance!");
			}
		}
		else
			lb::lua::SError(L, "Stop(SoundInstance soundinstance) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::GetVolume(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 1);
			float volume = lb::audio::GetVolume(soundinstance == nullptr ? nullptr : &soundinstance->soundinstance);
			lb::lua::SSetFloat(L, volume);
			return 1;
		}
		else
			lb::lua::SError(L, "GetVolume(opt SoundInstance soundinstance) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::SetVolume(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			float volume = lb::lua::SGetFloat(L, 1);
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 2);
			lb::audio::SetVolume(volume, soundinstance == nullptr ? nullptr : &soundinstance->soundinstance);
			return 0;
		}
		else
			lb::lua::SError(L, "SetVolume(float volume, opt SoundInstance soundinstance) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::ExitLoop(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 1);
			if (soundinstance != nullptr)
			{
				lb::audio::ExitLoop(&soundinstance->soundinstance);
			}
			else
			{
				lb::lua::SError(L, "ExitLoop(SoundInstance soundinstance) argument is not a SoundInstance!");
			}
		}
		else
			lb::lua::SError(L, "ExitLoop(SoundInstance soundinstance) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::GetSubmixVolume(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::audio::SUBMIX_TYPE type = (lb::audio::SUBMIX_TYPE)lb::lua::SGetInt(L, 1);
			float volume = lb::audio::GetSubmixVolume(type);
			lb::lua::SSetFloat(L, volume);
			return 1;
		}
		else
			lb::lua::SError(L, "GetSubmixVolume(int submixtype) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::SetSubmixVolume(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			lb::audio::SUBMIX_TYPE type = (lb::audio::SUBMIX_TYPE)lb::lua::SGetInt(L, 1);
			float volume = lb::lua::SGetFloat(L, 2);
			lb::audio::SetSubmixVolume(type, volume);
			return 0;
		}
		else
			lb::lua::SError(L, "SetSubmixVolume(int submixtype, float volume) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::Update3D(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			SoundInstance_BindLua* soundinstance = Luna<SoundInstance_BindLua>::lightcheck(L, 1);
			SoundInstance3D_BindLua* soundinstance3D = Luna<SoundInstance3D_BindLua>::lightcheck(L, 2);
			if (soundinstance != nullptr && soundinstance3D != nullptr)
			{
				lb::audio::Update3D(&soundinstance->soundinstance, soundinstance3D->soundinstance3D);
			}
			else
			{
				lb::lua::SError(L, "Update3D(SoundInstance soundinstance, SoundInstance3D instance3D) argument types mismatch!");
			}
		}
		else
			lb::lua::SError(L, "Update3D(SoundInstance soundinstance, SoundInstance3D instance3D) not enough arguments!");
		return 0;
	}
	int Audio_BindLua::SetReverb(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::audio::SetReverb((lb::audio::REVERB_PRESET)lb::lua::SGetInt(L, 1));
		}
		else
			lb::lua::SError(L, "SetReverb(int reverbtype) not enough arguments!");
		return 0;
	}

	void Audio_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<Audio_BindLua>::Register(lb::lua::GetLuaState());

			lb::lua::RunText(R"(
audio = Audio()

SUBMIX_TYPE_SOUNDEFFECT = 0
SUBMIX_TYPE_MUSIC = 1
SUBMIX_TYPE_USER0 = 2
SUBMIX_TYPE_USER1 = 3

REVERB_PRESET_DEFAULT = 0
REVERB_PRESET_GENERIC = 1
REVERB_PRESET_FOREST = 2
REVERB_PRESET_PADDEDCELL = 3
REVERB_PRESET_ROOM = 4
REVERB_PRESET_BATHROOM = 5
REVERB_PRESET_LIVINGROOM = 6
REVERB_PRESET_STONEROOM = 7
REVERB_PRESET_AUDITORIUM = 8
REVERB_PRESET_CONCERTHALL = 9
REVERB_PRESET_CAVE = 10
REVERB_PRESET_ARENA = 11
REVERB_PRESET_HANGAR = 12
REVERB_PRESET_CARPETEDHALLWAY = 13
REVERB_PRESET_HALLWAY = 14
REVERB_PRESET_STONECORRIDOR = 15
REVERB_PRESET_ALLEY = 16
REVERB_PRESET_CITY = 17
REVERB_PRESET_MOUNTAINS = 18
REVERB_PRESET_QUARRY = 19
REVERB_PRESET_PLAIN = 20
REVERB_PRESET_PARKINGLOT = 21
REVERB_PRESET_SEWERPIPE = 22
REVERB_PRESET_UNDERWATER = 23
REVERB_PRESET_SMALLROOM = 24
REVERB_PRESET_MEDIUMROOM = 25
REVERB_PRESET_LARGEROOM = 26
REVERB_PRESET_MEDIUMHALL = 27
REVERB_PRESET_LARGEHALL = 28
REVERB_PRESET_PLATE = 29
)");

			Sound_BindLua::Bind();
			SoundInstance_BindLua::Bind();
			SoundInstance3D_BindLua::Bind();
		}
	}




	Luna<Sound_BindLua>::FunctionType Sound_BindLua::methods[] = {
		lunamethod(Sound_BindLua, IsValid),
		{ NULL, NULL }
	};
	Luna<Sound_BindLua>::PropertyType Sound_BindLua::properties[] = {
		{ NULL, NULL }
	};

	Sound_BindLua::Sound_BindLua(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundResource = lb::resourcemanager::Load(lb::lua::SGetString(L, 1));
		}
	}

	int Sound_BindLua::IsValid(lua_State* L)
	{
		lb::lua::SSetBool(L, soundResource.IsValid() && soundResource.GetSound().IsValid());
		return 1;
	}

	void Sound_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<Sound_BindLua>::Register(lb::lua::GetLuaState());
		}
	}



	Luna<SoundInstance_BindLua>::FunctionType SoundInstance_BindLua::methods[] = {
		lunamethod(SoundInstance_BindLua, SetSubmixType),
		lunamethod(SoundInstance_BindLua, SetBegin),
		lunamethod(SoundInstance_BindLua, SetLength),
		lunamethod(SoundInstance_BindLua, SetLoopBegin),
		lunamethod(SoundInstance_BindLua, SetLoopLength),
		lunamethod(SoundInstance_BindLua, SetEnableReverb),
		lunamethod(SoundInstance_BindLua, SetLooped),
		lunamethod(SoundInstance_BindLua, GetSubmixType),
		lunamethod(SoundInstance_BindLua, GetBegin),
		lunamethod(SoundInstance_BindLua, GetLength),
		lunamethod(SoundInstance_BindLua, GetLoopBegin),
		lunamethod(SoundInstance_BindLua, GetLoopLength),
		lunamethod(SoundInstance_BindLua, IsEnableReverb),
		lunamethod(SoundInstance_BindLua, IsLooped),
		lunamethod(SoundInstance_BindLua, IsValid),
		{ NULL, NULL }
	};
	Luna<SoundInstance_BindLua>::PropertyType SoundInstance_BindLua::properties[] = {
		{ NULL, NULL }
	};

	SoundInstance_BindLua::SoundInstance_BindLua(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Sound_BindLua* s = Luna<Sound_BindLua>::lightcheck(L, 1);
			if (s == nullptr)
			{
				lb::lua::SError(L, "SoundInstance(Sound sound, opt float begin,length) : first argument is not a Sound!");
				return;
			}
			if (argc > 1)
			{
				soundinstance.begin = lb::lua::SGetFloat(L, 2);
				if (argc > 2)
				{
					soundinstance.length = lb::lua::SGetFloat(L, 3);
				}
			}
			if (!s->soundResource.IsValid())
			{
				lb::lua::SError(L, "SoundInstance(Sound sound, opt float begin,end) : Sound is not valid!");
				return;
			}
			const lb::audio::Sound& sound = s->soundResource.GetSound();
			if (!sound.IsValid())
			{
				lb::lua::SError(L, "SoundInstance(Sound sound, opt float begin,end) : Sound is not valid!");
				return;
			}
			lb::audio::CreateSoundInstance(&sound, &soundinstance);
		}
	}

	int SoundInstance_BindLua::SetSubmixType(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::audio::SUBMIX_TYPE type = (lb::audio::SUBMIX_TYPE)lb::lua::SGetInt(L, 1);
			soundinstance.type = type;
		}
		else
			lb::lua::SError(L, "SetSubmixType(int submixtype) not enough arguments!");
		return 0;
	}
	int SoundInstance_BindLua::SetBegin(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundinstance.begin = lb::lua::SGetFloat(L, 1);
		}
		else
			lb::lua::SError(L, "SetBegin(float seconds) not enough arguments!");
		return 0;
	}
	int SoundInstance_BindLua::SetLength(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundinstance.length = lb::lua::SGetFloat(L, 1);
		}
		else
			lb::lua::SError(L, "SetLength(float seconds) not enough arguments!");
		return 0;
	}
	int SoundInstance_BindLua::SetLoopBegin(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundinstance.loop_begin = lb::lua::SGetFloat(L, 1);
		}
		else
			lb::lua::SError(L, "SetLoopBegin(float seconds) not enough arguments!");
		return 0;
	}
	int SoundInstance_BindLua::SetLoopLength(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundinstance.loop_length = lb::lua::SGetFloat(L, 1);
		}
		else
			lb::lua::SError(L, "SetLoopLength(float seconds) not enough arguments!");
		return 0;
	}
	int SoundInstance_BindLua::SetEnableReverb(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundinstance.SetEnableReverb(lb::lua::SGetBool(L, 1));
		}
		else
			lb::lua::SError(L, "SetEnableReverb(bool value) not enough arguments!");
		return 0;
	}
	int SoundInstance_BindLua::SetLooped(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundinstance.SetLooped(lb::lua::SGetBool(L, 1));
		}
		else
			lb::lua::SError(L, "SetLooped(bool value) not enough arguments!");
		return 0;
	}
	int SoundInstance_BindLua::GetSubmixType(lua_State* L)
	{
		lb::lua::SSetInt(L, (int)soundinstance.type);
		return 1;
	}
	int SoundInstance_BindLua::GetBegin(lua_State* L)
	{
		lb::lua::SSetFloat(L, soundinstance.begin);
		return 1;
	}
	int SoundInstance_BindLua::GetLength(lua_State* L)
	{
		lb::lua::SSetFloat(L, soundinstance.length);
		return 1;
	}
	int SoundInstance_BindLua::GetLoopBegin(lua_State* L)
	{
		lb::lua::SSetFloat(L, soundinstance.loop_begin);
		return 1;
	}
	int SoundInstance_BindLua::GetLoopLength(lua_State* L)
	{
		lb::lua::SSetFloat(L, soundinstance.loop_length);
		return 1;
	}
	int SoundInstance_BindLua::IsEnableReverb(lua_State* L)
	{
		lb::lua::SSetBool(L, soundinstance.IsEnableReverb());
		return 1;
	}
	int SoundInstance_BindLua::IsLooped(lua_State* L)
	{
		lb::lua::SSetBool(L, soundinstance.IsLooped());
		return 1;
	}
	int SoundInstance_BindLua::IsValid(lua_State* L)
	{
		lb::lua::SSetBool(L, soundinstance.IsValid());
		return 1;
	}

	void SoundInstance_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<SoundInstance_BindLua>::Register(lb::lua::GetLuaState());
		}
	}




	Luna<SoundInstance3D_BindLua>::FunctionType SoundInstance3D_BindLua::methods[] = {
		lunamethod(SoundInstance3D_BindLua, SetListenerPos),
		lunamethod(SoundInstance3D_BindLua, SetListenerUp),
		lunamethod(SoundInstance3D_BindLua, SetListenerFront),
		lunamethod(SoundInstance3D_BindLua, SetListenerVelocity),
		lunamethod(SoundInstance3D_BindLua, SetEmitterPos),
		lunamethod(SoundInstance3D_BindLua, SetEmitterUp),
		lunamethod(SoundInstance3D_BindLua, SetEmitterFront),
		lunamethod(SoundInstance3D_BindLua, SetEmitterVelocity),
		lunamethod(SoundInstance3D_BindLua, SetEmitterRadius),
		{ NULL, NULL }
	};
	Luna<SoundInstance3D_BindLua>::PropertyType SoundInstance3D_BindLua::properties[] = {
		{ NULL, NULL }
	};

	int SoundInstance3D_BindLua::SetListenerPos(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.listenerPos, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetListenerPos(Vector value) not enough arguments!");
		return 0;
	}
	int SoundInstance3D_BindLua::SetListenerUp(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.listenerUp, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetListenerUp(Vector value) not enough arguments!");
		return 0;
	}
	int SoundInstance3D_BindLua::SetListenerFront(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.listenerFront, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetListenerFront(Vector value) not enough arguments!");
		return 0;
	}
	int SoundInstance3D_BindLua::SetListenerVelocity(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.listenerVelocity, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetListenerVelocity(Vector value) not enough arguments!");
		return 0;
	}

	int SoundInstance3D_BindLua::SetEmitterPos(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.emitterPos, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetEmitterPos(Vector value) not enough arguments!");
		return 0;
	}
	int SoundInstance3D_BindLua::SetEmitterUp(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.emitterUp, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetEmitterUp(Vector value) not enough arguments!");
		return 0;
	}
	int SoundInstance3D_BindLua::SetEmitterFront(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.emitterFront, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetEmitterFront(Vector value) not enough arguments!");
		return 0;
	}
	int SoundInstance3D_BindLua::SetEmitterVelocity(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				XMStoreFloat3(&soundinstance3D.emitterVelocity, XMLoadFloat4(&vec->data));
			}
		}
		else
			lb::lua::SError(L, "SetEmitterVelocity(Vector value) not enough arguments!");
		return 0;
	}
	int SoundInstance3D_BindLua::SetEmitterRadius(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			soundinstance3D.emitterRadius = lb::lua::SGetFloat(L, 1);
		}
		else
			lb::lua::SError(L, "SetEmitterVelocity(Vector value) not enough arguments!");
		return 0;
	}

	void SoundInstance3D_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<SoundInstance3D_BindLua>::Register(lb::lua::GetLuaState());
		}
	}

}
