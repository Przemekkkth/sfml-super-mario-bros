#ifndef WORLD_H
#define WORLD_H
#include "Entity.h"
#include "systems/System.h"
#include <SFML/Window/Event.hpp>
#include <optional>

class World {
public:
    World() = default;

    World(const World& other) = delete;

    ~World()
    {
        for (auto& system : m_systems)
        {
            system->onRemovedFromWorld(this);
        }
        m_systems.clear();

        for (auto entity : m_entities)
        {
            delete entity;
        }
        m_entities.clear();
    }

    Entity* create()
    {
        auto* entity = new Entity();
        m_entities.push_back(entity);
        return entity;
    }

    void destroy(Entity* entity)
    {
        assert(entity && "Destroying non-existent entity.");

        m_destroyQueue.push_back(entity);
    }

    void emptyDestroyQueue()
    {
        for (Entity* entity : m_destroyQueue)
        {
            m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(),
                                          [entity](Entity* other) {
                                              return entity == other;
                                          }),
                           m_entities.end());

            delete entity;
        }

        m_destroyQueue.clear();
    }

    template <typename... Components>
    void destroyAll()
    {
        for (Entity* entity : m_entities)
        {
            if (entity->hasComponent<Components...>())
            {
                destroy(entity);
            }
        }
    }

    template <typename S, typename... Args>
    S* registerSystem(Args&&... arguments)
    {
        std::unique_ptr<S> system = std::make_unique<S>(std::forward<Args>(arguments)...);

        auto* ptr = system.get();

        m_systemBitset[getSystemTypeID<S>()] = true;

        m_systemArray[getSystemTypeID<S>()] = ptr;
        m_systems.emplace_back(std::move(system));

        ptr->onAddedToWorld(this);
        return ptr;
    }

    template <typename T>
    void unregisterSystem()
    {
        if (!hasSystem<T>())
        {
            return;
        }

        m_systemBitset[getSystemTypeID<T>()] = false;

        auto* toRemove = m_systemArray[getSystemTypeID<T>()];

        m_systems.erase(std::remove_if(m_systems.begin(), m_systems.end(), [toRemove, this](auto& ptr) {
            if (ptr.get() == toRemove) {
                ptr->onRemovedFromWorld(this);
                return true;
            }
            return false;
        }));
    }

    template <typename T>
    bool hasSystem()
    {
        return m_systemBitset[getSystemTypeID<T>()];
    }

    template <typename T>
    T* getSystem()
    {
        auto ptr(m_systemArray[getSystemTypeID<T>()]);
        return static_cast<T*>(ptr);
    }

    template <typename T>
    void disableSystem()
    {
        getSystem<T>()->setEnabled(false);
    }

    template <typename A, typename B, typename... OTHERS>
    void disableSystem()
    {
        disableSystem<A>();

        disableSystem<B, OTHERS...>();
    }

    template <typename T>
    void enableSystem()
    {
        getSystem<T>()->setEnabled(true);
    }

    template <typename A, typename B, typename... OTHERS>
    void enableSystem()
    {
        enableSystem<A>();

        enableSystem<B, OTHERS...>();
    }

    bool hasEntity(Entity* entity)
    {
        auto it = std::find(m_entities.begin(), m_entities.end(), entity);

        return it != m_entities.end();
    }

    template <typename... Components>
    Entity* findFirst()
    {
        auto found = std::find_if(m_entities.begin(), m_entities.end(), [&](const Entity* entity) {
            return entity->hasComponent<Components...>();
        });
        return found != m_entities.end() ? *found : nullptr;
    }

    template <typename... Components, typename Function>
    void find(Function callback)
    {
        for (Entity* entity : m_entities)
        {
            if (entity->hasComponent<Components...>())
            {
                callback(entity);
            }
        }
    }

    template <typename... Components>
    std::vector<Entity*> findAny()
    {
        std::vector<Entity*> result;
        std::copy_if(m_entities.begin(), m_entities.end(), std::back_inserter(result),
                     [&](const Entity* s) {
                         return s->hasAny<Components...>();
                     });
        return result;
    }

    void tick()
    {
        for (auto& system : m_systems)
        {
            if (system->isEnabled())
            {
                system->tick(this);
                emptyDestroyQueue();
            }
        }
    }

    void render()
    {
        for (auto& system : m_systems)
        {
            if (system->isEnabled())
            {
                system->render();
                emptyDestroyQueue();
            }
        }
    }

    void handleInput()
    {
        for (auto& system : m_systems)
        {
            if (system->isEnabled())
            {
                system->handleInput();
            }
        }
    }

    void handleInput(const std::optional<sf::Event> &event)
    {
        for (auto& system : m_systems)
        {
            if(system->isEnabled())
            {
                system->handleInput(event);
            }
        }
    }

    std::vector<Entity*>& getEntities()
    {
        return m_entities;
    }

private:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_destroyQueue;

    SystemArray m_systemArray;
    SystemBitset m_systemBitset;
    std::vector<std::unique_ptr<System>> m_systems;
};

#endif // WORLD_H
