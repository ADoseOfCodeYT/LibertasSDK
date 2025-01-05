#pragma once
#include "CommonInclude.h"
#include "lbGraphicsDevice.h"
#include "lbCanvas.h"
#include "lbColor.h"
#include "lbConvars.h"

#include <string>
#include <cassert>

#define lblog_level(str,level,...) {char text[1024] = {}; snprintf(text, sizeof(text), str, ## __VA_ARGS__); lb::console::Post(text, level);}
#define lblog_messagebox(str,...) {char text[1024] = {}; snprintf(text, sizeof(text), str, ## __VA_ARGS__); lb::console::Post(text, lb::console::LogLevel::Error); lb::helper::messageBox(text, "Error!");}
#define lblog_warning(str,...) {lblog_level(str, lb::console::LogLevel::Warning, ## __VA_ARGS__);}
#define lblog_error(str,...) {lblog_level(str, lb::console::LogLevel::Error, ## __VA_ARGS__);}
#define lblog(str,...) {lblog_level(str, lb::console::LogLevel::Default, ## __VA_ARGS__);}
#define lblog_assert(cond,str,...) {if(!cond){lblog_error(str, ## __VA_ARGS__); assert(cond);}}

namespace lb::console
{
	// Do not modify the order, as this is exposed to LUA scripts as int!
	enum class LogLevel
	{
		None,
		Default,
		Warning,
		Error,
	};

	void Toggle();
	void Scroll(int direction);
	void Update(const lb::Canvas& canvas, float dt = 1.0f / 60.0f);
	void Draw(
		const lb::Canvas& canvas,
		lb::graphics::CommandList cmd,
		lb::graphics::ColorSpace colorspace = lb::graphics::ColorSpace::SRGB
	);
	void DrawOutputText(
		const lb::Canvas& canvas,
		lb::graphics::CommandList cmd,
		lb::graphics::ColorSpace colorspace = lb::graphics::ColorSpace::SRGB
	);

	std::string getText();
	void clear();
	void Post(const char* input, LogLevel level = LogLevel::Default);
	void Post(const std::string& input, LogLevel level = LogLevel::Default);

	void HistoryPrev();
	void HistoryNext();

	bool IsActive();

	void Lock();
	void Unlock();

	void SetLogLevel(LogLevel newLevel);

	LogLevel GetUnseenLogLevelMax();


	// These are no longer used, but kept here to not break user code:
	inline void input(const char input) {}
	inline void acceptInput() {}
	inline void deletefromInput() {}
};
