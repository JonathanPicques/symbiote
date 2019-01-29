#pragma once

#include <memory>
#include <string>
#include <iosfwd>

#include "entity.hpp"

#define DECLARE_COMPONENT(NAME) static constexpr const char* ComponentName{#NAME}; virtual std::string GetComponentName() const override
#define DEFINE_COMPONENT(NAME) std::string NAME::GetComponentName() const { return NAME::ComponentName; } constexpr const char* NAME::ComponentName

#define DECLARE_ROOT_COMPONENT(NAME) static constexpr const char* ComponentName{#NAME}; virtual std::string GetComponentName() const
#define DEFINE_ROOT_COMPONENT(NAME) std::string NAME::GetComponentName() const { return NAME::ComponentName; } constexpr const char* NAME::ComponentName

class EntityManager;

class Component
{
public:
    DECLARE_ROOT_COMPONENT(Component);

public:
    friend Entity;
    friend EntityManager;

public:
    virtual ~Component() = 0;

protected:
    virtual auto OnLoad() -> void;
    virtual auto OnResolveDependencies() -> void;

protected:
    virtual auto Serialize(std::ostream& os) const -> void;
    virtual auto Deserialize(std::istream& is) -> void;

protected:
   Entity mEntity = {};
};

#undef DECLARE_ROOT_COMPONENT