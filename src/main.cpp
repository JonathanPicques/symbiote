#include <iostream>

#include "core/entitymanager.hpp"

#include "game/systems/physics.hpp"
#include "game/systems/renderer.hpp"
#include "game/components/transform.hpp"

using namespace Symbiote::Core;
using namespace Symbiote::Game;

auto main() -> int {
	EntityManager manager;

	manager.RegisterComponent<TransformComponent>();

	auto physics = manager.AddSystem<PhysicsSystem>();
	auto renderer = manager.AddSystem<RendererSystem>();

	auto entity = manager.CreateEntityWith<TransformComponent>();
	auto transform = entity.GetComponent<TransformComponent>();

	physics->Update(0.16f);
	physics->Update(0.16f);
	physics->Update(0.16f);
	physics->Update(0.16f);

	std::cout << entity.GetComponent<TransformComponent>()->GetRotation() << std::endl;

	while (true) {
		renderer->PumpEvents();
	}

	return 0;
}