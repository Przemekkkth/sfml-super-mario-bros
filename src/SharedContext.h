#ifndef SHAREDCONTEXT_H
#define SHAREDCONTEXT_H

#include "Window.h"
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <iostream>
#include <cstdlib>
#include "utils/ResourceIdentifiers.h"
#include "utils/ResourceHolder.h"

struct SharedContext{
    SharedContext()
        : m_window(nullptr), m_textureHolder(nullptr)
    {
        if (!mainFont.openFromFile("assets/fonts/press-start-2p.ttf"))
        {
            exit(EXIT_FAILURE);
        }
    }

    SharedContext(TextureHolder* textureHolder)
        : m_window(nullptr), m_textureHolder(textureHolder)
    {

    }

    ~SharedContext()
    {

    }

    Window* m_window;
    sf::Font mainFont;
    TextureHolder* m_textureHolder;
};

#endif // SHAREDCONTEXT_H
