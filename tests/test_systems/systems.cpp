#include "systems.hpp"

DEFINE_SYSTEM(WorldStateSystem);

WorldStateSystem::WorldStateSystem(char* inputState, char* networkState) : mInputState(inputState), mNetworkState(networkState)
{

}

auto WorldStateSystem::Update() -> void
{

}