#pragma once

#include <stdexcept>
#include <utility>

#include "emu/application/application_error.h"

namespace emu {

template <typename T>
T Require(Result<T> result) {
	if (!result) throw std::runtime_error(result.error().message);
	return std::move(*result);
}

inline void Require(Result<void> result) {
	if (!result) throw std::runtime_error(result.error().message);
}

}
