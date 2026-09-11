#include "emu/launch/native_process_runner.h"

#include <string>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#endif

namespace emu {
namespace {

#ifdef _WIN32
std::wstring QuoteWindowsArgument(const std::wstring& argument) {
	if (argument.find_first_of(L" \t\"") == std::wstring::npos) return argument;
	std::wstring quoted = L"\"";
	std::size_t slashes = 0;
	for (const wchar_t value : argument) {
		if (value == L'\\') {
			++slashes;
		} else if (value == L'"') {
			quoted.append(slashes * 2 + 1, L'\\');
			quoted.push_back(value);
			slashes = 0;
		} else {
			quoted.append(slashes, L'\\');
			quoted.push_back(value);
			slashes = 0;
		}
	}
	quoted.append(slashes * 2, L'\\');
	quoted.push_back(L'"');
	return quoted;
}

std::wstring BuildWindowsCommandLine(const LaunchRequest& request) {
	std::wstring command = QuoteWindowsArgument(request.executable.wstring());
	for (const auto& argument : request.arguments) {
		command.push_back(L' ');
		command +=
			QuoteWindowsArgument(std::filesystem::path(argument).wstring());
	}
	return command;
}
#endif

}

Result<ProcessResult> NativeProcessRunner::Run(const LaunchRequest& request) {
	if (request.executable.empty()) {
		return Failure(ErrorCode::kInvalidArgument, "Executable path is empty");
	}
#ifdef _WIN32
	std::wstring command = BuildWindowsCommandLine(request);
	STARTUPINFOW startup{};
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION process{};
	const auto working_directory = request.working_directory.wstring();
	if (!CreateProcessW(
			request.executable.c_str(), command.data(), nullptr, nullptr, FALSE,
			0, nullptr,
			working_directory.empty() ? nullptr : working_directory.c_str(),
			&startup, &process)) {
		return Failure(ErrorCode::kProcess, "CreateProcessW failed: " +
												std::to_string(GetLastError()));
	}
	WaitForSingleObject(process.hProcess, INFINITE);
	DWORD exit_code = 0;
	GetExitCodeProcess(process.hProcess, &exit_code);
	CloseHandle(process.hThread);
	CloseHandle(process.hProcess);
	return ProcessResult{static_cast<int>(exit_code)};
#else
	std::vector<std::string> values;
	values.reserve(request.arguments.size() + 1);
	values.push_back(request.executable.string());
	values.insert(values.end(), request.arguments.begin(),
				  request.arguments.end());
	std::vector<char*> arguments;
	arguments.reserve(values.size() + 1);
	for (auto& value : values) arguments.push_back(value.data());
	arguments.push_back(nullptr);

	int failure_pipe[2];
	if (pipe(failure_pipe) != 0) {
		return Failure(ErrorCode::kProcess, "Cannot create process error pipe");
	}
	if (fcntl(failure_pipe[1], F_SETFD, FD_CLOEXEC) == -1) {
		close(failure_pipe[0]);
		close(failure_pipe[1]);
		return Failure(ErrorCode::kProcess,
					   "Cannot configure process error pipe");
	}
	const pid_t process = fork();
	if (process < 0) {
		close(failure_pipe[0]);
		close(failure_pipe[1]);
		return Failure(ErrorCode::kProcess, "Cannot create process");
	}
	if (process == 0) {
		close(failure_pipe[0]);
		if (!request.working_directory.empty() &&
			chdir(request.working_directory.c_str()) != 0) {
			const int error = errno;
			write(failure_pipe[1], &error, sizeof(error));
			_exit(127);
		}
		execv(request.executable.c_str(), arguments.data());
		const int error = errno;
		write(failure_pipe[1], &error, sizeof(error));
		_exit(127);
	}
	close(failure_pipe[1]);
	int child_error = 0;
	const ssize_t failure_size =
		read(failure_pipe[0], &child_error, sizeof(child_error));
	close(failure_pipe[0]);
	if (failure_size == static_cast<ssize_t>(sizeof(child_error))) {
		waitpid(process, nullptr, 0);
		return Failure(
			ErrorCode::kProcess,
			"Cannot start process: " + std::string(std::strerror(child_error)));
	}
	if (failure_size < 0) {
		waitpid(process, nullptr, 0);
		return Failure(ErrorCode::kProcess, "Cannot read process start result");
	}
	int status = 0;
	if (waitpid(process, &status, 0) < 0) {
		return Failure(ErrorCode::kProcess, "Cannot wait for process");
	}
	if (WIFEXITED(status)) return ProcessResult{WEXITSTATUS(status)};
	return Failure(ErrorCode::kProcess, "Process terminated abnormally");
#endif
}

}
