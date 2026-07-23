#pragma once

#include "istrategy.hpp"

#include "launch_context.hpp"

class DirectLaunchStrategy : public ILaunchStrategy {
public:
    void Launch(const LaunchContext& request) const override;
    LaunchStrategyInfo Info() const override;
};
