#ifndef WINDOW_H
#define WINDOW_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Window
{
public:
    Window(const sf::String& title, const sf::Vector2u& size);
    Window();
    ~Window();

    void BeginDraw();
    void EndDraw();

    void Update();

    bool IsDone() { return m_isDone; }
    bool IsFullscreen() { return m_isFullscreen; }
    sf::RenderWindow* GetRenderWindow() { return &m_window; }
    sf::Vector2u GetWindowSize() { return m_windowSize; }

    void ToggleFullscreen();

    void Draw(sf::Drawable& drawable);
    void HandlePlayerInputs(const std::optional<sf::Event>& event);
private:
    void Setup(const sf::String& title, const sf::Vector2u& size);
    void Create();
    void Destroy();

    sf::RenderWindow m_window;
    sf::Vector2u m_windowSize;
    sf::String m_windowTitle;
    bool m_isDone;
    bool m_isFullscreen;
};

#endif // WINDOW_H
