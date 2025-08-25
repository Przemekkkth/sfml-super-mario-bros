#include "Game.h"
#include "commands/CommandScheduler.h"
#include "commands/PrintCommand.h"
#include "commands/WaitCommand.h"
#include "commands/SequenceCommand.h"

#include <iostream>
TextureHolder* Game::s_textureHolder = new TextureHolder;
FontHolder* Game::s_fontHolder = new FontHolder;

Game::Game()
    : m_elapsed(0.0f), m_stateManager(&m_context)
{
    m_clock.restart();
    srand(time(nullptr));
    loadTextures();
    loadFonts();
    loadMaps();
    m_context.m_window = &m_window;
    m_stateManager.SwitchTo(StateType::MainMenu);
}

Game::~Game()
{

}

sf::Time Game::GetElapsed()
{
    return m_clock.getElapsedTime();
}

void Game::RestartClock()
{
    m_elapsed += m_clock.restart().asSeconds();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::loadTextures()
{
    s_textureHolder->load(TextureID::Avatar, "assets/avatar.png");
    s_textureHolder->load(TextureID::TitleLogo, "assets/sprites/icons/logo.png");
    s_textureHolder->load(TextureID::OptionsInfoBackground, "assets/sprites/icons/optionsinfobackground.png");
    s_textureHolder->load(TextureID::OptionsBackground, "assets/sprites/icons/optionsbackground.png");
    s_textureHolder->load(TextureID::BlockTileSheet, "assets/sprites/blocks/BlockTileSheet.png");
    s_textureHolder->load(TextureID::PlayerSprteSheet,
                          "assets/sprites/characters/PlayerSpriteSheet.png");
    s_textureHolder->load(TextureID::EnemySpriteSheet,
                          "assets/sprites/characters/EnemySpriteSheet.png");
    s_textureHolder->load(TextureID::SFMLLogo, "assets/sfml-icon-small.png");
    m_context.m_textureHolder = s_textureHolder;
}

void Game::loadFonts()
{
    s_fontHolder->loadFont(FontID::Main, "assets/fonts/press-start-2p.ttf");
}

void Game::loadMaps()
{
    Map::loadBlockIDS();
    Map::loadEnemyIDS();
    Map::loadIrregularBlockReferences();
    Map::loadPlayerIDS();
}

void Game::HandleInput()
{
    while (const std::optional event = GetWindow()->GetRenderWindow()->pollEvent())
    {
        GetWindow()->HandlePlayerInputs(event);
        m_stateManager.HandlePlayerInput(event);
    }
}

void Game::Update()
{
    float timestep = 1.0f / 60.0f;

    while (m_elapsed >= timestep) {
        m_stateManager.Update(sf::seconds(timestep));
        m_elapsed -= timestep;
        CommandScheduler::getInstance().run();
    }
}

void Game::Render()
{
    m_window.BeginDraw();
    m_stateManager.Draw();
    m_window.EndDraw();
}

void Game::LateUpdate()
{
    m_stateManager.ProcessRequests();
    RestartClock();
}
