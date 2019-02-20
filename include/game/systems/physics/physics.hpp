#pragma once

#include "core/ecs/system.hpp"

namespace Symbiote {
	namespace Game {

		class PhysicsSystem final : public Symbiote::Core::System {
		public:
			DECLARE_SYSTEM(Symbiote::Game::PhysicsSystem);

		public:
			PhysicsSystem() = default;
			PhysicsSystem(PhysicsSystem &&) = delete;
			PhysicsSystem(PhysicsSystem const &) = delete;
			PhysicsSystem &operator=(PhysicsSystem const &) = delete;

		public:
			auto Update(float deltaTime) -> void;
		};

	} // namespace Game
} // namespace Symbiote