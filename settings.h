/*
 *  settings.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#include <string>

#define MIN_WIDTH   12
#define MAX_WIDTH   48
#define MIN_HEIGHT  10
#define MAX_HEIGHT  40
#define DEF_WIDTH   20 // make sure it's between min and max

enum { HELP_ONLY = 0, SETTINGS_OK };

struct gameSettings
{
    bool game_hex ;
    bool auto_marks ;
    bool show_qmarks;
    bool batch_mode ;
    bool highlight ;
    bool scale ;
    int level;
    int width, height ;
    int sound_volume ;
    bool save () const;

    int parse (int argc, char * argv[]);

    private:

    static std::string rangeString (const std::string str, int minv, int maxv);

    const static char * szConfigIni ;
    const static char * szScale, *szHex, *szLevel;
    const static char * szWidth, *szHeight;
    const static char * szSound, *szBlue;
    const static char * szBatch, *szAuto, *szQMarks;
};