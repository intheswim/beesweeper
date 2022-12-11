/*
 *  settingsDialog.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#include "ui/ui.h"
#include "SDLFont.h"
#include "image.h"
#include <memory>
#include "settings.h"

class SettingsDialog
{
    public:
    SettingsDialog () { }
    ~SettingsDialog ();

    SettingsDialog (const SettingsDialog &) = delete;

    SettingsDialog& operator = (const SettingsDialog &) = delete;

    bool show (SDL_Window *parent_window, const gameSettings & input);

    int getSettings (gameSettings & input);

    int handleEvent (SDL_Event & event);

    bool sameWindow (Uint32 window_id) const
    {
        return ( SDL_GetWindowID (modal_window) == window_id );
    }

    private:

    enum { BTN_OK = 1, BTN_CANCEL, GAME_HEX, GAME_SQUARES, LEVEL_SLIDER, WIDTH_SLIDER, HEIGHT_SLIDER, VOLUME_SLIDER, AUTO_MARKS, QMARKS_BOX, BATCH_BOX, SCALE_BOX, HILITE_BOX };

    static std::string getLevelName (int level);
    ImagePtr getLevelIcon (int level) const;

    static void setlevelLabel (UILabelPtr label, const UISliderPtr slider);
    static void setVolumelabel (UILabelPtr label, const UISliderPtr slider);

    static void setWidthLabel (UILabelPtr label, const UISliderPtr slider);

    static void setHeightLabel (UILabelPtr label, const UISliderPtr slider);

    const int VOLUME_SCALE = 16; 

    SDL_Window * modal_window = nullptr;
    SDL_Window * m_parent = nullptr;
    SDL_Renderer * renderer = nullptr;
    FontPtr font = nullptr;

    // controls:
    UILabelPtr game_label = nullptr;
    UICheckboxPtr hex_box = nullptr, sqr_box = nullptr;

    UIBevelPtr game_bevel = nullptr;
    UILabelPtr level_label_dynamic = nullptr;

    UISliderPtr level_slider = nullptr;
    UIBevelPtr size_bevel = nullptr;
    UILabelPtr width_label = nullptr;
    UISliderPtr width_slider = nullptr;
    UILabelPtr  height_label = nullptr; 

    UISliderPtr height_slider = nullptr;
    UICheckboxPtr auto_marks_box = nullptr;
    UICheckboxPtr qmarks_box = nullptr;

    UICheckboxPtr batch_box = nullptr;
    UIBevelPtr visuals_bevel = nullptr;
    UILabelPtr visuals_label = nullptr;

    UICheckboxPtr scale_box = nullptr;
    UICheckboxPtr hilite_box = nullptr;

    UIBevelPtr  volume_bevel = nullptr;
    UILabelPtr  volume_label = nullptr;

    UISliderPtr volume_slider = nullptr;

    UIButtonPtr ok_button = nullptr, cancel_button = nullptr;

    UIManagerPtr manager = nullptr;

    ImagePtr levelIcon = nullptr;

    ImagePtr beginnerImage = nullptr, intermedImage = nullptr, advancedImage = nullptr;
};
