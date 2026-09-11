#ifndef EMU_MANAGER_INCLUDE_EMU_LAUNCH_SYSTEM_PROCESS_RUNNER_H_
#define EMU_MANAGER_INCLUDE_EMU_LAUNCH_SYSTEM_PROCESS_RUNNER_H_

#include "emu/application/process_runner.h"

namespace emu {

class SystemProcessRunner final : public ProcessRunner {
 public:
  void Run(const LaunchRequest& request) override;
};

}

#endif
