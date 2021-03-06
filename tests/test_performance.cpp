#if defined(SYMBIOTE_BENCHMARK)

#	include <ctime>
#	include <gtest/gtest.h>

#	include "core/ecs/entitymanager.hpp"

#	include "test_components/components.hpp"

TEST(Performance, CreateMaxEntities) {
	auto manager = CreateEntityManager();
	const clock_t t0 = clock();

	for (auto i = 0; i < std::numeric_limits<Symbiote::Core::Entity::PointerSize>::max(); i++) {
		manager->CreateEntity();
	}

	const clock_t t1 = clock();
	std::cout << "CreateMaxEntities took " << (t1 - t0) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
}

TEST(Performance, CreateMaxEntitiesWithComponents) {
	auto manager = CreateEntityManager();
	const clock_t t0 = clock();

	for (auto i = 0; i < std::numeric_limits<Symbiote::Core::Entity::PointerSize>::max(); i++) {
		manager->CreateEntityWith<DummyComponent, TransformComponent, PhysicsComponent>();
	}

	const clock_t t1 = clock();
	std::cout << "CreateMaxEntitiesWithComponents took " << (t1 - t0) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
}

TEST(Performance, CreateMaxEntitiesWithComponentsAndUpdateThem) {
	auto manager = CreateEntityManager();
	const clock_t t0 = clock();

	for (auto i = 0; i < std::numeric_limits<Symbiote::Core::Entity::PointerSize>::max(); i++) {
		if (i % 1000 == 0) {
			manager->CreateEntityWith<TransformComponent>();
		} else {
			manager->CreateEntityWith<DummyComponent>();
		}
	}
	for (auto i = 0; i < 100; i++) {
		manager->With<TransformComponent>([&i](auto, TransformComponent *ptr) {
			ptr->GetX();
			ptr->GetY();
		});
	}
	const clock_t t1 = clock();
	std::cout << "CreateMaxEntitiesWithComponentsAndUpdateThem took " << (t1 - t0) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
}

#endif