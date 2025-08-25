#ifndef SYSTEM_H
#define SYSTEM_H
#include <cstdint>
#include <bitset>
#include <optional>
#include <SFML/Window/Event.hpp>

constexpr std::uint8_t maxSystems = 16;

using SystemID = std::uint8_t;
using SystemBitset = std::bitset<maxSystems>;

inline SystemID getNewSystemTypeID() {
    static SystemID lastID = 0;
    return lastID++;
}

template <typename T>
inline SystemID getSystemTypeID() {
    static SystemID typeID = getNewSystemTypeID();
    return typeID;
}

class World;
class System {
    friend class World;

public:
    System() = default;
    virtual ~System() = default;

    virtual void onAddedToWorld(World* world) {}

    virtual void tick(World* world) = 0;

    virtual void handleInput(const std::optional<sf::Event> &event) {}

    virtual void handleInput() {}

    virtual void onRemovedFromWorld(World* world) {}

    virtual void render() {}

    void setEnabled(bool val) {
        m_enabled = val;
    }

    bool isEnabled() {
        return m_enabled;
    }

private:
    System(const System& other) = delete;
    System& operator=(const System& other) = delete;
    bool m_enabled = true;
};

using SystemArray = std::array<System*, maxSystems>;



#endif // SYSTEM_H
