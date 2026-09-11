#ifndef EMU_MANAGER_INCLUDE_EMU_APPLICATION_LAUNCH_STRATEGY_INFO_H_
#define EMU_MANAGER_INCLUDE_EMU_APPLICATION_LAUNCH_STRATEGY_INFO_H_
#include <string>
#include <string_view>

namespace emu {

struct LaunchStrategyInfo {
  std::string_view id;
  std::string_view name;
  std::string_view description;
};

}

#endif
