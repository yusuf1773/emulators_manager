#include "bootstrap/app_context.h"

#include <stdexcept>
#include <utility>

namespace emu {
namespace {

void Require(const Result<void>& result) {
	if (!result) throw std::runtime_error(result.error().message);
}

}

AppContext::AppContext(ConfigData config)
	: config_(std::move(config)),
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
						profile_service_) {
	Require(history_repository_.Trim(config_.history_max_size));
	Require(catalog_transfer_.ImportLegacyIfEmpty(config_.games_path,
												  config_.emulators_path));
}

}
