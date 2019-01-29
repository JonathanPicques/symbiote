#include "core/system.hpp"

DEFINE_ROOT_SYSTEM(System);

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

#undef DEFINE_ROOT_SYSTEM