#include <cassert>

#include "game/components/transform.hpp"

DEFINE_COMPONENT(Symbiote::Game::TransformComponent);

namespace Symbiote {
	namespace Game {

		auto TransformComponent::GetParent() -> TransformComponent * {
			return mParent;
		}

		auto TransformComponent::GetParent() const -> const TransformComponent * {
			return mParent;
		}

		auto TransformComponent::SetParent(TransformComponent *parent) -> void {
			mParent = parent;
		}

		auto TransformComponent::GetScale(TransformCoordinates coordinates) const -> Scale {
			if (coordinates == TransformCoordinates::World && mParent != nullptr) {
				return mParent->mLocalScale * mLocalScale;
			}
			return mLocalScale;
		}

		auto TransformComponent::GetPosition(TransformCoordinates coordinates) const -> Position {
			if (coordinates == TransformCoordinates::World && mParent != nullptr) {
				return mParent->mLocalPosition * mLocalPosition;
			}
			return mLocalPosition;
		}

		auto TransformComponent::GetRotation(TransformCoordinates coordinates) const -> Rotation {
			if (coordinates == TransformCoordinates::World && mParent != nullptr) {
				return mParent->mLocalRotation * mLocalRotation;
			}
			return mLocalRotation;
		}

		auto TransformComponent::SetScale(Scale const &scale, TransformCoordinates coordinates) -> void {
			mLocalScale = scale;
		}

		auto TransformComponent::SetPosition(Position const &position, TransformCoordinates coordinates) -> void {
			mLocalPosition = position;
		}

		auto TransformComponent::SetRotation(Rotation const &rotation, TransformCoordinates coordinates) -> void {
			mLocalRotation = rotation;
		}

	} // namespace Game
} // namespace Symbiote