#ifndef RESOURCEIDENTIFIERS_H
#define RESOURCEIDENTIFIERS_H
#include "ResourceHolder.h"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/String.hpp>

enum class TextureID {
    Avatar,
    SFMLLogo,
    TitleLogo,
    OptionsInfoBackground,
    OptionsBackground,
    BlockTileSheet,
    PlayerSprteSheet,
    EnemySpriteSheet
};

enum class FontID
{
    Main
};

enum class Direction
{
    None = 0,
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4
};

enum class CollisionDirection
{
    None = 0,
    Top = 1,
    Bottom = 2,
    Left = 3,
    Right = 4
};

enum class PlatformMotionType
{
    None = 0,
    OneDirectionRepeated = 1,    // Moves in one direction, but goes to min point when it reaches max
    OneDirectionContinuous = 2,  // Continuously moving in one direction
    BackAndForth = 3,            // Moves back and forth
    Gravity = 4                  // Affected by Gravity when mario stands on it
};

enum class RotationDirection
{
    None = 0,
    Clockwise = 1,
    CounterClockwise = 2
};

enum class MysteryBoxType
{
    None = 0,
    Mushroom = 1,
    Coins = 2,
    SuperStar = 3,
    OneUp = 4,
    Vines = 5
};

enum class CollectibleType
{
    None = 0,
    Mushroom = 1,
    SuperStar = 2,
    FireFlower = 3,
    Coin = 4,
    OneUp = 5
};

enum class WarpPipeDirections
{
    None = 0,
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4
};

enum class LevelType
{
    None = 0,
    Overworld = 1,
    Underground = 2,
    Underwater = 3,
    Castle = 4,
    StartUnderground = 5
};

enum class EnemyType
{
    None,
    Blooper,
    Bowser,
    BulletBill,
    BuzzyBeetle,
    CheepCheep,
    FireBar,
    Goomba,
    HammerBro,
    Koopa,
    KoopaParatroopa,
    KoopaShell,
    Lakitu,
    LavaBubble,
    PiranhaPlant,
    Spine
};

enum class ProjectileType
{
    None, Fireball, Other
};

enum class MusicID
{
    Overworld = 0,
    Underground,
    Castle,
    Underwater,
    SuperStar,
    GameWon
};

enum class PlayerState
{
    SmallMario, SuperMario, FireMario
};

enum class SoundID
{
    BlockBreak = 0,
    BlockHit = 1,
    BowserFall = 2,
    BowserFire = 3,
    CannonFire = 4,
    Coin = 5,
    Death = 6,
    Fireball = 7,
    FlagRaise = 8,
    GameOver = 9,
    Jump = 10,
    Kick = 11,
    OneUp = 12,
    Pause = 13,
    Pipe = 14,
    PowerUpAppear = 15,
    PowerUpCollect = 16,
    Shrink = 17,
    Stomp = 18,
    TimerTick = 19,
    CastleClear = 20
};

enum class AnimationState {
    Standing,
    Walking = 1,
    Running = 2,
    Drifting = 3,
    Ducking = 4,
    Jumping = 5,
    Swimming = 6,
    Swimming_Jump = 7,
    Swimming_Walk = 8,
    Launch_Fireball = 9,
    Climbing = 10,
    Sliding = 11,
    Gameover = 12
};

enum class GrowType { OneUp = 0, Mushroom, SuperStart, FireFlower };

// Forward declaration and a few type definitions
template<typename Resource, typename Identifier>
class ResourceHolder;

using TextureHolder = ResourceHolder<sf::Texture, TextureID>;
using FontHolder = ResourceHolder<sf::Font, FontID>;
using SoundBufferHolder = ResourceHolder<sf::SoundBuffer, SoundID>;
using MusicStreamNameHolder = ResourceHolder<std::string, MusicID>;

#endif // RESOURCEIDENTIFIERS_H
