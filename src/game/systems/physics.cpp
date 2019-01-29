#include <iostream>

#include "core/entity.hpp"
#include "core/entitymanager.hpp"

#include "game/systems/physics.hpp"
#include "game/components/rigidbody.hpp"
#include "game/components/transform.hpp"

DEFINE_SYSTEM(Symbiote::Game::PhysicsSystem);

namespace Symbiote {
    namespace Game {

        auto PhysicsSystem::Update(float deltaTime) -> void
        {
            this->mManager->With<RigidBodyComponent, TransformComponent>([&](auto e, auto rigidbody, auto transform) {
                transform->mPosition *= rigidbody->mSpeed * deltaTime;
            });
        }

    }
}