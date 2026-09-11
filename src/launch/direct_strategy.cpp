#include "emu/launch/direct_strategy.h"

#include <stdexcept>

namespace emu {

LaunchRequest DirectLaunchStrategy::BuildRequest(
	const LaunchContext& context) const {
	if (context.emu.path.empty()) {
		throw std::invalid_argument("Executable path is empty");
	}
	LaunchRequest request{context.emu.path, {context.game.path.string()}};
	request.arguments.insert(request.arguments.end(), context.emu.args.begin(),
							 context.emu.args.end());
	return request;
}

LaunchStrategyInfo DirectLaunchStrategy::Info() const {
	return {"direct", "Direct", "Launches the emulator executable directly."};
}

}
