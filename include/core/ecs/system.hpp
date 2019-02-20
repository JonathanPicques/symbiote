#pragma once

#include <string>

// clang-format off
#define DECLARE_SYSTEM(NAME) static constexpr const char* SystemName{#NAME}; virtual std::string GetSystemName() const override
#define DEFINE_SYSTEM(NAME) std::string NAME::GetSystemName() const { return NAME::SystemName; } constexpr const char* NAME::SystemName

#define DECLARE_ROOT_SYSTEM(NAME) static constexpr const char* SystemName{#NAME}; virtual std::string GetSystemName() const
#define DEFINE_ROOT_SYSTEM(NAME) std::string NAME::GetSystemName() const { return NAME::SystemName; } constexpr const char* NAME::SystemName
// clang-format on

namespace Symbiote {
	namespace Core {

		class EntityManager;

		class System {
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
			EntityManager *mManager = nullptr;
		};

	} // namespace Core
} // namespace Symbiote

#undef DECLARE_ROOT_SYSTEM