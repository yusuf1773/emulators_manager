#ifndef EMU_MANAGER_INCLUDE_EMU_APPLICATION_PROCESS_RUNNER_H_
#define EMU_MANAGER_INCLUDE_EMU_APPLICATION_PROCESS_RUNNER_H_

#include <filesystem>
#include <string>
#include <vector>

namespace emu {

struct LaunchRequest {
  std::filesystem::path executable;
  std::vector<std::string> arguments;
};

class ProcessRunner {
 public:
  virtual ~ProcessRunner() = default;

  virtual void Run(const LaunchRequest& request) = 0;
};

}

#endif
