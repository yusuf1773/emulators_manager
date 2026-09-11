#include "emu/application/id_generator.h"

#include <array>
#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>

namespace emu {

std::string RandomIdGenerator::Next() {
	std::random_device source;
	std::mt19937_64 random(source());
	std::array<std::uint8_t, 16> bytes{};
	for (auto& byte : bytes) {
		byte = static_cast<std::uint8_t>(random());
	}
	bytes[6] = static_cast<std::uint8_t>((bytes[6] & 0x0f) | 0x40);
	bytes[8] = static_cast<std::uint8_t>((bytes[8] & 0x3f) | 0x80);

	std::ostringstream value;
	value << std::hex << std::setfill('0');
	for (std::size_t index = 0; index < bytes.size(); ++index) {
		if (index == 4 || index == 6 || index == 8 || index == 10) {
			value << '-';
		}
		value << std::setw(2) << static_cast<int>(bytes[index]);
	}
	return value.str();
}

}
