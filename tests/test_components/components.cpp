#include <core/ecs/entitymanager.hpp>

#include "components.hpp"

DEFINE_COMPONENT(DummyComponent);
DEFINE_COMPONENT(PhysicsComponent);
DEFINE_COMPONENT(TransformComponent);

auto CreateEntityManager() -> std::unique_ptr<Symbiote::Core::EntityManager> {
	auto manager = std::make_unique<Symbiote::Core::EntityManager>();
	manager->RegisterComponent<DummyComponent>();
	manager->RegisterComponent<PhysicsComponent>();
	manager->RegisterComponent<TransformComponent>();
	return std::move(manager);
}