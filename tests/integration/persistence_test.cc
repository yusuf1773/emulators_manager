#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

#include "emu/persistence/config.h"
#include "emu/persistence/json_history_repository.h"
#include "emu/persistence/json_repository.h"
#include "test_support.h"

namespace emu {
namespace {

const std::string& GameName(const Game& game) { return game.name; }
const std::string& EmulatorName(const Emulator& emulator) {
  return emulator.name;
}

std::string ReadFile(const std::filesystem::path& path) {
  std::ifstream input(path);
  return {std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>()};
}

void TestRepositories() {
  testing::TemporaryDirectory directory;
  const auto path = directory.path() / "nested" / "games.json";
  JsonRepository<Game> games(path, GameName);
  const Game original{"A", "first.rom", "console"};
  const Game replacement{"A", "second with spaces.rom", "console"};
  games.Add(original);
  games.Add(replacement, true);
  JsonRepository<Game> reloaded(path, GameName);
  testing::Check(reloaded.GetByName("A") == replacement,
                 "Upsert was not saved");

  testing::ExpectThrow<std::runtime_error>([&] { games.Add(original); },
                                           "Duplicate accepted");
  const Game other{"B", "other.rom", "console"};
  games.Add(other);
  const auto before = ReadFile(path);
  testing::ExpectThrow<std::runtime_error>([&] { games.Replace("A", other); },
                                           "Rename collision accepted");
  testing::Check(games.GetByName("A") == replacement,
                 "Failed rename changed memory");
  testing::Check(ReadFile(path) == before, "Failed rename changed file");

  games.Replace("A", {"C", "new.rom", "console"});
  JsonRepository<Game> renamed(path, GameName);
  testing::Check(renamed.GetAll().back().name == "C",
                 "Rename ordering or persistence changed");
  testing::ExpectThrow<std::runtime_error>([&] { renamed.GetByName("A"); },
                                           "Old name remained");
  games.RemoveByName("C");
  JsonRepository<Game> removed(path, GameName);
  testing::Check(removed.GetAll().size() == 1 && removed.GetAll()[0] == other,
                 "Removal not persisted");

  const auto emulator_path = directory.path() / "emulators.json";
  JsonRepository<Emulator> emulators(emulator_path, EmulatorName);
  const Emulator emulator{
      "E", "emulator.exe", "console", "direct", {"--arg", "two words"}};
  emulators.Add(emulator);
  JsonRepository<Emulator> emulators_reloaded(emulator_path, EmulatorName);
  testing::Check(emulators_reloaded.GetByName("E") == emulator,
                 "Emulator format changed");

  const auto malformed = directory.path() / "bad.json";
  std::ofstream(malformed) << "{broken";
  testing::ExpectThrow<std::runtime_error>(
      [&] { JsonRepository<Game> bad(malformed, GameName); },
      "Malformed JSON silently accepted");
  testing::Check(ReadFile(malformed) == "{broken",
                 "Malformed JSON overwritten");

  std::filesystem::remove(path);
  std::filesystem::create_directory(path);
  testing::ExpectThrow<std::runtime_error>([&] { games.Add(original); },
                                           "Write failure ignored");
  testing::Check(games.GetAll().size() == 1, "Write failure changed memory");
  testing::Check(std::filesystem::is_directory(path),
                 "Write failure replaced the target");
  for (const auto& entry :
       std::filesystem::directory_iterator(path.parent_path())) {
    testing::Check(entry.path().filename() == "games.json",
                   "Temporary write file leaked");
  }
}

void TestHistoryAndConfig() {
  testing::TemporaryDirectory directory;
  const auto path = directory.path() / "history.json";
  const auto timestamp =
      std::chrono::system_clock::time_point(std::chrono::seconds(123));
  JsonHistoryRepository history(path, 2);
  history.Add({"one", timestamp});
  history.Add({"two", timestamp});
  history.Add({"three", timestamp});
  JsonHistoryRepository reloaded(path, 2);
  testing::Check(reloaded.GetHistories().size() == 2, "History limit ignored");
  testing::Check(reloaded.GetHistories()[0].command == "three" &&
                     reloaded.GetHistories()[1].command == "two",
                 "History ordering changed");
  testing::Check(reloaded.GetHistories()[0].timestamp == timestamp,
                 "Timestamp format changed");
  JsonHistoryRepository reduced(path, 1);
  testing::Check(reduced.GetHistories().size() == 1,
                 "Reduced history limit ignored");
  JsonHistoryRepository disabled(path, 0);
  disabled.Add({"ignored", timestamp});
  testing::Check(disabled.GetHistories().empty(),
                 "Zero limit must disable history");
  JsonHistoryRepository empty_again(path, 2);
  testing::Check(empty_again.GetHistories().empty(),
                 "Zero limit was not persisted");

  const auto config_path = directory.path() / "config" / "config.json";
  Config defaults(config_path);
  testing::Check(
      defaults.GetData().games_path == config_path.parent_path() / "games.json",
      "Relative path is not resolved against the config directory");
  ConfigData custom;
  custom.games_path = directory.path() / "absolute.json";
  custom.emulators_path = "data/emulators.json";
  std::ofstream(config_path) << nlohmann::json(custom);
  Config configured(config_path);
  testing::Check(configured.GetData().games_path == custom.games_path,
                 "Absolute path changed");
  testing::Check(configured.GetData().emulators_path ==
                     config_path.parent_path() / custom.emulators_path,
                 "Nested relative path resolution changed");
}

}
}

int main() {
  return emu::testing::RunTests([] {
    emu::TestRepositories();
    emu::TestHistoryAndConfig();
  });
}
