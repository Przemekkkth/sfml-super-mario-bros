#ifndef ENTITY_H
#define ENTITY_H
#include "components/Component.h"

class Entity
{
public:
    Entity(){}
    Entity(const Entity& other) = delete;
    ~Entity(){}

    // Adds a component to the Entity, along with the arguments for the component
    template <typename ComponentType, typename... Args>
    inline ComponentType* addComponent(Args&&... arguments) {
        auto component = std::make_unique<ComponentType>(std::forward<Args>(arguments)...);
        auto* ptr = component.get();

        m_components.emplace_back(std::move(component));

        m_componentArray[getComponentTypeID<ComponentType>()] = ptr;
        m_componentBitset[getComponentTypeID<ComponentType>()] = true;

        return ptr;
    }

    // Removes all of a certain component type from the entity
    template <typename ComponentType>
    void remove() {
        if (!hasComponent<ComponentType>()) {
            return;
        }

        m_componentBitset[getComponentTypeID<ComponentType>()] = false;

        auto* toRemove = m_componentArray[getComponentTypeID<ComponentType>()];

        m_components.erase(std::remove_if(m_components.begin(), m_components.end(), [toRemove](auto& ptr) {
            return ptr.get() == toRemove;
        }));
    }

    template <typename A, typename B, typename... OTHERS>
    void remove() {
        if (!hasComponent<A>()) {
            remove<B, OTHERS...>();
            return;
        }

        remove<A>();

        remove<B, OTHERS...>();
    }

    // Removes all of the components from the entity
    bool clearComponents() {
        m_components.clear();
        return true;
    }

    // Returns the component it needs to get
    template <typename C>
    inline C* getComponent() {
        auto ptr(m_componentArray[getComponentTypeID<C>()]);
        return static_cast<C*>(ptr);
    }

    // Returns true if it has the one component
    template <typename C>
    inline bool hasComponent() const {
        return m_componentBitset[getComponentTypeID<C>()];
    }

    // Returns true if it has all of the specified components
    template <typename A, typename B, typename... OTHERS>
    inline bool hasComponent() const {
        return hasComponent<A>() && hasComponent<B, OTHERS...>();
    }

    // This is only here to allow the hasAny() to work with one template argument
    template <typename A>
    inline bool hasAny() const {
        return hasComponent<A>();
    }

    // Returns true if it has at least one of the specified components
    template <typename A, typename B, typename... OTHERS>
    inline bool hasAny() const {
        return hasComponent<A>() || hasAny<B, OTHERS...>();
    }

private:
    std::vector<std::unique_ptr<Component>> m_components;
    ComponentArray m_componentArray;
    ComponentBitSet m_componentBitset;
};

#endif // ENTITY_H
