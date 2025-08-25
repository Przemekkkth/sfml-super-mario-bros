cmake_minimum_required(VERSION 3.28)
project(SFML-Mario LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(mario
    src/Game.cpp
    src/Game.h
    src/Globals.h
    src/Main.cpp
    src/SharedContext.h
    src/Window.cpp
    src/Window.h
    src/GameWorld.cpp
    src/GameWorld.h

    src/states/BaseState.h
    src/states/GameState.cpp
    src/states/GameState.h
    src/states/MenuState.cpp
    src/states/MenuState.h
    src/states/StateManager.cpp
    src/states/StateManager.h
    src/states/PausedState.cpp
    src/states/PausedState.h


    src/states/GameOverState.h src/states/GameOverState.cpp

    src/utils/ResourceHolder.h
    src/utils/ResourceIdentifiers.h

    src/commands/Command.h src/commands/Command.cpp
    src/commands/CommandScheduler.h src/commands/CommandScheduler.cpp
    src/commands/PrintCommand.h
    src/commands/DelayedCommand.h
    src/commands/RunCommand.h
    src/commands/SequenceCommand.h
    src/commands/WaitCommand.h
    src/commands/WaitUntilCommand.h
    src/commands/VineCommand.h
    src/commands/WarpCommand.h
    src/Map.h src/Map.cpp

    src/ECS/components/Component.h
    src/ECS/Entity.h
    src/ECS/systems/System.h
    src/ECS/World.h
    src/ECS/components/TransformComponent.h
    src/ECS/components/TextureComponent.h
    src/ECS/components/SpritesheetComponent.h
    src/ECS/systems/MenuSystem.h src/ECS/systems/MenuSystem.cpp
    src/ECS/components/TextComponent.h
    src/Camera.h src/Camera.cpp
    src/ECS/components/BackgroundComponent.h
    src/ECS/components/ForegroundComponent.h
    src/ECS/components/ProjectileComponent.h
    src/ECS/components/CollectibleComponent.h
    src/ECS/components/EnemyComponent.h
    src/ECS/components/FloatingTextComponent.h
    src/ECS/components/PlayerComponent.h
    src/ECS/components/AboveForegroundComponent.h
    src/ECS/components/ParticleComponent.h
    src/ECS/components/IconComponent.h
    src/ECS/systems/RenderSystem.h src/ECS/systems/RenderSystem.cpp
    src/ECS/systems/OptionsSystem.h src/ECS/systems/OptionsSystem.cpp
    src/ECS/systems/GameplayStatsSystem.h src/ECS/systems/GameplayStatsSystem.cpp
    src/states/PreGameState.h src/states/PreGameState.cpp
    src/ECS/components/AnimationComponent.h
    src/ECS/components/PausedAnimationComponent.h
    src/ECS/components/EndingBlinkComponent.h
    src/ECS/systems/AnimationSystem.h src/ECS/systems/AnimationSystem.cpp
    src/GameData.h
    src/ECS/systems/PausedSystem.h src/ECS/systems/PausedSystem.cpp
    src/ECS/systems/MapSystem.h src/ECS/systems/MapSystem.cpp
    src/GameData.cpp
    src/LevelData.h src/LevelData.cpp
    src/ECS/components/FlagPoleComponent.h
    src/ECS/components/MovingComponent.h
    src/ECS/components/FlagComponent.h
    src/ECS/components/TileComponent.h
    src/ECS/components/BumpableComponent.h
    src/ECS/components/MysteryBoxComponent.h
    src/ECS/components/AddLivesComponent.h
    src/ECS/components/AddScoreComponent.h
    src/ECS/components/AxeComponent.h
    src/ECS/components/BlockBumpComponent.h
    src/ECS/components/BottomCollisionComponent.h
    src/ECS/components/BowserComponent.h
    src/ECS/components/BridgeComponent.h
    src/ECS/components/BridgeChainComponent.h
    src/ECS/components/CallbackComponent.h
    src/ECS/components/CollisionExemptComponent.h
    src/ECS/components/CreateFloatingTextComponent.h
    src/ECS/components/CrushableComponent.h
    src/ECS/components/CrushedComponent.h
    src/ECS/components/DeadComponent.h
    src/ECS/components/DestroyDelayedComponent.h
    src/ECS/components/DestroyOutsideCameraComponent.h
    src/ECS/components/DestructibleComponent.h
    src/ECS/components/DownCollisionComponent.h
    src/ECS/components/EnemyDestroyedComponent.h
    src/ECS/components/FireBarComponent.h
    src/ECS/components/FrictionExemptComponent.h
    src/ECS/components/FrozenComponent.h
    src/ECS/components/GravityComponent.h
    src/ECS/components/HammerBroComponent.h
    src/ECS/components/InvisibleBlockComponent.h
    src/ECS/components/LakituComponent.h
    src/ECS/components/LeftCollisionComponent.h
    src/ECS/components/MoveOutsideCameraComponent.h
    src/ECS/components/MovingPlatformComponent.h
    src/ECS/components/MusicComponent.h
    src/ECS/components/PiranhaPlantComponent.h
    src/ECS/components/PlatformLevelComponent.h
    src/ECS/components/RightCollisionComponent.h
    src/ECS/components/SoundComponent.h
    src/ECS/components/TimerComponent.h
    src/ECS/components/TopCollisionComponent.h
    src/ECS/components/TrampolineComponent.h
    src/ECS/components/VineComponent.h
    src/ECS/components/WaitUntilComponent.h
    src/ECS/components/WarpPipeComponent.h

    src/utils/PIDController.h src/utils/PIDController.cpp
    src/utils/AABB.h src/utils/AABB.cpp
    src/ECS/systems/CallbackSystem.h src/ECS/systems/CallbackSystem.cpp
    src/ECS/systems/PlayerSystem.h src/ECS/systems/PlayerSystem.cpp
    src/ECS/systems/HUDSystem.h src/ECS/systems/HUDSystem.cpp
    src/ECS/systems/PhysicsSystem.h src/ECS/systems/PhysicsSystem.cpp
    src/ECS/systems/CollectibleSystem.h src/ECS/systems/CollectibleSystem.cpp
    src/ECS/systems/FlagSystem.h src/ECS/systems/FlagSystem.cpp
    src/ECS/systems/EnemySystem.h src/ECS/systems/EnemySystem.cpp
    src/ECS/systems/WarpSystem.h src/ECS/systems/WarpSystem.cpp

)

target_compile_features(mario PRIVATE cxx_std_17)
target_link_libraries(mario PRIVATE SFML::Graphics SFML::Audio)

target_include_directories(mario
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/json/include
)


add_custom_command(TARGET mario POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)
