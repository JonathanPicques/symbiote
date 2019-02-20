#pragma once

#include "core/ecs/component.hpp"

namespace Symbiote {
	namespace Game {

		class RigidBodyComponent final : public Symbiote::Core::Component {
		public:
			DECLARE_COMPONENT(Symbiote::Game::RigidBodyComponent);

		public:
			RigidBodyComponent() = default;
			RigidBodyComponent(RigidBodyComponent &&) = delete;
			RigidBodyComponent(RigidBodyComponent const &) = delete;
			RigidBodyComponent &operator=(RigidBodyComponent const &) = delete;

		public:
			float mSpeed;
		};

	} // namespace Game
} // namespace Symbiote