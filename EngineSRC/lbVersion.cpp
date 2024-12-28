#include "lbVersion.h"

#include <string>

namespace lb::version
{
	// main engine core
	const int major = 0;
	// minor features, major updates, breaking compatibility changes
	const int minor = 71;
	// minor bug fixes, alterations, refactors, updates
	const int revision = 87;

	const std::string version_string = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(revision);

	int GetMajor()
	{
		return major;
	}
	int GetMinor()
	{
		return minor;
	}
	int GetRevision()
	{
		return revision;
	}
	const char* GetVersionString()
	{
		return version_string.c_str();
	}

	const char* GetCreditsString()
	{
		static const char* credits = R"(
Credits
-----------
Made by Ismail Durmaz

Libertas Engine is a fork of Wicked Engine

Wicked Engine : https://github.com/turanszkij/WickedEngine
Wicked Engine Contributors : https://github.com/turanszkij/WickedEngine/graphs/contributors
		)";

		return credits;
	}

}
