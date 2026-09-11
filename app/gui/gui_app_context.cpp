#include "gui_app_context.h"

#include "emu/persistence/config.h"

namespace emu {

GuiAppContext::GuiAppContext(const std::filesystem::path& config_path)
	: config_(Config(config_path).GetData()),
	  database_(config_.database_path),
	  game_repository_(database_),
	  emulator_repository_(database_),
	  profile_repository_(database_),
	  history_repository_(database_),
	  game_service_(game_repository_, id_generator_),
	  emulator_service_(emulator_repository_, id_generator_),
	  profile_service_(profile_repository_, game_repository_,
					   emulator_repository_, id_generator_),
	  catalog_transfer_(game_repository_, emulator_repository_,
						profile_repository_, game_service_, emulator_service_,
						profile_service_),
	  game_list_model_(game_service_, errors_),
	  emulator_list_model_(emulator_service_, errors_),
	  profile_list_model_(profile_service_, errors_) {
	if (auto result = history_repository_.Trim(config_.history_max_size);
		!result) {
		errors_.Present(result.error());
	}
	if (auto result = catalog_transfer_.ImportLegacyIfEmpty(
			config_.games_path, config_.emulators_path);
		!result) {
		errors_.Present(result.error());
	}
	game_list_model_.Reload();
	emulator_list_model_.Reload();
}

}
