#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H
#include "Component.h"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "../../Globals.h"

class AnimationComponent : public Component
{
public:
    AnimationComponent(std::vector<int> frameIDS, int framesPerSecond, std::unordered_map<int, sf::Vector2i> &coordinateSupplier, bool repeated = true)
        :  m_playing{true}, m_frameIDS{frameIDS}, m_frameCount{(int)frameIDS.size()}, m_framesPerSecond{framesPerSecond},
        m_coordinateSupplier{coordinateSupplier},
        m_repeated{repeated}
    {
        m_frameDelay = (int)std::round((float)GLOBALS::FPS / (float)m_framesPerSecond);
    }

    void setPlaying(bool val)
    {
        m_playing = val;
    }

    bool isPlaying() const
    {
        return m_playing;
    }

    void setFramesPerSecond(int fps)
    {
        m_framesPerSecond = fps;
    }

    void setFrameTimer(int frameTimer) { m_currentFrame = frameTimer; }

    bool isFrameFinished() const
    {
        return m_frameTimer <= 0;
    }

    bool isRepeated() const
    {
        return m_repeated;
    }

    void setRepeated(bool on) { m_repeated = on; }

    bool isAnimationFinished()
    {
        return m_currentFrame >= m_frameCount;
    }

    void setCurrentFrame(int frame)
    {
        m_currentFrame = frame;
    }

    int getCurrentFrame() const
    {
        return m_currentFrame;
    }

    void increaseFrame()
    {
        ++m_currentFrame;
    }

    void decreaseFrameTimer()
    {
        --m_frameTimer;
    }

    void resetTimer()
    {
        m_frameTimer = m_frameDelay;
    }

    int getAnimationFrameID() const
    {
        return m_frameIDS[m_currentFrame];
    }

    sf::Vector2i getFrameCoordinates() const
    {
        return m_coordinateSupplier.at(getAnimationFrameID());
    }

    void setFrameIDs(std::vector<int> frameIDs) { m_frameIDS = frameIDs; }

    std::vector<int> getFrameIDs() const { return m_frameIDS; }

    void setFrameCount(int frameCount) { m_frameCount = frameCount; }

    std::vector<int> m_frameIDS;

private:
    bool m_playing;
    //std::vector<int> m_frameIDS;
    int m_frameCount;
    int m_framesPerSecond;
    int m_frameDelay;
    int m_frameTimer = 0;
    std::unordered_map<int, sf::Vector2i>& m_coordinateSupplier;
    bool m_repeated;
    int m_currentFrame = 0;
};

#endif // ANIMATIONCOMPONENT_H
