#ifndef EMU_MANAGER_INCLUDE_EMU_LAUNCH_DIRECT_STRATEGY_H_
#define EMU_MANAGER_INCLUDE_EMU_LAUNCH_DIRECT_STRATEGY_H_

#include "emu/application/launch_context.h"
#include "emu/application/launch_strategy.h"

namespace emu {

class DirectLaunchStrategy : public LaunchStrategy {
 public:
  LaunchRequest BuildRequest(const LaunchContext& context) const override;
  LaunchStrategyInfo Info() const override;
};

}

#endif
