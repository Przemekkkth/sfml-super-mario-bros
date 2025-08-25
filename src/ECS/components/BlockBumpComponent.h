#ifndef BLOCKBUMPCOMPONENT_H
#define BLOCKBUMPCOMPONENT_H
#include "Component.h"
#include <vector>

class BlockBumpComponent : public Component
{
public:
    BlockBumpComponent(std::vector<int> &yChanges)
        : m_yChanges(yChanges)
        , m_yChangeIndex{0}
    {}

    int getYChange(int idx) const { return m_yChanges[idx]; }

    void setYChangeIndex(int idx) { m_yChangeIndex = idx; }

    int getCurrentYChanged() const { return m_yChanges[m_yChangeIndex]; }

    std::vector<int> getYChanges() const { return m_yChanges; }

    int yChangeIndex() const { return m_yChangeIndex; }

private:
    std::vector<int> m_yChanges;
    int m_yChangeIndex;
};

#endif // BLOCKBUMPCOMPONENT_H
