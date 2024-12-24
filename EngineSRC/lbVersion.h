#ifndef LIBERTASENGINE_VERSION_DEFINED
#define LIBERTASENGINE_VERSION_DEFINED

namespace lb::version
{
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
