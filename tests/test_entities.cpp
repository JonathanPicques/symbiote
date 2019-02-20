#include <gtest/gtest.h>

#include <core/ecs/entitymanager.hpp>

#include "test_components/components.hpp"

TEST(Entities, CreateDestroyEntities) {
	auto manager = CreateEntityManager();
	auto entity = manager->CreateEntity();
	EXPECT_TRUE(entity.IsValid());
	entity.Destroy();
	EXPECT_FALSE(entity.IsValid());
	EXPECT_ANY_THROW(entity.Destroy());

	auto entity2 = manager->CreateEntity();
	EXPECT_TRUE(entity2.IsValid());
	entity2.Destroy();
	EXPECT_FALSE(entity2.IsValid());

	auto entity3 = manager->CreateEntity();
	auto entity4 = manager->CreateEntity();
	auto entity5 = manager->CreateEntity();
	EXPECT_TRUE(entity3.IsValid());
	EXPECT_TRUE(entity4.IsValid());
	EXPECT_TRUE(entity5.IsValid());
	EXPECT_FALSE(entity2.IsValid());

	EXPECT_ANY_THROW(entity.Destroy());

	// bool operator
	EXPECT_TRUE(entity3);
	EXPECT_FALSE(entity2);

	// invalid entity
	EXPECT_FALSE(Symbiote::Core::Entity(manager.get()));
}

TEST(Components, AddComponent) {
	auto manager = CreateEntityManager();
	auto entity = manager->CreateEntity();
	auto component = entity.AddComponent<TransformComponent>();
	EXPECT_EQ(component->GetX(), 0.0f);
	EXPECT_EQ(component->GetY(), 0.0f);
	entity.Destroy();
	EXPECT_ANY_THROW(entity.GetComponent<TransformComponent>());
}

TEST(Components, AddComponentWithConstructor) {
	auto manager = CreateEntityManager();
	auto entity = manager->CreateEntity();
	auto component = entity.AddComponent<TransformComponent>(32.0f, 64.0f);
	EXPECT_EQ(component->GetX(), 32.0f);
	EXPECT_EQ(component->GetY(), 64.0f);
	entity.Destroy();
	EXPECT_ANY_THROW(entity.GetComponent<TransformComponent>());
}

TEST(Components, GetComponent) {
	auto manager = CreateEntityManager();
	auto entity = manager->CreateEntity();
	EXPECT_EQ(entity.GetComponent<TransformComponent>(), nullptr);

	auto component1 = entity.AddComponent<TransformComponent>();
	auto component2 = entity.GetComponent<TransformComponent>();

	component1->mData.x = 32.0f;
	component1->mData.y = 64.0f;

	EXPECT_EQ(component1, component2);
	EXPECT_EQ(component2->GetX(), 32.0f);
	EXPECT_EQ(component2->GetY(), 64.0f);

	EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));
	EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));
	EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));
	EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));

	EXPECT_EQ(entity.GetComponent<TransformComponent>()->GetX(), 32.0f);
	EXPECT_EQ(entity.GetComponent<TransformComponent>()->GetY(), 64.0f);

	const auto component3 = entity.GetComponent<TransformComponent>();

	EXPECT_EQ(component1, component3);
	EXPECT_EQ(component2, component3);
	EXPECT_EQ(component3->GetX(), 32.0f);
	EXPECT_EQ(component3->GetY(), 64.0f);
}

TEST(Components, RemoveComponent) {
	auto manager = CreateEntityManager();
	auto entity1 = manager->CreateEntity();
	auto entity2 = manager->CreateEntity();
	auto component = entity1.AddComponent<TransformComponent>();
	EXPECT_EQ(entity1.GetComponent<TransformComponent>(), component);
	EXPECT_EQ(entity2.GetComponent<TransformComponent>(), nullptr);

	entity1.RemoveComponent<TransformComponent>();
	EXPECT_EQ(entity1.GetComponent<TransformComponent>(), nullptr);

	EXPECT_ANY_THROW(entity1.RemoveComponent<TransformComponent>());
	EXPECT_EQ(entity1.GetComponent<TransformComponent>(), nullptr);

	entity1.AddComponent<TransformComponent>();
	ASSERT_NE(entity1.GetComponent<TransformComponent>(), nullptr);

	EXPECT_ANY_THROW(entity1.RemoveComponent<DummyComponent>());
	EXPECT_EQ(entity1.GetComponent<DummyComponent>(), nullptr);

	entity1.RemoveComponent<TransformComponent>();
	EXPECT_EQ(entity1.GetComponent<TransformComponent>(), nullptr);
}

TEST(Components, Has_Component) {
	auto manager = CreateEntityManager();
	auto entity = manager->CreateEntity();
	EXPECT_FALSE(entity.HasComponent<DummyComponent>());
	EXPECT_FALSE(entity.HasComponent<TransformComponent>());
	EXPECT_FALSE((entity.HasComponent<DummyComponent, TransformComponent>()));
	EXPECT_FALSE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));

	entity.AddComponent<DummyComponent>();
	EXPECT_TRUE(entity.HasComponent<DummyComponent>());
	EXPECT_FALSE((entity.HasComponent<DummyComponent, TransformComponent>()));
	EXPECT_TRUE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));

	entity.AddComponent<TransformComponent>();
	EXPECT_TRUE((entity.HasComponent<DummyComponent, TransformComponent>()));
	EXPECT_TRUE((entity.HasComponent<TransformComponent, DummyComponent>()));
	EXPECT_TRUE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));
	EXPECT_TRUE((entity.HasAnyComponent<TransformComponent, DummyComponent>()));
	EXPECT_TRUE((entity.HasComponent<TransformComponent, TransformComponent, TransformComponent>()));
	EXPECT_TRUE((entity.HasComponent<TransformComponent, DummyComponent, TransformComponent>()));
	EXPECT_TRUE((entity.HasAnyComponent<TransformComponent, TransformComponent, TransformComponent>()));

	entity.RemoveComponent<DummyComponent>();
	entity.RemoveComponent<TransformComponent>();
	EXPECT_FALSE((entity.HasAnyComponent<TransformComponent, DummyComponent>()));
	EXPECT_FALSE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));
}

TEST(Entity, CreateWith) {
	auto manager = CreateEntityManager();
	auto entity = manager->CreateEntityWith<DummyComponent, TransformComponent>();

	auto component1 = entity.GetComponent<DummyComponent>();
	auto component2 = entity.GetComponent<TransformComponent>();

	ASSERT_NE(component1, nullptr);
	ASSERT_NE(component2, nullptr);

	entity.Destroy();
	entity = manager->CreateEntity();
	component1 = entity.GetComponent<DummyComponent>();
	component2 = entity.GetComponent<TransformComponent>();
	EXPECT_EQ(nullptr, component1);
	EXPECT_EQ(nullptr, component2);
}

TEST(Entity, SparseRepartitionStart) {
	auto manager = CreateEntityManager();
	auto entity1 = manager->CreateEntity();
	auto entity2 = manager->CreateEntity();
	auto entity3 = manager->CreateEntity();
	entity1.Destroy();
	std::vector<Symbiote::Core::Entity> entities;
	for (auto entity : *manager) {
		entities.emplace_back(entity);
	}
	EXPECT_EQ(2, entities.size());
}

TEST(Entity, SparseRepartitionMiddle) {
	auto manager = CreateEntityManager();
	auto entity1 = manager->CreateEntity();
	auto entity2 = manager->CreateEntity();
	auto entity3 = manager->CreateEntity();
	entity2.Destroy();
	std::vector<Symbiote::Core::Entity> entities;
	for (auto entity : *manager) {
		entities.emplace_back(entity);
	}
	EXPECT_EQ(2, entities.size());
}

TEST(Entity, SparseRepartitionEnd) {
	auto manager = CreateEntityManager();
	auto entity1 = manager->CreateEntity();
	auto entity2 = manager->CreateEntity();
	auto entity3 = manager->CreateEntity();
	entity3.Destroy();
	std::vector<Symbiote::Core::Entity> entities;
	for (auto entity : *manager) {
		entities.emplace_back(entity);
	}
	EXPECT_EQ(2, entities.size());
}

TEST(Entity, SparseRepartitionExtreme) {
	auto manager = CreateEntityManager();
	auto entity1 = manager->CreateEntity();
	auto entity2 = manager->CreateEntity();
	auto entity3 = manager->CreateEntity();
	auto entity4 = manager->CreateEntity();
	auto entity5 = manager->CreateEntity();
	auto entity6 = manager->CreateEntity();
	auto entity7 = manager->CreateEntity();
	entity1.Destroy();
	entity3.Destroy();
	entity6.Destroy();
	std::vector<Symbiote::Core::Entity> entities;
	for (auto entity : *manager) {
		entities.emplace_back(entity);
	}
	EXPECT_EQ(4, entities.size());
}