#include "core/entity.hpp"
#include "core/entitymanager.hpp"

namespace Symbiote {
	namespace Core {

		Entity::Entity(EntityManager *manager) : mManager(manager) {
		}

		Entity::Entity(EntityManager *manager, PointerSize index, PointerSize version) : mManager(manager), mIndex(index), mVersion(version) {
		}

		Entity::operator bool() const {
			return IsValid();
		}

		auto Entity::IsValid() const -> bool {
			return mManager != nullptr && mManager->IsEntityPointerValid(*this);
		}

		auto Entity::Destroy() -> void {
			mManager->DestroyEntity(*this);
		}

		auto Entity::ResolveComponentDependencies() -> void {
			mManager->EntityResolveComponentDependencies(*this);
		}

		auto Entity::GetManager() -> EntityManager * {
			return mManager;
		}

		auto operator==(const Entity &a, const Entity &b) -> bool {
			return a.mIndex == b.mIndex && a.mVersion == b.mVersion;
		}

	} // namespace Core
} // namespace Symbiote