#include "emu/launch/direct_strategy.h"

namespace emu {

Result<LaunchRequest> DirectLaunchStrategy::BuildRequest(
	const LaunchContext& context) const {
	if (context.emu.executable_path.empty()) {
		return Failure(ErrorCode::kInvalidArgument, "Executable path is empty");
	}
	LaunchRequest request{context.emu.executable_path,
						  {context.game.content_path.string()},
						  context.profile.working_directory};
	request.arguments.insert(request.arguments.end(),
							 context.emu.base_arguments.begin(),
							 context.emu.base_arguments.end());
	request.arguments.insert(request.arguments.end(),
							 context.profile.arguments.begin(),
							 context.profile.arguments.end());
	return request;
}

LaunchStrategyInfo DirectLaunchStrategy::Info() const {
	return {"direct", "Direct", "Launches the emulator executable directly."};
}

}
