#include "emu/application/launch_game_service.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "emu/launch/direct_strategy.h"
#include "test_support.h"

namespace emu {
namespace {

template <typename T>
class ReadOnlyRepository final : public Repository<T> {
	public:
	explicit ReadOnlyRepository(std::vector<T> values)
		: values_(std::move(values)) {}

	const std::vector<T>& GetAll() const override { return values_; }
	const T& GetByName(const std::string& name) const override {
		const auto found =
			std::find_if(values_.begin(), values_.end(),
						 [&](const T& value) { return value.name == name; });
		if (found == values_.end()) throw std::runtime_error("Missing entity");
		return *found;
	}
	void Add(const T&, bool) override {
		throw std::logic_error("Unexpected write");
	}
	void Replace(const std::string&, const T&) override {
		throw std::logic_error("Unexpected write");
	}
	void RemoveByName(const std::string&) override {
		throw std::logic_error("Unexpected write");
	}

	private:
	std::vector<T> values_;
};

class RecordingRunner final : public ProcessRunner {
	public:
	void Run(const LaunchRequest& request) override {
		requests.push_back(request);
		if (fail) throw std::runtime_error("Cannot launch");
	}
	std::vector<LaunchRequest> requests;
	bool fail = false;
};

void TestLaunch() {
	const Game game{"Game", "games/game with spaces.rom", "console"};
	const Emulator emulator{
		"Emulator",
		"emulators/path with spaces.exe",
		"console",
		"direct",
		{"--flag", "argument with spaces", "\"quoted\"", ""}};
	ReadOnlyRepository<Game> games({game});
	ReadOnlyRepository<Emulator> emulators({emulator});
	LaunchStrategyRegistry strategies;
	strategies.Add<DirectLaunchStrategy>();
	RecordingRunner runner;
	LaunchGameService service(games, emulators, strategies, runner);

	service.Launch("Game", "Emulator");
	testing::Check(runner.requests.size() == 1,
				   "Runner must be called exactly once");
	testing::Check(runner.requests[0].executable == emulator.path,
				   "Executable changed");
	testing::Check(
		runner.requests[0].arguments ==
			std::vector<std::string>{game.path.string(), "--flag",
									 "argument with spaces", "\"quoted\"", ""},
		"Arguments or their ordering changed");

	testing::ExpectThrow<std::runtime_error>(
		[&] { service.Launch("missing", "Emulator"); },
		"Missing game must fail");
	testing::ExpectThrow<std::runtime_error>(
		[&] { service.Launch("Game", "missing"); },
		"Missing emulator must fail");
	testing::Check(runner.requests.size() == 1,
				   "Lookup failure must not launch");
	testing::ExpectThrow<std::invalid_argument>(
		[&] { strategies.Add<DirectLaunchStrategy>(); },
		"Duplicate strategy must fail");

	LaunchStrategyRegistry empty_strategies;
	LaunchGameService unknown(games, emulators, empty_strategies, runner);
	testing::ExpectThrow<std::invalid_argument>(
		[&] { unknown.Launch("Game", "Emulator"); },
		"Unknown strategy must fail");
	testing::Check(runner.requests.size() == 1,
				   "Unknown strategy must not launch");

	auto invalid = emulator;
	invalid.path.clear();
	ReadOnlyRepository<Emulator> invalid_emulators({invalid});
	LaunchGameService invalid_service(games, invalid_emulators, strategies,
									  runner);
	testing::ExpectThrow<std::invalid_argument>(
		[&] { invalid_service.Launch("Game", "Emulator"); },
		"Empty executable must fail");
	testing::Check(runner.requests.size() == 1,
				   "Invalid request must not launch");

	runner.fail = true;
	testing::ExpectThrow<std::runtime_error>(
		[&] { service.Launch("Game", "Emulator"); },
		"Runner failure must reach the caller");
}

}
}

int main() { return emu::testing::RunTests(emu::TestLaunch); }
