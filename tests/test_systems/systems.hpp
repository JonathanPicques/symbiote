#pragma once

#include <core/ecs/system.hpp>

class WorldStateSystem final : public Symbiote::Core::System {
public:
	DECLARE_SYSTEM(WorldStateSystem);

public:
	WorldStateSystem(char *inputState, char *networkState);

public:
	char *mInputState = nullptr;
	char *mNetworkState = nullptr;
};