#ifndef EMU_MANAGER_INCLUDE_EMU_APPLICATION_LAUNCH_STRATEGY_H_
#define EMU_MANAGER_INCLUDE_EMU_APPLICATION_LAUNCH_STRATEGY_H_

#include "emu/application/launch_context.h"
#include "emu/application/launch_strategy_info.h"
#include "emu/application/process_runner.h"

namespace emu {

class LaunchStrategy {
 public:
  virtual ~LaunchStrategy() = default;
  virtual LaunchRequest BuildRequest(const LaunchContext& context) const = 0;
  virtual LaunchStrategyInfo Info() const = 0;
};

}

#endif
