#include <iostream>
#include "game.hpp"
#include <SDL.h>

int main()
{
    try {
        GameManager game;
        game.start();
    } catch (std::runtime_error &err) {
        std::cout << "Runtime Error:\n" << err.what() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Runtime Error", err.what(), nullptr);
        return 1;
    }
    return 0;
}
