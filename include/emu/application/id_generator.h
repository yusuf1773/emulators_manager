#pragma once

#include <string>

namespace emu {

class IdGenerator {
public:
	virtual ~IdGenerator() = default;
	virtual std::string Next() = 0;
};

class RandomIdGenerator final : public IdGenerator {
public:
	std::string Next() override;
};

}
