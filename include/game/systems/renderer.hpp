#pragma once

#include "core/system.hpp"

struct SDL_Window;

namespace Symbiote {
	namespace Game {

		class RendererSystem final : public Symbiote::Core::System {
		public:
			DECLARE_SYSTEM(Symbiote::Game::RendererSystem);

		public:
			RendererSystem();

		public:
			auto PumpEvents() -> void;

		private:
			SDL_Window *mWindow = nullptr;
		};

	} // namespace Game
} // namespace Symbiote