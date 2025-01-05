#ifndef LIBERTASENGINE_VERSION_DEFINED
#define LIBERTASENGINE_VERSION_DEFINED

namespace lb::version
{
	// main engine core
	const int major = 1;
	// minor features, major updates, breaking compatibility changes
	const int minor = 71;
	// minor bug fixes, alterations, refactors, updates
	const int revision = 96;

	static const char* credits = R"(
Credits
-----------
Made by Ismail Durmaz

Libertas Engine is a fork of Wicked Engine

Wicked Engine : https://github.com/turanszkij/WickedEngine
Wicked Engine Contributors : https://github.com/turanszkij/WickedEngine/graphs/contributors
		)";

	long GetVersion();
	// major features
	int GetMajor();
	// minor features, major bug fixes
	int GetMinor();
	// minor bug fixes, alterations
	int GetRevision();
	const char* GetVersionString();

	const char* GetCreditsString();
}

#endif // LIBERTASENGINE_VERSION_DEFINED
