#ifndef EMU_MANAGER_INCLUDE_EMU_DOMAIN_EMULATOR_H_
#define EMU_MANAGER_INCLUDE_EMU_DOMAIN_EMULATOR_H_

#include <filesystem>
#include <string>
#include <vector>

namespace emu {

struct Emulator {
  std::string name;
  std::filesystem::path path;
  std::string platform;
  std::string launch_strategy_id;
  std::vector<std::string> args;

  bool operator==(const Emulator& emulator) const = default;
};

}

#endif
