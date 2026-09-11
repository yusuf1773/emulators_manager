#ifndef EMU_MANAGER_INCLUDE_EMU_DOMAIN_GAME_H_
#define EMU_MANAGER_INCLUDE_EMU_DOMAIN_GAME_H_

#include <filesystem>
#include <string>

namespace emu {

struct Game {
  std::string name;
  std::filesystem::path path;
  std::string platform;

  bool operator==(const Game& game) const = default;
};

}

#endif
