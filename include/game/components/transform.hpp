#pragma once

#include "glm/vec2.hpp"

#include "core/component.hpp"

namespace Symbiote {
    namespace Game {

        class TransformComponent : public Symbiote::Core::Component {
        public:
            DECLARE_COMPONENT(Symbiote::Game::TransformComponent);

        public:
            float mRotation;
            glm::vec2 mScale = {1, 1};
            glm::vec2 mPosition = {0, 0};
        };

    }
}