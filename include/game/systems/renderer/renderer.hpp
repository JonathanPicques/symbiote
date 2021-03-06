#pragma once

#include <string>

#include <glm/fwd.hpp>

#include "core/ecs/system.hpp"
#include "game/systems/renderer/vulkan/vulkan.hpp"

struct SDL_Window;

namespace Symbiote {
	namespace Game {

		class RendererSystem final : public Symbiote::Core::System {
		public:
			DECLARE_SYSTEM(Symbiote::Game::RendererSystem);

		public:
			RendererSystem(std::string const &window_name, glm::vec4 const &clear_color);
			RendererSystem(RendererSystem &&) = delete;
			RendererSystem(RendererSystem const &) = delete;
			RendererSystem &operator=(RendererSystem const &) = delete;

		public:
			~RendererSystem();

		public:
			auto Render() -> void;
			auto PollEvents() -> bool;

		private:
			SDL_Window *mWindow = nullptr;
			VulkanRenderer mVulkanRenderer;
		};

	} // namespace Game
} // namespace Symbiote