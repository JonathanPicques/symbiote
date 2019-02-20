#include <iostream>

#include "core/ecs/entity.hpp"
#include "core/ecs/entitymanager.hpp"

#include "game/systems/physics/physics.hpp"
#include "game/components/rigidbody/rigidbody.hpp"
#include "game/components/transform/transform.hpp"

DEFINE_SYSTEM(Symbiote::Game::PhysicsSystem);

namespace Symbiote {
	namespace Game {

		auto PhysicsSystem::Update(float deltaTime) -> void {
			this->mManager->With<RigidBodyComponent, TransformComponent>([&](auto e, auto rigidbody, auto transform) { transform->SetPosition(transform->GetPosition() * rigidbody->mSpeed * deltaTime); });
		}

	} // namespace Game
} // namespace Symbiote