#ifndef EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_FILE_IO_H_
#define EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_FILE_IO_H_

#include <filesystem>
#include <string>
#include <string_view>

namespace emu::persistence {

void WriteFile(const std::filesystem::path& path, std::string_view contents);
}

#endif
