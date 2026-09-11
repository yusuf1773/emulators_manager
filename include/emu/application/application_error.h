#pragma once

#include <expected>
#include <string>
#include <utility>

namespace emu {

enum class ErrorCode {
	kNotFound,
	kDuplicateName,
	kInvalidArgument,
	kPersistence,
	kProcess,
};

struct ApplicationError {
	ErrorCode code;
	std::string message;

	bool operator==(const ApplicationError&) const = default;
};

template <typename T>
using Result = std::expected<T, ApplicationError>;

inline std::unexpected<ApplicationError> Failure(ErrorCode code,
												 std::string message) {
	return std::unexpected(ApplicationError{code, std::move(message)});
}

}
