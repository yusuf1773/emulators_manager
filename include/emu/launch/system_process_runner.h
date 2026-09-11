#pragma once

#include "emu/application/process_runner.h"

namespace emu {

class SystemProcessRunner final : public ProcessRunner {
	public:
	void Run(const LaunchRequest& request) override;
};

}
