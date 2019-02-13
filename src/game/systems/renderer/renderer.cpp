#include "SDL2/SDL.h"

#include "game/systems/renderer/renderer.hpp"

DEFINE_SYSTEM(Symbiote::Game::RendererSystem);

namespace Symbiote {
	namespace Game {

		RendererSystem::RendererSystem() {
			auto init = SDL_Init(SDL_INIT_VIDEO);
			if (init != 0) {
				std::abort(); // Failed to init SDL
			}
			mWindow = SDL_CreateWindow("Hello", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_VULKAN);
			if (mWindow == nullptr) {
				std::abort(); // Failed to create SDL window
			}
		}

		RendererSystem::~RendererSystem() {
			SDL_DestroyWindow(mWindow);
			SDL_Quit();
		}

		auto RendererSystem::PollEvents() -> bool {
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					return false;
				}
			}
			return true;
		}

	} // namespace Game
} // namespace Symbiote