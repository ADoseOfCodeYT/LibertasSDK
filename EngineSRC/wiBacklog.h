#pragma once
#include "CommonInclude.h"
#include "wiGraphicsDevice.h"
#include "wiCanvas.h"
#include "wiColor.h"

#include <string>

namespace lb::backlog
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
	void post(const char* input, LogLevel level = LogLevel::Default);
	void post(const std::string& input, LogLevel level = LogLevel::Default);

	void historyPrev();
	void historyNext();

	bool isActive();

	void setBackground(lb::graphics::Texture* texture);
	void setFontSize(int value);
	void setFontRowspacing(float value);
	void setFontColor(lb::Color color);

	void Lock();
	void Unlock();

	void BlockLuaExecution();
	void UnblockLuaExecution();

	void SetLogLevel(LogLevel newLevel);

	LogLevel GetUnseenLogLevelMax();


	// These are no longer used, but kept here to not break user code:
	inline void input(const char input) {}
	inline void acceptInput() {}
	inline void deletefromInput() {}
};
