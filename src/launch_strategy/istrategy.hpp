#pragma once

#include "launch_strategy_info.h"
#include "../launch_request/launch_request.hpp"

class ILaunchStrategy {
public:
    virtual ~ILaunchStrategy() = default;
    virtual void Run(const LaunchRequest& request) const = 0;
    virtual LaunchStrategyInfo Info() const = 0;
};