#pragma once

#include <memory>
#include <ostream>
#include <istream>

#include <core/component.hpp>

namespace Symbiote {
	namespace Core {
		class EntityManager;
	}
} // namespace Symbiote

class DummyComponent final : public Symbiote::Core::Component {
public:
	DECLARE_COMPONENT(DummyComponent);
};

class PhysicsComponent final : public Symbiote::Core::Component {
public:
	DECLARE_COMPONENT(PhysicsComponent);
};

class TransformComponent final : public Symbiote::Core::Component {
public:
	DECLARE_COMPONENT(TransformComponent);

public:
	TransformComponent() = default;
	TransformComponent(float x, float y) : mData{x, y} {
	}

public:
	auto Serialize(std::ostream &out) const -> void override {
		static_assert(std::is_pod<decltype(TransformComponent::mData)>::value, "TransformComponent is not POD");
		out.write((char *)&mData, sizeof(mData));
	}
	auto Deserialize(std::istream &is) -> void override {
		static_assert(std::is_pod<decltype(TransformComponent::mData)>::value, "TransformComponent is not POD");
		is.read((char *)&mData, sizeof(mData));
	}

public:
	auto GetX() const -> float {
		return mData.x;
	}
	auto GetY() const -> float {
		return mData.y;
	}

public:
	struct {
		float x;
		float y;
	} mData = {};
};

auto CreateEntityManager() -> std::unique_ptr<Symbiote::Core::EntityManager>;