#include "core/ecs/system.hpp"

DEFINE_ROOT_SYSTEM(Symbiote::Core::System);

namespace Symbiote {
	namespace Core {

		System::~System() {
		}

		auto System::OnLoad() -> void {
			OnResolveDependencies();
		}

		auto System::OnResolveDependencies() -> void {
		}

	} // namespace Core
} // namespace Symbiote

#undef DEFINE_ROOT_SYSTEM