#include "Game.h"
#include "commands/CommandScheduler.h"
#include "commands/PrintCommand.h"
#include "commands/WaitCommand.h"
#include "commands/SequenceCommand.h"

#include <iostream>
TextureHolder* Game::s_textureHolder = new TextureHolder;
FontHolder* Game::s_fontHolder = new FontHolder;
SoundBufferHolder *Game::s_soundBufferHolder = new SoundBufferHolder;

Game::Game()
    : m_elapsed(0.0f), m_stateManager(&m_context)
{
    m_clock.restart();
    srand(time(nullptr));
    loadTextures();
    loadFonts();
    loadMaps();
    loadSounds();
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

void Game::loadSounds()
{
    s_soundBufferHolder->load(SoundID::BlockBreak, "assets/sounds/effects/blockbreak.wav");
    s_soundBufferHolder->load(SoundID::BlockHit, "assets/sounds/effects/blockhit.wav");
    s_soundBufferHolder->load(SoundID::BowserFall, "assets/sounds/effects/bowserfall.wav");
    s_soundBufferHolder->load(SoundID::BowserFire, "assets/sounds/effects/bowserfire.wav");
    s_soundBufferHolder->load(SoundID::CannonFire, "assets/sounds/effects/cannonfire.wav");
    s_soundBufferHolder->load(SoundID::CastleClear, "assets/sounds/effects/castleclear.wav");
    s_soundBufferHolder->load(SoundID::Coin, "assets/sounds/effects/coin.wav");
    s_soundBufferHolder->load(SoundID::Death, "assets/sounds/effects/death.wav");
    s_soundBufferHolder->load(SoundID::Fireball, "assets/sounds/effects/fireball.wav");
    s_soundBufferHolder->load(SoundID::FlagRaise, "assets/sounds/effects/flagraise.wav");
    s_soundBufferHolder->load(SoundID::GameOver, "assets/sounds/effects/gameover.wav");
    s_soundBufferHolder->load(SoundID::Jump, "assets/sounds/effects/jump.wav");
    s_soundBufferHolder->load(SoundID::Kick, "assets/sounds/effects/kick.wav");
    s_soundBufferHolder->load(SoundID::OneUp, "assets/sounds/effects/oneup.wav");
    s_soundBufferHolder->load(SoundID::Pause, "assets/sounds/effects/pause.wav");
    s_soundBufferHolder->load(SoundID::Pipe, "assets/sounds/effects/pipe.wav");
    s_soundBufferHolder->load(SoundID::PowerUpAppear, "assets/sounds/effects/powerupappear.wav");
    s_soundBufferHolder->load(SoundID::PowerUpCollect, "assets/sounds/effects/powerupcollect.wav");
    s_soundBufferHolder->load(SoundID::Shrink, "assets/sounds/effects/shrink.wav");
    s_soundBufferHolder->load(SoundID::Stomp, "assets/sounds/effects/stomp.wav");
    s_soundBufferHolder->load(SoundID::TimerTick, "assets/sounds/effects/timertick.wav");
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
