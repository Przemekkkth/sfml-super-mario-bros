#ifndef TRAMPOLINECOMPONENT_H
#define TRAMPOLINECOMPONENT_H
#include "../Entity.h"
#include "Component.h"

class TrampolineComponent : public Component
{
public:
    TrampolineComponent(Entity *bottomEntity, int topIDS[], int bottomIDS[])
        : m_bottomEntity{bottomEntity}
    {
        m_topExtendedID = topIDS[0];
        m_topMediumRetractedID = topIDS[1];
        m_topRetractedID = topIDS[2];

        m_bottomExtendedID = bottomIDS[0];
        m_bottomMediumRetractedID = bottomIDS[1];
        m_bottomRetractedID = bottomIDS[2];
    }

    Entity *getBottomEntity() { return m_bottomEntity; }

    int &getCurrentSequenceIndex() { return m_currentSequenceIndex; }

    int topExtendedID() const { return m_topExtendedID; }

    int topMediumRetractedID() const { return m_topMediumRetractedID; }

    int topRetractedID() const { return m_topRetractedID; }

    int bottomExtended() const { return m_bottomExtendedID; }

    int bottomMediumRetractedID() const { return m_bottomMediumRetractedID; }

    int bottomRetractedID() const { return m_bottomRetractedID; }

private:
    Entity* m_bottomEntity;

    int m_currentSequenceIndex = 0;

    int m_topExtendedID;
    int m_topMediumRetractedID;
    int m_topRetractedID;

    int m_bottomExtendedID;
    int m_bottomMediumRetractedID;
    int m_bottomRetractedID;
};

#endif // TRAMPOLINECOMPONENT_H
