#pragma once

#include <filesystem>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

namespace emu::testing {

inline void Check(bool condition, const std::string& message) {
	if (!condition) throw std::runtime_error(message);
}

template <typename Exception, typename Function>
void ExpectThrow(Function function, const std::string& message) {
	try {
		function();
	} catch (const Exception&) {
		return;
	}
	throw std::runtime_error(message);
}

class TemporaryDirectory {
	public:
	TemporaryDirectory() {
		std::random_device random;
		for (int attempt = 0; attempt < 100; ++attempt) {
			path_ = std::filesystem::temp_directory_path() /
					("emu-test-" + std::to_string(random()));
			if (std::filesystem::create_directory(path_)) return;
		}
		throw std::runtime_error("Cannot create temporary directory");
	}

	~TemporaryDirectory() {
		std::error_code ignored;
		std::filesystem::remove_all(path_, ignored);
	}

	TemporaryDirectory(const TemporaryDirectory&) = delete;
	TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;

	const std::filesystem::path& path() const { return path_; }

	private:
	std::filesystem::path path_;
};

template <typename Function>
int RunTests(Function function) {
	try {
		function();
		return 0;
	} catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}

}
