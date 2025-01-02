#include "lbConvars.h"
#include "lbConsole.h"

// Based on https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/cvars.cpp
// Yes the fact that this is a class is wierd but turning it into a namespace would take alot of rewriting

#include <unordered_map>
#include <array>
#include <shared_mutex>

#define MAX_INT_CONVARS 1000;
#define MAX_FLOAT_CONVARS 1000;
#define MAX_STRING_CONVARS 200;


// Similar to Source engine's sv_cheats
// Console variables that have the flag Protected cannot be changed by the console if this value is equal to 1
ConVar_Int CVAR_protection_enabled ("protection_enabled", 1, ConVarFlags::UnProtected);

class ConVarParams
{
public:
	friend class lbConVarSystem;

	int32_t ArrayIndex;

	ConVarTypes Type;
	ConVarFlags Flags;
	std::string Name;
};

template<typename T>
struct ConVarStorage
{
	T Initial;
	T Current;
	ConVarParams* Parameter;
};

template<typename T>
struct ConVarArray
{
	ConVarStorage<T>* ConVars{ nullptr };
	int32_t LastConVar{ 0 };

	ConVarArray(size_t size)
	{
		ConVars = new ConVarStorage<T>[size]();
	}

	ConVarStorage<T>* GetCurrentStorage(int32_t Index)
	{
		return &ConVars[Index];
	}

	T* GetCurrentPtr(int32_t Index)
	{
		return &ConVars[Index].Current;
	};

	T GetCurrent(int32_t Index)
	{
		return ConVars[Index].Current;
	};

	void SetCurrent(const T& val, int32_t Index)
	{
		ConVars[Index].Current = val;
	}

	int Add(const T& value, ConVarParams* Param)
	{
		int Index = LastConVar;

		ConVars[Index].Current = value;
		ConVars[Index].Initial = value;
		ConVars[Index].Parameter = Param;

		Param->ArrayIndex = Index;
		LastConVar++;
		return Index;
	}

	int Add(const T& InitialValue, const T& CurrentValue, ConVarParams* Param)
	{
		int Index = LastConVar;

		ConVars[Index].Current = CurrentValue;
		ConVars[Index].Initial = InitialValue;
		ConVars[Index].Parameter = Param;

		Param->ArrayIndex = Index;
		LastConVar++;

		return Index;
	}
};

class lbConVarSystem : public ConVarSystem
{
public:
	ConVarParams* CreateConvar(const char* Name, double defaultValue, double CurrentValue) override final;
	ConVarParams* CreateConvar(const char* Name, int32_t defaultValue, int32_t CurrentValue) override final;
	ConVarParams* CreateConvar(const char* Name, const char* defaultValue, const char* CurrentValue) override final;


	ConVarParams* FindConVar(StringUtils::StringHash hash) override final;

	double* GetFloatConVarVal(StringUtils::StringHash hash) override final;
	int32_t* GetIntConVarVal(StringUtils::StringHash hash) override final;
	const char* GetStringConVarVal(StringUtils::StringHash hash) override final;

	void SetConVarValueSafe(StringUtils::StringHash hash, double value) override final;
	void SetConVarValueSafe(StringUtils::StringHash hash, int32_t value) override final;
	void SetConVarValueSafe(StringUtils::StringHash hash, const char* value) override final;

	ConVarArray<int32_t> IntConVars = MAX_INT_CONVARS;
	ConVarArray<double> FloatConVars = MAX_FLOAT_CONVARS;
	ConVarArray<std::string> StringConVars = MAX_STRING_CONVARS;
	
	template<typename T>
	ConVarArray<T>* GetConVarArray();

	template<>
	ConVarArray<int32_t>* GetConVarArray()
	{
		return &IntConVars;
	}
	template<>
	ConVarArray<double>* GetConVarArray()
	{
		return &FloatConVars;
	}
	template<>
	ConVarArray<std::string>* GetConVarArray()
	{
		return &StringConVars;
	}

	template<typename T>
	T* GetCurrentConVar(uint32_t namehash) {
		ConVarParams* par = FindConVar(namehash);
		if (!par) {
			return nullptr;
		}
		else {
			return GetConVarArray<T>()->GetCurrentPtr(par->ArrayIndex);
		}
	}

	template<typename T>
	void SetConVarCurrent(uint32_t namehash, const T& value)
	{
		ConVarParams* convar = FindConVar(namehash);
		
		if (convar)
		{
			switch (convar->Flags)
			{
			case ConVarFlags::Protected:
				if(CVAR_protection_enabled.Get() != 1)
					GetConVarArray<T>()->SetCurrent(value, convar->ArrayIndex);
				else
					lb::console::Post("Cannot change the value of ( " + convar->Name + " ) because it is a protected variable", lb::console::LogLevel::Warning);
				break;
			case ConVarFlags::UnProtected:
				GetConVarArray<T>()->SetCurrent(value, convar->ArrayIndex);
				break;
			default:
				break;
			}
		}
	}

	static lbConVarSystem* Get()
	{
		return static_cast<lbConVarSystem*>(ConVarSystem::Get());
	}

private:

	std::shared_mutex mutex_;

	ConVarParams* RegisterConVar(const char* Name);

	std::unordered_map<uint32_t, ConVarParams> SavedConVars;

	std::vector<ConVarParams*> cachedEditParameters;
};

double* lbConVarSystem::GetFloatConVarVal(StringUtils::StringHash hash)
{
	return GetCurrentConVar<double>(hash);
}

int32_t* lbConVarSystem::GetIntConVarVal(StringUtils::StringHash hash)
{
	return GetCurrentConVar<int32_t>(hash);
}

const char* lbConVarSystem::GetStringConVarVal(StringUtils::StringHash hash)
{
	return GetCurrentConVar<std::string>(hash)->c_str();
}

ConVarParams* lbConVarSystem::FindConVar(StringUtils::StringHash hash)
{
	std::shared_lock lock(mutex_);
	auto it = SavedConVars.find(hash);

	if (it != SavedConVars.end())
	{
		return &(*it).second;
	}

	return nullptr;
}

ConVarSystem* ConVarSystem::Get()
{
	static lbConVarSystem ConvarSys{};
	return &ConvarSys;
}

void lbConVarSystem::SetConVarValueSafe(StringUtils::StringHash hash, double value)
{
	SetConVarCurrent<double>(hash, value);
}

void lbConVarSystem::SetConVarValueSafe(StringUtils::StringHash hash, int32_t value)
{
	SetConVarCurrent<int32_t>(hash, value);
}

void lbConVarSystem::SetConVarValueSafe(StringUtils::StringHash hash, const char* value)
{
	SetConVarCurrent<std::string>(hash, value);
}

ConVarParams* lbConVarSystem::CreateConvar(const char* Name, double defaultValue, double CurrentValue)
{
	std::unique_lock lock(mutex_);
	ConVarParams* Param = RegisterConVar(Name);
	if (!Param) return nullptr;

	Param->Type = ConVarTypes::FLOAT;

	GetConVarArray<double>()->Add(defaultValue, CurrentValue, Param);

	return Param;
}

ConVarParams* lbConVarSystem::CreateConvar(const char* Name, int32_t defaultValue, int32_t CurrentValue)
{
	std::unique_lock lock(mutex_);
	ConVarParams* Param = RegisterConVar(Name);
	if (!Param) return nullptr;

	Param->Type = ConVarTypes::INT;

	GetConVarArray<int32_t>()->Add(defaultValue, CurrentValue, Param);

	return Param;
}

ConVarParams* lbConVarSystem::CreateConvar(const char* Name, const char* defaultValue, const char* CurrentValue)
{
	std::unique_lock lock(mutex_);
	ConVarParams* Param = RegisterConVar(Name);
	if (!Param) return nullptr;

	Param->Type = ConVarTypes::STRING;

	GetConVarArray<std::string>()->Add(defaultValue, CurrentValue, Param);

	return Param;
}

ConVarParams* lbConVarSystem::RegisterConVar(const char* Name)
{

	uint32_t namehash = StringUtils::StringHash{ Name };
	SavedConVars[namehash] = ConVarParams{};

	ConVarParams& newParam = SavedConVars[namehash];

	newParam.Name = Name;

	return &newParam;
}

template<typename T>
T GetCurrentConVarByIndex(int32_t Index) {
	return lbConVarSystem::Get()->GetConVarArray<T>()->GetCurrent(Index);
}
template<typename T>
T* GetCurrentPtrByIndex(int32_t Index) {
	return lbConVarSystem::Get()->GetConVarArray<T>()->GetCurrentPtr(Index);
}

template<typename T>
void SetCurrentConVarByIndex(int32_t Index, const T& data) {

	lbConVarSystem::Get()->GetConVarArray<T>()->SetCurrent(data, Index);
}

// ConVar_Float
ConVar_Float::ConVar_Float(const char* Name, double defaultValue, ConVarFlags Flags)
{
	ConVarParams* convar = ConVarSystem::Get()->CreateConvar(Name, defaultValue, defaultValue);
	if (Flags == ConVarFlags::None)
	{
		lb::console::Post("Convar ( " + convar->Name + " ) was created with the flag ConVarFlags::None", lb::console::LogLevel::Error);
	}
	convar->Flags = Flags;
	Index = convar->ArrayIndex;
}

double ConVar_Float::Get()
{
	return GetCurrentConVarByIndex<ConVarTypes>(Index);
}

double* ConVar_Float::GetPtr()
{
	return GetCurrentPtrByIndex<ConVarTypes>(Index);
}

float ConVar_Float::GetFloat()
{
	return static_cast<float>(Get());
}

float* ConVar_Float::GetFloatPtr()
{
	float* result = reinterpret_cast<float*>(GetPtr());
	return result;
}

void ConVar_Float::Set(double val)
{
	SetCurrentConVarByIndex<ConVarTypes>(Index, val);
}

// ConVar_Int
ConVar_Int::ConVar_Int(const char* Name, int32_t defaultValue, ConVarFlags Flags)
{
	ConVarParams* convar = ConVarSystem::Get()->CreateConvar(Name, defaultValue, defaultValue);
	if (Flags == ConVarFlags::None)
	{
		lb::console::Post("Convar ( " + convar->Name + " ) was created with the flag ConVarFlags::None", lb::console::LogLevel::Error);
		// return;
	}
	convar->Flags = Flags;
	Index = convar->ArrayIndex;
}

int32_t ConVar_Int::Get()
{
	return GetCurrentConVarByIndex<ConVarTypes>(Index);
}

int32_t* ConVar_Int::GetPtr()
{
	return GetCurrentPtrByIndex<ConVarTypes>(Index);
}

void ConVar_Int::Set(int32_t val)
{
	SetCurrentConVarByIndex<ConVarTypes>(Index, val);
}

// ConVar_String
ConVar_String::ConVar_String(const char* Name, const char* defaultValue, ConVarFlags Flags)
{
	ConVarParams* convar = ConVarSystem::Get()->CreateConvar(Name, defaultValue, defaultValue);
	if (Flags == ConVarFlags::None)
	{
		lb::console::Post("Convar ( " + convar->Name + " ) was created with the flag ConVarFlags::None", lb::console::LogLevel::Error);
		// return;
	}
	convar->Flags = Flags;
	Index = convar->ArrayIndex;
}

const char* ConVar_String::Get()
{
	return GetCurrentConVarByIndex<ConVarTypes>(Index).c_str();
};

void ConVar_String::Set(std::string&& val)
{
	SetCurrentConVarByIndex<ConVarTypes>(Index, val);
}
