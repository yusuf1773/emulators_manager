#ifndef EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_CONFIG_H_
#define EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_CONFIG_H_

#include <filesystem>

#include "emu/persistence/config_data.h"

namespace emu {

class Config {
 public:
  explicit Config(std::filesystem::path path);

  const ConfigData& GetData() const;

 private:
  void Load();
  void SaveDefault();

  std::filesystem::path path_;
  ConfigData data_;
};

}

#endif
