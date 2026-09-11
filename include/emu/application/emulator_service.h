#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "emu/application/emulator_repository.h"
#include "emu/application/id_generator.h"

namespace emu {

class EmulatorService {
public:
	EmulatorService(EmulatorRepository& emulators, IdGenerator& ids);

	Result<Emulator> Create(std::string name,
							std::filesystem::path executable_path,
							std::string platform,
							std::vector<std::string> base_arguments);
	Result<Emulator> Update(EmulatorId id, std::string name,
							std::filesystem::path executable_path,
							std::string platform,
							std::vector<std::string> base_arguments);
	Result<void> Remove(const EmulatorId& id);
	Result<std::vector<Emulator>> List() const;
	Result<std::optional<Emulator>> FindById(const EmulatorId& id) const;
	Result<std::optional<Emulator>> FindByName(std::string_view name) const;

private:
	EmulatorRepository& emulators_;
	IdGenerator& ids_;
};

}
