#pragma once

#include "core/component.hpp"

namespace Symbiote {
    namespace Game {

        class RigidBodyComponent : public Symbiote::Core::Component {
        public:
            DECLARE_COMPONENT(Symbiote::Game::RigidBodyComponent);

        public:
            float mSpeed;
        };

    }
}