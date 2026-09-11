#include "emu/application/emulator_service.h"

#include <gtest/gtest.h>

#include "fakes.h"

namespace emu {
namespace {

TEST(EmulatorServiceTest, CreatesEmulatorWithArguments) {
	testing::MemoryEmulatorRepository repository;
	testing::FixedIdGenerator ids({"emulator-1"});
	EmulatorService service(repository, ids);
	auto result = service.Create("Emulator", "emulator.exe", "console",
								 {"--fullscreen", "two words"});
	ASSERT_TRUE(result);
	EXPECT_EQ(result->id, EmulatorId("emulator-1"));
	EXPECT_EQ(result->base_arguments,
			  (std::vector<std::string>{"--fullscreen", "two words"}));
	EXPECT_EQ(repository.values, std::vector<Emulator>{*result});
}

TEST(EmulatorServiceTest, RejectsInvalidAndDuplicateEmulators) {
	testing::MemoryEmulatorRepository repository;
	repository.values.push_back(
		{EmulatorId("existing"), "Emulator", "emulator.exe", "console", {}});
	testing::FixedIdGenerator ids({"unused"});
	EmulatorService service(repository, ids);
	auto empty_name = service.Create("", "emulator.exe", "console", {});
	auto empty_path = service.Create("Other", {}, "console", {});
	auto duplicate = service.Create("Emulator", "other.exe", "console", {});
	ASSERT_FALSE(empty_name);
	EXPECT_EQ(empty_name.error().code, ErrorCode::kInvalidArgument);
	ASSERT_FALSE(empty_path);
	EXPECT_EQ(empty_path.error().code, ErrorCode::kInvalidArgument);
	ASSERT_FALSE(duplicate);
	EXPECT_EQ(duplicate.error().code, ErrorCode::kDuplicateName);
	EXPECT_EQ(repository.values.size(), 1);
}

TEST(EmulatorServiceTest, RejectsRenameToExistingName) {
	testing::MemoryEmulatorRepository repository;
	repository.values = {{EmulatorId("one"), "One", "one.exe", "console", {}},
						 {EmulatorId("two"), "Two", "two.exe", "console", {}}};
	testing::FixedIdGenerator ids({});
	EmulatorService service(repository, ids);
	auto result =
		service.Update(EmulatorId("two"), "One", "two.exe", "console", {});
	ASSERT_FALSE(result);
	EXPECT_EQ(result.error().code, ErrorCode::kDuplicateName);
	EXPECT_EQ(repository.values[1].name, "Two");
}

}
}
