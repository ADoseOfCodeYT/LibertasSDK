#pragma once
#include "Utility/string_utils.h"

// Based on https://github.com/vblanco20-1/vulkan-guide/blob/engine/extra-engine/cvars.h

class ConVarParams;

enum class ConVarFlags : uint32_t
{
	None = 0,
	Protected = 1 << 1,
	UnProtected = 1 << 2,
};

enum class ConVarTypes : char
{
	INT,
	FLOAT,
	STRING,
};

class ConVarSystem
{

public:
	// Returns a reference to lbConVarSystem that was created in lbConvars.cpp
	static ConVarSystem* Get(); 

	virtual ConVarParams* FindConVar(StringUtils::StringHash hash) = 0;

	virtual double* GetFloatConVarVal(StringUtils::StringHash hash) = 0;
	virtual int32_t* GetIntConVarVal(StringUtils::StringHash hash) = 0;
	virtual const char* GetStringConVarVal(StringUtils::StringHash hash) = 0;

	// This is safe because it respects the Protected flag
	virtual void SetConVarValueSafe(StringUtils::StringHash hash, double value) = 0;
	virtual void SetConVarValueSafe(StringUtils::StringHash hash, int32_t value) = 0;
	virtual void SetConVarValueSafe(StringUtils::StringHash hash, const char* value) = 0;

	// This is an internal function DO NOT USE
	virtual ConVarParams* CreateConvar(const char* Name, double defaultValue, double CurrentValue) = 0;
	virtual ConVarParams* CreateConvar(const char* Name, int32_t defaultValue, int32_t CurrentValue) = 0;
	virtual ConVarParams* CreateConvar(const char* Name, const char* defaultValue, const char* CurrentValue) = 0;
};

template<typename T>
struct ConVar_Base
{
protected:
	int Index;
	using ConVarTypes = T;
};

struct ConVar_Float : ConVar_Base<double>
{
	ConVar_Float(const char* Name, double defaultValue, ConVarFlags Flags = ConVarFlags::UnProtected);

	double Get();
	double* GetPtr();
	float GetFloat();
	float* GetFloatPtr();
	// Doesnt respect the Protected flag
	void Set(double val);
};

struct ConVar_Int : ConVar_Base<int32_t>
{
	ConVar_Int(const char* Name, int32_t defaultValue, ConVarFlags Flags = ConVarFlags::UnProtected);
	int32_t Get();
	int32_t* GetPtr();
	// Doesnt respect the Protected flag
	void Set(int32_t val); 
};

struct ConVar_String : ConVar_Base<std::string>
{
	ConVar_String(const char* Name, const char* defaultValue, ConVarFlags Flags = ConVarFlags::UnProtected);

	const char* Get();
	// Doesnt respect the Protected flag
	void Set(std::string&& val); 
};
