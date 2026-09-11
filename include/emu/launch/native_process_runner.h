#pragma once

#include "emu/application/process_runner.h"

namespace emu {

class NativeProcessRunner final : public ProcessRunner {
public:
	Result<ProcessResult> Run(const LaunchRequest& request) override;
};

}
