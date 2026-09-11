#include "emu/launch/native_process_runner.h"

#include <gtest/gtest.h>

#include <filesystem>

#include "temporary_directory.h"

namespace emu {
namespace {

TEST(NativeProcessRunnerTest, RunsProcessWithArgumentsAndWorkingDirectory) {
	testing::TemporaryDirectory directory;
	NativeProcessRunner runner;
#ifdef _WIN32
	const auto output = directory.path() / "result.txt";
	LaunchRequest request{"C:\\Windows\\System32\\cmd.exe",
						  {"/C", "cd > result.txt"},
						  directory.path()};
#else
	const auto output = directory.path() / "result.txt";
	LaunchRequest request{"/usr/bin/touch", {"result.txt"}, directory.path()};
#endif
	auto result = runner.Run(request);
	ASSERT_TRUE(result);
	EXPECT_EQ(result->exit_code, 0);
	EXPECT_TRUE(std::filesystem::exists(output));
}

TEST(NativeProcessRunnerTest, ReportsEmptyExecutable) {
	NativeProcessRunner runner;
	auto result = runner.Run({});
	ASSERT_FALSE(result);
	EXPECT_EQ(result.error().code, ErrorCode::kInvalidArgument);
}

TEST(NativeProcessRunnerTest, ReportsMissingExecutable) {
	NativeProcessRunner runner;
	auto result = runner.Run({"missing-emulator-executable", {}, {}});
	ASSERT_FALSE(result);
	EXPECT_EQ(result.error().code, ErrorCode::kProcess);
}

}
}
