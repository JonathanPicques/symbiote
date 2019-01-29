#include "core/system.hpp"

DEFINE_ROOT_SYSTEM(Symbiote::Core::System);

namespace Symbiote {
    namespace Core {

        System::~System()
        {

        }

        auto System::OnLoad() -> void
        {
            OnResolveDependencies();
        }

        auto System::OnResolveDependencies() -> void
        {

        }

    }
}

#undef DEFINE_ROOT_SYSTEM