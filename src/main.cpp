#include "game.hpp"

int main()
{
    GameManager game;
    if (game.isRunning()) {
        game.start();
    }
    return 0;
}
