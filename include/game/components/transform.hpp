#pragma once

#include "glm/vec2.hpp"

#include "core/component.hpp"

namespace Symbiote {
	namespace Game {

		enum class TransformCoordinates { Local, World };

		class TransformComponent : public Symbiote::Core::Component {
		public:
			DECLARE_COMPONENT(Symbiote::Game::TransformComponent);

		public:
			using Scale = glm::vec2;
			using Position = glm::vec2;
			using Rotation = float;

		public:
			auto GetParent() -> TransformComponent *;
			auto GetParent() const -> const TransformComponent *;

			auto SetParent(TransformComponent *parent) -> void;

		public:
			auto GetScale(TransformCoordinates coordinates = TransformCoordinates::Local) const -> Scale;
			auto GetPosition(TransformCoordinates coordinates = TransformCoordinates::Local) const -> Position;
			auto GetRotation(TransformCoordinates coordinates = TransformCoordinates::Local) const -> Rotation;

			auto SetScale(Scale const &scale, TransformCoordinates coordinates = TransformCoordinates::Local) -> void;
			auto SetPosition(Position const &position, TransformCoordinates coordinates = TransformCoordinates::Local) -> void;
			auto SetRotation(Rotation const &rotation, TransformCoordinates coordinates = TransformCoordinates::Local) -> void;

		private:
			TransformComponent *mParent = nullptr;

		private:
			Scale mLocalScale = {1, 1};
			Position mLocalPosition = {0, 0};
			Rotation mLocalRotation = 0.0f;
		};

	} // namespace Game
} // namespace Symbiote