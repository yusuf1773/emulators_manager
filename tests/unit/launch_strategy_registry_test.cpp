#include "emu/application/launch_strategy_registry.h"

#include <stdexcept>
#include <string>
#include <utility>

#include "test_support.h"

namespace emu {
namespace {

class StubStrategy final : public LaunchStrategy {
	public:
	explicit StubStrategy(std::string id) : id_(std::move(id)) {}

	LaunchRequest BuildRequest(const LaunchContext&) const override {
		return {};
	}

	LaunchStrategyInfo Info() const override { return {id_, id_, id_}; }

	private:
	std::string id_;
};

void TestEmptyRegistry() {
	LaunchStrategyRegistry registry;
	testing::Check(registry.GetAll().empty(), "New registry must be empty");
	testing::ExpectThrow<std::invalid_argument>(
		[&] { registry.GetById("missing"); }, "Missing strategy must throw");
	testing::Check(registry.GetAll().empty(),
				   "Failed lookup must not change registry");
}

void TestRegistrationAndLookup() {
	LaunchStrategyRegistry registry;
	registry.Add<StubStrategy>("first");
	registry.Add<StubStrategy>("second");

	testing::Check(registry.GetAll().size() == 2,
				   "Registered strategies are missing");
	testing::Check(registry.GetAll()[0]->Info().id == "first" &&
					   registry.GetAll()[1]->Info().id == "second",
				   "Registration order changed");
	testing::Check(registry.GetById("first").Info().id == "first",
				   "Lookup returned the wrong strategy");
	testing::Check(&registry.GetById("second") == registry.GetAll()[1].get(),
				   "Lookup returned a different strategy object");

	testing::ExpectThrow<std::invalid_argument>(
		[&] { registry.Add<StubStrategy>("first"); },
		"Duplicate strategy id must throw");
	testing::Check(registry.GetAll().size() == 2,
				   "Duplicate registration changed registry");
}

}
}

int main() {
	return emu::testing::RunTests([] {
		emu::TestEmptyRegistry();
		emu::TestRegistrationAndLookup();
	});
}
