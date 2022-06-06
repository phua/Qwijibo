#include "../include/fish.hpp"

int main(int argc, char *argv[])
{
    KGame *game = new FishDish();
    while (game->handleEvents()) {
        game->update();
        game->render();
    }

    return 0;
}
