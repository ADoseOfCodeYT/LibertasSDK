#include "lbArguments.h"
#include "lbHelper.h"
#include "lbUnorderedSet.h"

#include <sstream>
#include <iterator>

namespace lb::arguments
{
	lb::unordered_set<std::string> params;

	void Parse(const wchar_t* args)
	{
		std::wstring from = args;
		std::string to;
		lb::helper::StringConvert(from, to);

		std::istringstream iss(to);

		params =
		{
			std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>{}
		};

	}

	void Parse(int argc, char *argv[])
    {
		for (int i = 1; i < argc; i++)
		{
			params.insert(std::string(argv[i]));
		}
    }

	bool HasArgument(const std::string& value)
	{
		return params.find(value) != params.end();
	}
}
