#ifndef COMPONENT_H
#define COMPONENT_H
#include <bitset>
#include <cstdint>
#include <iostream>
#include <vector>

constexpr std::uint8_t maxComponents = 64;
using ComponentID = std::uint8_t;
using ComponentBitSet = std::bitset<maxComponents>;

inline ComponentID getNewComponentTypeID() {
    static ComponentID lastID = 0;
    //std::cout << std::to_string(lastID) << '\n';
    return lastID++;
}

template <typename T>
inline ComponentID getComponentTypeID() noexcept {
    static ComponentID typeID = getNewComponentTypeID();
    return typeID;
}

class Component {
public:
    Component() = default;
    virtual ~Component() = default;
private:
    Component(const Component& other) = delete;
    Component& operator=(const Component& other) = delete;
};

using ComponentArray = std::array<Component*, maxComponents>;

#endif // COMPONENT_H
