#pragma once

#include <core/system.hpp>

class WorldStateSystem final : public Symbiote::Core::System
{
public:
    DECLARE_SYSTEM(WorldStateSystem);

public:
    WorldStateSystem(char* inputState, char* networkState);

public:
    auto Update() -> void final;

public:
    char* mInputState = nullptr;
    char* mNetworkState = nullptr;
};