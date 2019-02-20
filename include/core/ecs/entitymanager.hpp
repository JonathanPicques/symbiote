#pragma once

#include <memory>
#include <vector>
#include <string>
#include <iosfwd>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_map>

#include "system.hpp"
#include "entity.hpp"
#include "component.hpp"

namespace Symbiote {
	namespace Core {

		class EntityManager final {
		public:
			friend Entity;

		public:
			auto CreateEntity() -> Entity;
			template<typename... C>
			auto CreateEntityWith() -> Entity;

		private:
			auto DestroyEntity(Entity &entityPointer) -> void;

		public:
			template<typename S, typename... Args>
			auto AddSystem(Args &&... args) -> S *;
			template<typename S>
			auto GetSystem() -> S *;
			template<typename S>
			auto GetSystem() const -> const S *;
			template<typename S>
			auto RemoveSystem() -> void;
			template<typename S>
			auto HasSystem() -> bool;

		public:
			template<typename C>
			auto RegisterComponent() -> void;
#if defined(_DEBUG)
			auto IsComponentRegistered(const std::string &componentName) const -> bool;
			auto AssertComponentRegistered(const std::string &componentName) const -> void;
#endif

		public:
			template<typename... C>
			auto Any(typename std::common_type<std::function<void(Entity, C *...)>>::type view) -> void;
			template<typename... C>
			auto Any() -> std::vector<Entity>;
			template<typename... C>
			auto With(typename std::common_type<std::function<void(Entity, C *...)>>::type view) -> void;
			template<typename... C>
			auto With() -> std::vector<Entity>;

		public:
			auto Serialize(std::ostream &os) const -> void;
			auto Deserialize(std::istream &is) -> void;

		private:
			auto IsEntityPointerValid(const Entity &entityPointer) const -> bool;
			auto AssertEntityPointerValid(const Entity &entityPointer) const -> void;

		public:
			template<bool is_const>
			class EntityComponentContainerIterator final {
			public:
				friend EntityManager;

			public:
				using EntityType = std::conditional_t<is_const, const Entity, Entity>;
				using ManagerType = std::conditional_t<is_const, const EntityManager, EntityManager>;

			public:
				EntityComponentContainerIterator(ManagerType &manager, Entity::PointerSize position) : mManager(manager), mIndex(position), mVersion(0) {
					IterateToNextValidEntity();
				}

			public:
				auto operator*() -> EntityType {
					return EntityType(const_cast<EntityManager *>(&mManager), mIndex, mVersion);
				}
				auto operator!=(const EntityComponentContainerIterator &other) -> bool {
					return mIndex != other.mIndex;
				}
				auto operator++() -> const EntityComponentContainerIterator & {
					{
						mIndex += 1;
						IterateToNextValidEntity();
						return *this;
					}
				}

			private:
				auto IterateToNextValidEntity() -> void {
					while (mIndex < mManager.mNextIndex && std::find(mManager.mFreeIndexes.begin(), mManager.mFreeIndexes.end(), mIndex) != mManager.mFreeIndexes.end()) {
						mIndex += 1;
					}
					if (mIndex < mManager.mNextIndex) {
						mVersion = mManager.mVersions[mIndex];
					}
				}

			private:
				ManagerType &mManager;
				Entity::PointerSize mIndex;
				Entity::PointerSize mVersion;
			};

		public:
			using Iterator = EntityComponentContainerIterator<false>;
			using ConstIterator = EntityComponentContainerIterator<true>;

		public:
			auto begin() -> Iterator;
			auto end() -> Iterator;
			auto begin() const -> ConstIterator;
			auto end() const -> ConstIterator;

		public:
			auto Clear() -> void;
			auto Size() const -> std::size_t;

		private:
			template<typename C>
			auto EntityGetComponent(const Entity &entityPointer) -> C *;
			template<typename C>
			auto EntityGetComponent(const Entity &entityPointer) const -> const C *;
			template<typename C, typename... Args>
			auto EntityAddComponent(const Entity &entityPointer, Args &&... args) -> C *;
			template<typename C>
			auto EntityRemoveComponent(const Entity &entityPointer) -> void;

		private:
			auto EntityConstructComponent(Component *component, const Entity &entityPointer) -> void;
			auto EntityResolveComponentDependencies(const Entity &entityPointer) -> void;

		private:
			template<typename C>
			auto EntityHasComponent(const Entity &entityPointer) const -> bool;
			template<typename C1, typename C2, typename... C>
			auto EntityHasComponent(const Entity &entityPointer) const -> bool;
			template<typename C>
			auto EntityHasAnyComponent(const Entity &entityPointer) const -> bool;
			template<typename C1, typename C2, typename... C>
			auto EntityHasAnyComponent(const Entity &entityPointer) const -> bool;

		private:
			template<typename... C>
			auto EntityAny(const Entity &entityPointer, typename std::common_type<std::function<void(C *...)>>::type view) -> bool;
			template<typename... C>
			auto EntityWith(const Entity &entityPointer, typename std::common_type<std::function<void(C *...)>>::type view) -> bool;

		private:
			Entity::PointerSize mNextIndex = {};
			std::vector<Entity::PointerSize> mVersions = {};
			std::vector<Entity::PointerSize> mFreeIndexes = {};
			std::vector<std::vector<std::unique_ptr<Component>>> mEntityComponents = {};

		private:
			std::vector<std::unique_ptr<System>> mSystems = {};
			std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> mRegisteredComponents = {};
		};

		template<typename C>
		auto Entity::GetComponent() -> C * {
			return mManager->EntityGetComponent<C>(*this);
		}

		template<typename C>
		auto Entity::GetComponent() const -> const C * {
			return mManager->EntityGetComponent<C>(*this);
		}

		template<typename C, typename... Args>
		auto Entity::AddComponent(Args &&... args) -> C * {
			return mManager->EntityAddComponent<C>(*this, std::forward<Args>(args)...);
		}

		template<typename C>
		auto Entity::RemoveComponent() -> void {
			mManager->EntityRemoveComponent<C>(*this);
		}

		template<typename C>
		auto Entity::HasComponent() const -> bool {
			return mManager->EntityHasComponent<C>(*this);
		}

		template<typename C1, typename C2, typename... C>
		auto Entity::HasComponent() const -> bool {
			return mManager->EntityHasComponent<C1, C2, C...>(*this);
		}

		template<typename C>
		auto Entity::HasAnyComponent() const -> bool {
			return mManager->EntityHasAnyComponent<C>(*this);
		}

		template<typename C1, typename C2, typename... C>
		auto Entity::HasAnyComponent() const -> bool {
			return mManager->EntityHasAnyComponent<C1, C2, C...>(*this);
		}

		template<typename... C>
		auto Entity::Any(typename std::common_type<std::function<void(C *...)>>::type view) -> bool {
			return mManager->template EntityAny<C...>(*this, view);
		}

		template<typename... C>
		auto Entity::With(typename std::common_type<std::function<void(C *...)>>::type view) -> bool {
			return mManager->template EntityWith<C...>(*this, view);
		}

		template<typename... C>
		auto EntityManager::CreateEntityWith() -> Entity {
			auto entityPointer = CreateEntity();
			(entityPointer.AddComponent<C>(), ...);
			EntityResolveComponentDependencies(entityPointer);
			return entityPointer;
		}

		template<typename S, typename... Args>
		auto EntityManager::AddSystem(Args &&... args) -> S * {
			if (HasSystem<S>()) {
				throw std::logic_error(std::string{"EntityManager::AddSystem: System "} + S::SystemName + std::string{" already exists"});
			}
			auto system = std::make_unique<S>(std::forward<Args>(args)...);
			auto systemPtr = system.get();
			mSystems.emplace_back(std::move(system));
			systemPtr->mManager = this;
			return systemPtr;
		}

		template<typename S>
		auto EntityManager::GetSystem() -> S * {
			return const_cast<S *>(static_cast<const EntityManager *>(this)->GetSystem<S>());
		}

		template<typename S>
		auto EntityManager::GetSystem() const -> const S * {
			auto found = std::find_if(mSystems.begin(), mSystems.end(), [](const auto &s) { return s->GetSystemName() == S::SystemName; });
			if (found != mSystems.end()) {
				return static_cast<S *>(found->get());
			}
			return nullptr;
		}

		template<typename S>
		auto EntityManager::RemoveSystem() -> void {
			if (!HasSystem<S>()) {
				throw std::logic_error(std::string{"EntityManager::RemoveSystem: System "} + S::SystemName + std::string{" not found"});
			}
			auto found = std::find_if(mSystems.begin(), mSystems.end(), [](const auto &s) { return s->GetSystemName() == S::SystemName; });
			if (found != mSystems.end()) {
				mSystems.erase(found);
			}
		}

		template<typename S>
		auto EntityManager::HasSystem() -> bool {
			return GetSystem<S>() != nullptr;
		}

		template<typename C>
		auto EntityManager::RegisterComponent() -> void {
			mRegisteredComponents[C::ComponentName] = []() { return std::make_unique<C>(); };
		}

		template<typename C>
		auto EntityManager::EntityGetComponent(const Entity &entityPointer) -> C * {
			AssertEntityPointerValid(entityPointer);
			return const_cast<C *>(static_cast<const EntityManager *>(this)->EntityGetComponent<C>(entityPointer));
		}

		template<typename C>
		auto EntityManager::EntityGetComponent(const Entity &entityPointer) const -> const C * {
			AssertEntityPointerValid(entityPointer);
			auto &components = mEntityComponents[entityPointer.mIndex];
			auto found = std::find_if(components.begin(), components.end(), [](const auto &c) { return c->GetComponentName() == C::ComponentName; });
			if (found != components.end()) {
				return static_cast<C *>(found->get());
			}
			return nullptr;
		}

		template<typename C, typename... Args>
		auto EntityManager::EntityAddComponent(const Entity &entityPointer, Args &&... args) -> C * {
			AssertEntityPointerValid(entityPointer);
#if defined(_DEBUG)
			AssertComponentRegistered(C::ComponentName);
#endif
			if (EntityHasComponent<C>(entityPointer)) {
				throw std::logic_error(std::string{"Entity::AddComponent: Component "} + C::ComponentName + std::string{" already exists"});
			}
			auto component = std::make_unique<C>(std::forward<Args>(args)...);
			auto componentPtr = component.get();
			mEntityComponents[entityPointer.mIndex].emplace_back(std::move(component));
			EntityConstructComponent(componentPtr, entityPointer);
			return componentPtr;
		}

		template<typename C>
		auto EntityManager::EntityRemoveComponent(const Entity &entityPointer) -> void {
			AssertEntityPointerValid(entityPointer);
#if defined(_DEBUG)
			AssertComponentRegistered(C::ComponentName);
#endif
			auto &components = mEntityComponents[entityPointer.mIndex];
			if (!EntityHasComponent<C>(entityPointer)) {
				throw std::logic_error(std::string{"Entity::RemoveComponent: Component "} + C::ComponentName + std::string{" not found"});
			}
			auto found = std::find_if(components.begin(), components.end(), [](const auto &c) { return C::ComponentName == c->GetComponentName(); });
			if (found != components.end()) {
				components.erase(found);
			}
		}

		template<typename C>
		auto EntityManager::EntityHasComponent(const Entity &entityPointer) const -> bool {
			AssertEntityPointerValid(entityPointer);
			return EntityGetComponent<C>(entityPointer) != nullptr;
		}

		template<typename C1, typename C2, typename... C>
		auto EntityManager::EntityHasComponent(const Entity &entityPointer) const -> bool {
			AssertEntityPointerValid(entityPointer);
			return EntityHasComponent<C1>(entityPointer) && EntityHasComponent<C2, C...>(entityPointer);
		}

		template<typename C>
		auto EntityManager::EntityHasAnyComponent(const Entity &entityPointer) const -> bool {
			AssertEntityPointerValid(entityPointer);
			return EntityGetComponent<C>(entityPointer) != nullptr;
		}

		template<typename C1, typename C2, typename... C>
		auto EntityManager::EntityHasAnyComponent(const Entity &entityPointer) const -> bool {
			AssertEntityPointerValid(entityPointer);
			if (EntityHasComponent<C1>(entityPointer)) {
				return true;
			}
			return EntityHasComponent<C2, C...>(entityPointer);
		}

		template<typename... C>
		auto EntityManager::EntityAny(const Entity &entityPointer, typename std::common_type<std::function<void(C *...)>>::type view) -> bool {
			AssertEntityPointerValid(entityPointer);
			if (EntityHasAnyComponent<C...>(entityPointer)) {
				view(EntityGetComponent<C>(entityPointer)...);
				return true;
			}
			return false;
		}

		template<typename... C>
		auto EntityManager::EntityWith(const Entity &entityPointer, typename std::common_type<std::function<void(C *...)>>::type view) -> bool {
			AssertEntityPointerValid(entityPointer);
			if (EntityHasComponent<C...>(entityPointer)) {
				view(EntityGetComponent<C>(entityPointer)...);
				return true;
			}
			return false;
		}

		template<typename... C>
		auto EntityManager::Any(typename std::common_type<std::function<void(Entity, C *...)>>::type view) -> void {
			for (auto entityPointer : *this) {
				if (entityPointer.HasAnyComponent<C...>()) {
					view(entityPointer, entityPointer.GetComponent<C>()...);
				}
			}
		}

		template<typename... C>
		auto EntityManager::Any() -> std::vector<Entity> {
			std::vector<Entity> entityPointers;
			for (auto entityPointer : *this) {
				if (entityPointer.HasAnyComponent<C...>()) {
					entityPointers.emplace_back(entityPointer);
				}
			}
			return entityPointers;
		}

		template<typename... C>
		auto EntityManager::With(typename std::common_type<std::function<void(Entity, C *...)>>::type view) -> void {
			for (auto entityPointer : *this) {
				if (entityPointer.HasComponent<C...>()) {
					view(entityPointer, entityPointer.GetComponent<C>()...);
				}
			}
		}

		template<typename... C>
		auto EntityManager::With() -> std::vector<Entity> {
			std::vector<Entity> entityPointers;
			for (auto entityPointer : *this) {
				if (entityPointer.HasComponent<C...>()) {
					entityPointers.emplace_back(entityPointer);
				}
			}
			return entityPointers;
		}

	} // namespace Core
} // namespace Symbiote