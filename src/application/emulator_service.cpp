#include "emu/application/emulator_service.h"

#include <utility>

namespace emu {
namespace {

Result<void> ValidateEmulator(std::string_view name,
							  const std::filesystem::path& executable_path) {
	if (name.empty()) {
		return Failure(ErrorCode::kInvalidArgument, "Emulator name is empty");
	}
	if (executable_path.empty()) {
		return Failure(ErrorCode::kInvalidArgument, "Executable path is empty");
	}
	return {};
}

}

EmulatorService::EmulatorService(EmulatorRepository& emulators,
								 IdGenerator& ids)
	: emulators_(emulators), ids_(ids) {}

Result<Emulator> EmulatorService::Create(
	std::string name, std::filesystem::path executable_path,
	std::string platform, std::vector<std::string> base_arguments) {
	if (auto validation = ValidateEmulator(name, executable_path);
		!validation) {
		return std::unexpected(validation.error());
	}
	auto existing = emulators_.FindByName(name);
	if (!existing) return std::unexpected(existing.error());
	if (*existing) {
		return Failure(ErrorCode::kDuplicateName,
					   "Emulator name already exists");
	}
	Emulator emulator{EmulatorId(ids_.Next()), std::move(name),
					  std::move(executable_path), std::move(platform),
					  std::move(base_arguments)};
	if (auto saved = emulators_.Save(emulator); !saved) {
		return std::unexpected(saved.error());
	}
	return emulator;
}

Result<Emulator> EmulatorService::Update(
	EmulatorId id, std::string name, std::filesystem::path executable_path,
	std::string platform, std::vector<std::string> base_arguments) {
	if (auto validation = ValidateEmulator(name, executable_path);
		!validation) {
		return std::unexpected(validation.error());
	}
	auto current = emulators_.FindById(id);
	if (!current) return std::unexpected(current.error());
	if (!*current) return Failure(ErrorCode::kNotFound, "Emulator not found");
	auto duplicate = emulators_.FindByName(name);
	if (!duplicate) return std::unexpected(duplicate.error());
	if (*duplicate && (*duplicate)->id != id) {
		return Failure(ErrorCode::kDuplicateName,
					   "Emulator name already exists");
	}
	Emulator emulator{std::move(id), std::move(name),
					  std::move(executable_path), std::move(platform),
					  std::move(base_arguments)};
	if (auto saved = emulators_.Save(emulator); !saved) {
		return std::unexpected(saved.error());
	}
	return emulator;
}

Result<void> EmulatorService::Remove(const EmulatorId& id) {
	return emulators_.Remove(id);
}

Result<std::vector<Emulator>> EmulatorService::List() const {
	return emulators_.FindAll();
}

Result<std::optional<Emulator>> EmulatorService::FindById(
	const EmulatorId& id) const {
	return emulators_.FindById(id);
}

Result<std::optional<Emulator>> EmulatorService::FindByName(
	std::string_view name) const {
	return emulators_.FindByName(name);
}

}
