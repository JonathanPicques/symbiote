#include <exception>

#include <SDL2/SDL.h>

#include "game/systems/renderer/renderer.hpp"

DEFINE_SYSTEM(Symbiote::Game::RendererSystem);

static auto inline create_sdl_window(std::string const &window_name) -> SDL_Window * {
	auto window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_VULKAN);
	if (window == nullptr) {
		throw std::runtime_error("create_sdl_window(): SDL_CreateWindow failed");
	}
	return window;
}

namespace Symbiote {
	namespace Game {

		RendererSystem::RendererSystem(std::string const &window_name) : mWindow(create_sdl_window(window_name)), mVulkanRenderer(mWindow) {
			//
		}

		RendererSystem::~RendererSystem() {
			SDL_DestroyWindow(mWindow);
			SDL_Quit();
		}

		auto RendererSystem::Render() -> void {
			mVulkanRenderer.Render();
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