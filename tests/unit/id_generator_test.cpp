#include "emu/application/id_generator.h"

#include <gtest/gtest.h>

#include <regex>
#include <set>
#include <string>

namespace emu {
namespace {

TEST(RandomIdGeneratorTest, GeneratesDistinctUuidValues) {
	RandomIdGenerator generator;
	const std::regex format(
		"[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}");
	std::set<std::string> values;
	for (int index = 0; index < 100; ++index) {
		const auto value = generator.Next();
		EXPECT_TRUE(std::regex_match(value, format));
		values.insert(value);
	}
	EXPECT_EQ(values.size(), 100);
}

}
}
