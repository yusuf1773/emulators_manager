#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace emu::persistence {

void WriteFile(const std::filesystem::path& path, std::string_view contents);
}
