#include <cstring>
#include <ostream>
#include <istream>
#include <sstream>
#include <algorithm>

#include "core/entitymanager.hpp"

namespace Symbiote {
    namespace Core {

        auto EntityManager::CreateEntity() -> Entity
        {
            Entity::PointerSize index;
            Entity::PointerSize version;
            if (mFreeIndexes.empty())
            {
                index = mNextIndex++;
                mVersions.resize(index + 1);
                mEntityComponents.resize(index + 1);
                version = mVersions[index] = 1;
            }
            else
            {
                index = mFreeIndexes.back();
                version = mVersions[index];
                mFreeIndexes.pop_back();
            }
            return { this, index, version };
        }

        auto EntityManager::DestroyEntity(Entity& entityPointer) -> void
        {
            AssertEntityPointerValid(entityPointer);
            mVersions[entityPointer.mIndex] += 1;
            mEntityComponents[entityPointer.mIndex].clear();
            mFreeIndexes.push_back(entityPointer.mIndex);
        }

        auto EntityManager::Serialize(std::ostream& os) const -> void
        {
            for (auto entityPointer : *this)
            {
                os << '{';
                os.write(reinterpret_cast<char*>(&entityPointer.mIndex), sizeof(entityPointer.mIndex));
                os.write(reinterpret_cast<char*>(&entityPointer.mVersion), sizeof(entityPointer.mVersion));
                for (const auto& component : mEntityComponents[entityPointer.mIndex])
                {
                    os.write(component->GetComponentName().c_str(), 1 + component->GetComponentName().size());
                    component->Serialize(os);
                }
                os << '}';
            }
        }

        auto EntityManager::Deserialize(std::istream& is) -> void
        {
            Clear();
            enum class ParsingState
            {
                eEntity,
                eComponentName,
            };
            auto state = ParsingState::eEntity;
            std::unique_ptr<Entity> entityPointer;
            std::string componentName;
            while (!is.eof())
            {
                char token;
                is >> token;
                if (state == ParsingState::eEntity)
                {
                    if (token == '0')
                    {
                        break;
                    }
                    else if (token == '{')
                    {
                        entityPointer = std::make_unique<Entity>(this);
                        is.read(reinterpret_cast<char*>(&entityPointer->mIndex), sizeof(entityPointer->mIndex));
                        is.read(reinterpret_cast<char*>(&entityPointer->mVersion), sizeof(entityPointer->mVersion));
                        for (auto i = mNextIndex; i < entityPointer->mIndex; i++)
                        {
                            mFreeIndexes.emplace_back(i);
                        }
                        mNextIndex = static_cast<Entity::PointerSize>(entityPointer->mIndex + 1);
                        mVersions.resize(mNextIndex);
                        mEntityComponents.resize(mNextIndex);
                        mVersions[entityPointer->mIndex] = entityPointer->mVersion;
                        state = ParsingState::eComponentName;
                    }
                }
                else if (state == ParsingState::eComponentName)
                {
                    if (token == '}')
                    {
                        entityPointer->ResolveComponentDependencies();
                        state = ParsingState::eEntity;
                    }
                    else if (token == '\0')
                    {
                        auto componentCreator = mRegisteredComponents.find(componentName);
                        if (componentCreator == mRegisteredComponents.end())
                        {
                            throw std::logic_error(componentName + std::string { " is not registered" });
                        }
                        auto component = componentCreator->second();
                        auto componentPtr = component.get();
                        mEntityComponents[entityPointer->mIndex].emplace_back(std::move(component));
                        componentPtr->Deserialize(is);
                        EntityConstructComponent(componentPtr, *(entityPointer.get()));
                        componentName.clear();
                        state = ParsingState::eComponentName;
                    }
                    else
                    {
                        componentName += token;
                    }
                }
            }
        }

        auto EntityManager::IsEntityPointerValid(const Entity& entityPointer) const -> bool
        {
            return entityPointer.mIndex < mVersions.size() && mVersions[entityPointer.mIndex] == entityPointer.mVersion;
        }

        auto EntityManager::AssertEntityPointerValid(const Entity& entityPointer) const -> void
        {
            if (!IsEntityPointerValid(entityPointer))
            {
                std::stringstream errorFormat;
                errorFormat << "Entity invalid: " << entityPointer.mIndex << "(" << entityPointer.mIndex << ")";
                throw std::logic_error(errorFormat.str());
            }
        }

        auto EntityManager::EntityConstructComponent(Component* component, const Entity& entityPointer) -> void
        {
            AssertEntityPointerValid(entityPointer);
            component->mEntity = entityPointer;
            component->OnLoad();
        }

        auto EntityManager::EntityResolveComponentDependencies(const Entity& entityPointer) -> void
        {
            AssertEntityPointerValid(entityPointer);
            auto& components = mEntityComponents[entityPointer.mIndex];
            for (auto& component : components)
            {
                component->OnResolveDependencies();
            }
        }

        #if defined(_DEBUG)
        auto EntityManager::IsComponentRegistered(const std::string& componentName) const -> bool
        {
            return mRegisteredComponents.find(componentName) != mRegisteredComponents.end();
        }

        auto EntityManager::AssertComponentRegistered(const std::string& componentName) const -> void
        {
            if (!IsComponentRegistered(componentName))
            {
                throw std::logic_error(std::string{ "EntityManager::AssertComponentRegistered: Component " } + componentName + std::string{ " not registered" });
            }
        }
        #endif

        auto EntityManager::begin() -> EntityManager::Iterator
        {
            return { *this, 0 };
        }

        auto EntityManager::end() -> EntityManager::Iterator
        {
            return { *this, static_cast<Entity::PointerSize>(mEntityComponents.size()) };
        }

        auto EntityManager::begin() const -> EntityManager::ConstIterator
        {
            return { *this, 0 };
        }

        auto EntityManager::end() const -> EntityManager::ConstIterator
        {
            return { *this, static_cast<Entity::PointerSize>(mEntityComponents.size()) };
        }

        auto EntityManager::Clear() -> void
        {
            mNextIndex = 0;
            mVersions.clear();
            mFreeIndexes.clear();
            mEntityComponents.clear();
        }

        auto EntityManager::Size() const -> std::size_t
        {
            return mEntityComponents.size() - mFreeIndexes.size();
        }

    }
}