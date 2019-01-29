#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

namespace Symbiote {
    namespace Core {

        class EntityManager;

        class Entity final
        {
        public:
            friend EntityManager;

        public:
            using PointerSize = std::uint16_t;

        public:
            Entity() = default;
            Entity(Entity&&) = default;
            Entity(Entity const&) = default;
            Entity& operator=(Entity const&) = default;

        public:
            Entity(EntityManager* manager);
            Entity(EntityManager* manager, PointerSize index, PointerSize version);

        public:
            explicit operator bool() const;

        public:
            auto IsValid() const -> bool;

        public:
            template<typename C>
            auto GetComponent() -> C*;
            template<typename C>
            auto GetComponent() const -> const C*;
            template<typename C>
            auto AddComponent() -> C*;
            template<typename C>
            auto RemoveComponent() -> void;
            template<typename C>
            auto HasComponent() const -> bool;
            template<typename C1, typename C2, typename ...C>
            auto HasComponent() const -> bool;
            template<typename C>
            auto HasAnyComponent() const -> bool;
            template<typename C1, typename C2, typename ...C>
            auto HasAnyComponent() const -> bool;

        public:
            template<typename ...C>
            auto Any(typename std::common_type<std::function<void(C* ...)>>::type view) -> bool;
            template<typename ...C>
            auto With(typename std::common_type<std::function<void(C* ...)>>::type view) -> bool;

        public:
            auto ResolveComponentDependencies() -> void;

        public:
            auto Destroy() -> void;

        public:
            auto GetManager() -> EntityManager*;

        public:
            friend auto operator==(const Entity& a, const Entity& b) -> bool;

        private:
            EntityManager* mManager = nullptr;
            PointerSize mIndex = 0;
            PointerSize mVersion = 0;
        };

    }
}