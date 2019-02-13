#include "SDL2/SDL.h"

#include "game/systems/renderer.hpp"

DEFINE_SYSTEM(Symbiote::Game::RendererSystem);

namespace Symbiote {
	namespace Game {

		RendererSystem::RendererSystem() {
			mWindow = SDL_CreateWindow("Hello", 0, 0, 320, 240, 0);
			if (mWindow == nullptr) {
				std::abort(); // Failed to create SDL window
			}
		}

		auto RendererSystem::PumpEvents() -> void {
			SDL_PumpEvents();
		}

	} // namespace Game
} // namespace Symbiote