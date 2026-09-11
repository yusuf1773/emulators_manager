#pragma once
#include <string>
#include <string_view>

namespace emu {

struct LaunchStrategyInfo {
	std::string_view id;
	std::string_view name;
	std::string_view description;
};

}
