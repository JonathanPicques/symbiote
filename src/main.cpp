#include <iostream>

#include <glm/vec4.hpp>

#include "core/ecs/entitymanager.hpp"

#include "game/systems/physics/physics.hpp"
#include "game/systems/renderer/renderer.hpp"
#include "game/components/transform/transform.hpp"

using namespace Symbiote::Core;
using namespace Symbiote::Game;

auto main() -> int {
	EntityManager manager;

	manager.RegisterComponent<TransformComponent>();

	auto physics = manager.AddSystem<PhysicsSystem>();
	auto renderer = manager.AddSystem<RendererSystem>("Symbiote Engine - Vulkan", glm::vec4(0, 0, 1, 0));

	auto entity = manager.CreateEntityWith<TransformComponent>();
	auto transform = entity.GetComponent<TransformComponent>();

	while (renderer->PollEvents()) {
		physics->Update(0.16f);
		renderer->Render();
	}

	return 0;
}