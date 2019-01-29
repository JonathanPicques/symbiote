#pragma once

#include "core/system.hpp"

namespace Symbiote {
    namespace Game {

        class PhysicsSystem final : public Symbiote::Core::System {
        public:
            DECLARE_SYSTEM(Symbiote::Game::PhysicsSystem);

        public:
            auto Update(float deltaTime) -> void;
        };

    }
}