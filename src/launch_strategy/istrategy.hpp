#pragma once

#include "../launch_request/launch_request.hpp"

class ILaunchStrategy {
public:
    virtual ~ILaunchStrategy() = default;
    virtual void Run(const LaunchRequest& request) const = 0;
};