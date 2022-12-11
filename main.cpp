/*
 *  main.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "game.h"
#include "settings.h"
#include "fileUtils.h"

#include <iostream>
#include <fstream>


/////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    gameSettings settings = { 0 };
    settings.level = LEVEL_BEGINNER;
    settings.auto_marks = true;
    settings.width = MIN_WIDTH;
    settings.height = MIN_HEIGHT;
    settings.highlight = false;
    settings.batch_mode = false;
    settings.sound_volume = 64;
    settings.game_hex  = false;
    settings.scale = true;

    if (HELP_ONLY == settings.parse (argc, argv))
    {
        return EXIT_SUCCESS;
    }

    SDL_LogSetPriority (SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN);

    // FileUtils::initPrefix ("/usr/local/bin");

    if (getenv ("SNAP"))
    {
        FileUtils::initPrefix (getenv("SNAP"));
    }

    Game game;

    if (false == game.init (settings))
    {
        return EXIT_FAILURE;
    }

    game.startNewGame();

    while (game.isRunning())
    {
        game.handleEvents();

        if (game.update())
        {
            game.render();
        }
    }

    return EXIT_SUCCESS;
}