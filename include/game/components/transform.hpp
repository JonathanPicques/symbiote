#pragma once

#include "glm/vec3.hpp"
#include "glm/ext/quaternion_float.hpp"

#include "core/component.hpp"

namespace Symbiote {
    namespace Game {

        class TransformComponent : public Symbiote::Core::Component {
        public:
            DECLARE_COMPONENT(Symbiote::Game::TransformComponent);

        private:
            glm::vec3 mScale = {1, 1, 1};
            glm::vec3 mPosition = {0, 0, 0};
            glm::quat mRotation = {};
        };

    }
}