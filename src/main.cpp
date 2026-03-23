#include <SDL3/SDL_main.h>
#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    Game game;
    if (!game.init()) {
        std::cerr << "Failed to initialize game\n";
        return 1;
    }
    game.run();
    game.shutdown();
    return 0;
}
