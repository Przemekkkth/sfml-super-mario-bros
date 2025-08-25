#ifndef ADDSCORECOMPONENT_H
#define ADDSCORECOMPONENT_H
#include "Component.h"

class AddScoreComponent : public Component
{
public:
    AddScoreComponent(int score, int addCoin = 0)
        : m_score{score}
        , m_addCoin{addCoin}
    {}

    int getScore() const { return m_score; }

    int getAddCoinCount() const { return m_addCoin; }

private:
    int m_score;
    int m_addCoin;
};

#endif // ADDSCORECOMPONENT_H
