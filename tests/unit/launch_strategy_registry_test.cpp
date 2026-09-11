#include "emu/application/launch_strategy_registry.h"

#include <gtest/gtest.h>

#include <string>
#include <utility>

namespace emu {
namespace {

class StubStrategy final : public LaunchStrategy {
public:
	explicit StubStrategy(std::string id) : id_(std::move(id)) {}
	Result<LaunchRequest> BuildRequest(const LaunchContext&) const override {
		return LaunchRequest{};
	}
	LaunchStrategyInfo Info() const override { return {id_, id_, id_}; }

private:
	std::string id_;
};

TEST(LaunchStrategyRegistryTest, RegistersAndFindsStrategies) {
	LaunchStrategyRegistry registry;
	registry.Add<StubStrategy>("first");
	registry.Add<StubStrategy>("second");
	ASSERT_EQ(registry.GetAll().size(), 2);
	EXPECT_EQ(registry.GetById("first").Info().id, "first");
	EXPECT_EQ(registry.GetAll()[1]->Info().id, "second");
}

TEST(LaunchStrategyRegistryTest, RejectsDuplicateAndMissingIds) {
	LaunchStrategyRegistry registry;
	registry.Add<StubStrategy>("first");
	EXPECT_THROW(registry.Add<StubStrategy>("first"), std::invalid_argument);
	EXPECT_THROW(registry.GetById("missing"), std::invalid_argument);
	EXPECT_EQ(registry.GetAll().size(), 1);
}

}
}
