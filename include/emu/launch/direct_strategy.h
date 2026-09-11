#pragma once

#include "emu/application/launch_context.h"
#include "emu/application/launch_strategy.h"

namespace emu {

class DirectLaunchStrategy : public LaunchStrategy {
public:
	Result<LaunchRequest> BuildRequest(
		const LaunchContext& context) const override;
	LaunchStrategyInfo Info() const override;
};

}
