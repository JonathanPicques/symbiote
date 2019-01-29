#include "core/component.hpp"

DEFINE_ROOT_COMPONENT(Symbiote::Core::Component);

namespace Symbiote {
    namespace Core {

        Component::~Component()
        {

        }

        auto Component::OnLoad() -> void
        {
            OnResolveDependencies();
        }

        auto Component::OnResolveDependencies() -> void
        {

        }

        auto Component::Serialize(std::ostream&) const -> void
        {

        }

        auto Component::Deserialize(std::istream&) -> void
        {

        }

    }
}

#undef DEFINE_ROOT_COMPONENT