#pragma once

#include "launch_strategy_info.h"
#include "launch_context.hpp"

class ILaunchStrategy {
public:
    virtual ~ILaunchStrategy() = default;
    virtual void Launch(const LaunchContext& request) const = 0;
    virtual LaunchStrategyInfo Info() const = 0;
};