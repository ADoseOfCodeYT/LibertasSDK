#pragma once
#include "CommonInclude.h"
#include "lbGraphicsDevice.h"
#include "lbCanvas.h"
#include "lbColor.h"

#include <string>

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

	void setBackground(lb::graphics::Texture* texture);
	void setFontSize(int value);
	void setFontRowspacing(float value);
	void setFontColor(lb::Color color);

	void Lock();
	void Unlock();

	void SetLogLevel(LogLevel newLevel);

	LogLevel GetUnseenLogLevelMax();


	// These are no longer used, but kept here to not break user code:
	inline void input(const char input) {}
	inline void acceptInput() {}
	inline void deletefromInput() {}
};
