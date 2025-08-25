#include "Window.h"
#include "Globals.h"

Window::Window(const sf::String &title, const sf::Vector2u &size)
{
    Setup(title, size);
}

Window::Window()
{
    Setup(GLOBALS::WINDOW_TITLE, GLOBALS::WINDOW_SIZE);
}

Window::~Window()
{

}

void Window::BeginDraw()
{
    m_window.clear();
}

void Window::EndDraw()
{
    m_window.display();
}

void Window::Update()
{

}

void Window::ToggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    m_window.close();
    Create();
}

void Window::Draw(sf::Drawable &drawable)
{
    m_window.draw(drawable);
}

void Window::HandlePlayerInputs(const std::optional<sf::Event> &event)
{
    if (event->is<sf::Event::Closed>())
    {
        m_isDone = true;
    }
    else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
        {
            m_isDone = true;
            m_window.close();
        }
        else if (keyPressed->scancode == sf::Keyboard::Scancode::F5)
        {
            ToggleFullscreen();
        }
    }
}

void Window::Setup(const sf::String &title, const sf::Vector2u &size)
{
    m_windowTitle = title;
    m_windowSize = size;
    m_isFullscreen = false;
    m_isDone = false;
    Create();
}

void Window::Create()
{
    auto style = (m_isFullscreen ? sf::State::Fullscreen
                                 : sf::State::Windowed);
    sf::VideoMode videoMode{sf::Vector2u( m_windowSize.x, m_windowSize.y)};
    m_window.create(videoMode, m_windowTitle, style);
    m_window.setActive(true);
}

void Window::Destroy()
{
    m_window.close();
}
