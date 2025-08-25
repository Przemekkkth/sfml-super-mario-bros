#include "Game.h"
int main()
{
    auto game = Game();
    while (!game.GetWindow()->IsDone()) {
        game.HandleInput();
        game.Update();
        game.Render();
        game.LateUpdate();
    }
}
