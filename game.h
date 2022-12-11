/*
 *  game.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#else 
#include <SDL.h>
#include <SDL_mixer.h>
#endif

#include "GenericField.h"
#include "digits.h"
#include "faceButton.h"
#include "settingsButton.h"
#include "image.h"
#include "MusicChunk.h"
#include "SDLTimer.h" 

#include "settings.h"
#include "settingsDialog.h"

class Game 
{
    public:
    Game();
    ~Game();

    // forbid coping since we have raw pointer in class.
    Game(const Game &) = delete;
    Game& operator = (const Game &) = delete;

    bool init (gameSettings & settings);

    void handleEvents(); // return true to re-draw

    void render();

    bool update() const { return m_redraw; }

    bool isRunning () const { return m_isRunning; }

    void startNewGame (); // same parameters as before

    // void setSoundVolume (int value) { m_settings.sound_volume = value; }

    SDL_Window * getWindow () const { return window; }

    private:

    bool init (bool hex_game, bool scale, int rows, int cols, bool highlight);

    bool init (bool hex_game, bool scale, int xpos, int ypos, int width, int height, bool highlight_cells);

    void startNewGame (GameLevel, bool batch_mode, bool auto_marks, bool show_qmarks = true, bool flag_mode = false);

    static void getRequiredScreenDimensions (bool hex_game, int cell_cols, int cell_rows, int & width, int & height);

    void setupLayout (bool hex_game, int width, int height, bool highlight, bool scale);

    bool onLeftMouseUp(int x, int y);

    bool onLeftMouseDown (bool rightAlsoPressed, int x, int y);

    bool onRightMouseDown (bool leftAlsoPressed, int x, int y);

    bool onRightMouseUp (int x, int y);

    bool onMouseMove (int x, int y);

    void clean();

    bool startTimer();

    void stopTimer();

    void timerFiredMethod () ;

    bool loadMedia();

    void applyNewSettings (const struct gameSettings newSettings);

    static DigitColor DigitColorFromLevel(GameLevel level);

    static std::string getWindowTitle (bool game_hex);

    static int scaledValue (int value, int scale) { return scale ? (value << 1) : value; }

    void reloadImages (SDL_Renderer *g, bool scaled);

    bool m_isRunning = false;
    bool m_redraw = false;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    DigitsPtr secondsView = nullptr, minesView = nullptr;

    FaceButtonPtr faceButton = nullptr;

    GenericField *game_field = nullptr;

    TimerPtr timer = nullptr;

    MusicChunkUPtr gErrorSound = nullptr, gVictorySound = nullptr;

    Uint32 m_userEventID = 0;

    SettingsButtonPtr settingsBtn = nullptr;

    struct gameSettings m_settings = { 0 };

    SettingsDialog * settingsDialog = nullptr;
};