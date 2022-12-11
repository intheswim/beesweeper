/*
 *  settings.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "settings.h"
#include "game.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>


#if !defined (_MSC_VER)
#define sprintf_s(buf, ...) snprintf((buf), sizeof(buf), __VA_ARGS__)
#endif

namespace po = boost::program_options;

const char * gameSettings::szConfigIni = "config.ini" ;
const char * gameSettings::szHex = "hex";
const char * gameSettings::szLevel = "level";
const char * gameSettings::szScale = "scale";
const char * gameSettings::szWidth = "width";
const char * gameSettings::szHeight = "height";
const char * gameSettings::szSound = "sound";
const char * gameSettings::szBlue = "blue";
const char * gameSettings::szBatch = "batch";
const char * gameSettings::szAuto = "auto";
const char * gameSettings::szQMarks = "qmarks";

bool gameSettings::save () const
{
    const char * config_loc = getenv ("SNAP_USER_DATA");
    if (config_loc == nullptr)
        config_loc = getenv ("HOME");

    std::string full_path = config_loc ? (std::string(config_loc) + "/" + szConfigIni) : std::string (szConfigIni);


#if defined(_MSC_VER)
    FILE* fp = nullptr;
    fopen_s (&fp, full_path.c_str(), "wt");
#else
    FILE* fp = fopen(full_path.c_str(), "wt");
#endif

    if (!fp) return false;

    fprintf (fp, "%s = %d\n", szHex, game_hex ? 1 : 0);
    fprintf (fp, "%s = %d\n", szLevel, level);
    fprintf (fp, "%s = %d\n", szWidth, width );
    fprintf (fp, "%s = %d\n", szHeight, height);
    fprintf (fp, "%s = %d\n", szScale, scale ? 1 : 0);
    fprintf (fp, "%s = %d\n", szAuto, auto_marks ? 1 : 0);
    fprintf (fp, "%s = %d\n", szQMarks, show_qmarks ? 1 : 0);
    fprintf (fp, "%s = %d\n", szBatch, batch_mode ? 1 : 0);
    fprintf (fp, "%s = %d\n", szBlue, highlight ? 1 : 0);
    fprintf (fp, "%s = %d\n", szSound, sound_volume);

    return (0 == fclose (fp));
}

static void on_level(int level)
{
  if (level < 0 || level >= LEVEL_MAX)
  {
    std::cout << "Invalid level: " << level << '\n';
    assert (0);
  }
}

static void on_height(int size)
{
    if (size < MIN_HEIGHT || size > MAX_HEIGHT)
    {
        std::cout << "Invalid height: " << size << '\n';
        assert (0);
    }
}

static void on_width (int size)
{
    if (size < MIN_WIDTH || size > MAX_WIDTH)
    {
        std::cout << "Invalid width: " << size << '\n';
        assert (0);
    }
}


static void on_volume (int volume)
{
    if (volume < 0 || volume > MIX_MAX_VOLUME)
    {
        std::cout << "Invalid sound volume: " << volume << '\n';
        assert (0);
    }
}

std::string gameSettings::rangeString (const std::string str, int minv, int maxv)
{
    char temp[64];

    sprintf_s (temp, "(%d-%d)", minv, maxv);

    return str + std::string (temp);
}


int gameSettings::parse (int argc, char * argv[])
{
    po::options_description desc ("Options");
        desc.add_options ()
            ("help,h", "produce help message")
            (szHex, po::value<bool>()->default_value(false), "Hexagons (default no)")
            (szLevel, po::value<int>()->default_value(LEVEL_BEGINNER)->notifier(on_level), rangeString ("Level ", 0, 2).c_str() )
            (szAuto, po::value<bool>()->default_value(true), "Auto marks")
            (szQMarks, po::value<bool>()->default_value(true), "Show q-marks")            
            (szBatch, po::value<bool>()->default_value(true), "Batch mode")             
            (szWidth, po::value<int>()->default_value(DEF_WIDTH)->notifier(on_width), rangeString ("Width ", MIN_WIDTH, MAX_WIDTH).c_str())
            (szHeight, po::value<int>()->default_value(MIN_HEIGHT)->notifier(on_height), rangeString ("Height ", MIN_HEIGHT, MAX_HEIGHT).c_str())
            (szBlue, po::value<bool>()->default_value(true), "Highlight cells")
            (szSound, po::value<int>()->default_value(MIX_MAX_VOLUME / 2)->notifier(on_volume), rangeString ("Sound volume ", 0, MIX_MAX_VOLUME).c_str())
            (szScale, po::value<bool>()->default_value(true), "Scale screen 2x");

    po::variables_map vm;

    const char * config_loc = getenv ("SNAP_USER_DATA");
    if (config_loc == nullptr)
        config_loc = getenv ("HOME");

    if (argc == 1) // when no arguments given, read config file.
    {
        std::string full_path = config_loc ? (std::string(config_loc) + "/" + szConfigIni) : std::string(szConfigIni);
        
        std::ifstream ini_file(full_path.c_str());

        po::store(parse_config_file(ini_file, desc), vm);
    }
    else 
    {
        po::store(parse_command_line(argc, argv, desc), vm);
    }
    
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << '\n';
        return HELP_ONLY;
    }

    level = LEVEL_BEGINNER;
    auto_marks = true;
    width = MIN_WIDTH;
    height = MIN_HEIGHT;
    highlight = false;
    batch_mode = false;
    sound_volume = MIX_MAX_VOLUME / 2;
    game_hex  = false;
    scale = true;

    if (vm.count(szHex))
    {
        game_hex = vm[szHex].as<bool>();
    }
    if (vm.count(szLevel))
    {
        level = vm[szLevel].as<int>();
    }
    if (vm.count(szAuto))
    {
        auto_marks = vm[szAuto].as<bool>();
    }
    if (vm.count(szQMarks))
    {
        show_qmarks = vm[szQMarks].as<bool>();
    }
    if (vm.count(szWidth))
    {
        width = vm[szWidth].as<int>();
    }
    if (vm.count(szHeight))
    {
        height = vm[szHeight].as<int>();
    }
    if (vm.count(szSound))
    {
        sound_volume = vm[szSound].as<int>();
    }
    if (vm.count(szBlue))
    {
        highlight = vm[szBlue].as<bool>();
    }
    if (vm.count(szBatch))
    {
        batch_mode = vm[szBatch].as<bool>();
    }
    if (vm.count(szScale))
    {
        scale = (vm[szScale].as<bool>());
    }    

    return SETTINGS_OK;
}
