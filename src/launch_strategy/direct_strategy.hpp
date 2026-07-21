#pragma once

#include "istrategy.hpp"

#include "../launch_request/launch_request.hpp"

class DirectLaunchStrategy : public ILaunchStrategy {
public:
    void Run(const LaunchRequest& request) const override;
};
