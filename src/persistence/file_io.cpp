#include "emu/persistence/file_io.h"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace emu::persistence {
void WriteFile(const std::filesystem::path& path, std::string_view contents) {
	static std::atomic<unsigned long long> sequence{0};
#ifdef _WIN32
	const auto process_id = GetCurrentProcessId();
#else
	const auto process_id = getpid();
#endif
	auto temporary = path;
	temporary +=
		".tmp." + std::to_string(process_id) + "." +
		std::to_string(
			std::chrono::steady_clock::now().time_since_epoch().count()) +
		"." + std::to_string(sequence++);

	try {
		if (!path.parent_path().empty()) {
			std::filesystem::create_directories(path.parent_path());
		}
		std::ofstream out(temporary, std::ios::binary | std::ios::trunc);
		out.exceptions(std::ios::failbit | std::ios::badbit);
		out.write(contents.data(),
				  static_cast<std::streamsize>(contents.size()));
		out.close();
#ifdef _WIN32
		if (!MoveFileExW(temporary.c_str(), path.c_str(),
						 MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
			throw std::system_error(static_cast<int>(GetLastError()),
									std::system_category(),
									"Replace repository file");
		}
#else
		std::filesystem::rename(temporary, path);
#endif
	} catch (const std::exception& error) {
		std::error_code ignored;
		std::filesystem::remove(temporary, ignored);
		throw std::runtime_error("Failed to save " + path.string() + ": " +
								 error.what());
	}
}
}
