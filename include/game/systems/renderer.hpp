#pragma once

#include "core/system.hpp"

namespace Symbiote {
    namespace Game {

        class RendererSystem final : public Symbiote::Core::System {
        public:
            DECLARE_SYSTEM(Symbiote::Game::RendererSystem);

        public:
            RendererSystem();
        };

    }
}