#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "emu/application/application_error.h"
#include "emu/domain/emulator.h"

namespace emu {

class EmulatorRepository {
public:
	virtual ~EmulatorRepository() = default;
	virtual Result<std::vector<Emulator>> FindAll() const = 0;
	virtual Result<std::optional<Emulator>> FindById(
		const EmulatorId& id) const = 0;
	virtual Result<std::optional<Emulator>> FindByName(
		std::string_view name) const = 0;
	virtual Result<void> Save(const Emulator& emulator) = 0;
	virtual Result<void> Remove(const EmulatorId& id) = 0;
};

}
