#pragma once

#include <string>

#define DECLARE_SYSTEM(NAME) static constexpr const char* SystemName{#NAME}; virtual std::string GetSystemName() const override
#define DEFINE_SYSTEM(NAME) std::string NAME::GetSystemName() const { return NAME::SystemName; } constexpr const char* NAME::SystemName

#define DECLARE_ROOT_SYSTEM(NAME) static constexpr const char* SystemName{#NAME}; virtual std::string GetSystemName() const
#define DEFINE_ROOT_SYSTEM(NAME) std::string NAME::GetSystemName() const { return NAME::SystemName; } constexpr const char* NAME::SystemName

namespace Symbiote {
    namespace Core {

        class EntityManager;

        class System
        {
        public:
            DECLARE_ROOT_SYSTEM(Symbiote::Core::System);

        public:
            friend EntityManager;

        public:
            virtual ~System() = 0;

        protected:
            virtual auto OnLoad() -> void;
            virtual auto OnResolveDependencies() -> void;

        protected:
            EntityManager* mManager = nullptr;
        };

    }
}

#undef DECLARE_ROOT_SYSTEM