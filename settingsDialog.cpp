/*
 *  settingsDialog.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "settingsDialog.h"

SettingsDialog::~SettingsDialog ()
{
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow (modal_window);

    font = nullptr;
    manager = nullptr;
    
    levelIcon = nullptr;

    SDL_FlushEvent(SDL_KEYDOWN);
}


std::string SettingsDialog::getLevelName (int level)
{
    if (level == 0) 
        return "Beginner";
    else if (level == 1) 
        return "Intermediate";
    else if (level == 2) 
        return "Advanced";

    return "Unknown";
}

ImagePtr SettingsDialog::getLevelIcon (int level) const
{
    if (level == 0) 
        return beginnerImage;
    else if (level == 1) 
        return intermedImage;
    else if (level == 2) 
        return advancedImage;

    return nullptr;
}


void SettingsDialog::setlevelLabel (UILabelPtr label, const UISliderPtr slider)
{
     *label = getLevelName (slider->getPosition()).c_str();
}

void SettingsDialog::setVolumelabel (UILabelPtr label, const UISliderPtr slider)
{
    *label = (std::string ("Sound volume: ") + std::to_string (16 * slider->getPosition())).c_str();

    if (slider->getPosition() == 0)
    {
        *label = "Mute";
    }
}

void SettingsDialog::setWidthLabel (UILabelPtr label, const UISliderPtr slider)
{
    *label = (std::string ("Width: ") + std::to_string (slider->getPosition())).c_str();
}

void SettingsDialog::setHeightLabel (UILabelPtr label, const UISliderPtr slider)
{
     *label = (std::string ("Height: ") + std::to_string (slider->getPosition())).c_str();
} 

bool SettingsDialog::show (SDL_Window *parent_window, const gameSettings & input)
{
    m_parent = parent_window;

    // make window hidden until we know its actual required height.
    Uint32 flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS ;

    int width = 360, height = 560 ;

    char title [] = "Beesweeper Settings";

    modal_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags );

    if (modal_window == NULL)
    {
        return false;
    }

    if (0 > SDL_SetWindowModalFor(modal_window, parent_window))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not set modal window. SDL Error: %s", SDL_GetError());
    }

    const Uint32 rend_flags = SDL_RENDERER_SOFTWARE;

	renderer = SDL_CreateRenderer(modal_window, -1, rend_flags);

    if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s", SDL_GetError());
        return false;
	}

    int button_corner_radius = 3;

    if (0 > SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND))
	{
		// caefual: any pixel set with alpha set to zero will be invisible!!
		SDL_LogWarn (SDL_LOG_CATEGORY_APPLICATION, "Could not enable (alpha) blend mode");
        button_corner_radius = 0;
	}

    SDL_Color textColor = { 20, 20, 20 };

    SDL_Color buttonBgColor = { 192, 192, 192 }; 

    SDL_Color greenColor = { 32, 160, 64 };

    SDL_Color blueColor = { 80, 80, 224 };

    SDL_Color cancelColor = blueColor; // { 255, 64, 64 };

    beginnerImage = std::make_shared<Image> (renderer, "beginner.png");
    intermedImage = std::make_shared<Image> (renderer, "intermed.png");
    advancedImage = std::make_shared<Image> (renderer, "expert.png");

    font = std::make_shared<SDLFont> ("assets/arial.ttf", 14);

    game_label = std::make_shared<UILabel> ("Game & Level", font->getFont(), textColor, renderer, 20, 20, width/2 - 40);

    hex_box = std::make_shared<UICheckbox> (GAME_HEX, "Hexagons", font->getFont(), textColor, renderer, 40, 50, (width/2 - 40) - 8) ;

    sqr_box = std::make_shared<UICheckbox> (GAME_SQUARES, "Squares", font->getFont(), textColor, renderer, 40, hex_box->getBottom() + 12, (width/2 - 40) - 8);

    game_bevel = std::make_shared<UIBevel> ( renderer, 8, 8, width- 16, sqr_box->getBottom() + 16);

    level_label_dynamic = std::make_shared<UILabel> ("Beginner", font->getFont(), textColor, renderer, hex_box->getRight() + 16, hex_box->getTop() + 4, width - 16 - hex_box->getRight() - 24);

    level_label_dynamic->setAlignment (TEXT_ALIGN_CENTER);

    level_slider = std::make_shared<UISlider> (LEVEL_SLIDER, 0, 2, renderer,  hex_box->getRight() + 16, sqr_box->getTop() - 2 + 4, width - 16 - hex_box->getRight() - 16 - 8);

    size_bevel = std::make_shared<UIBevel> ( renderer, 8, game_bevel->getBottom() + 16, width - 16, 80);

    width_label = std::make_shared<UILabel> ("Width", font->getFont(), textColor, renderer, 32, size_bevel->getTop() + 12, (width/2 - 32) - 16);

    width_label->setAlignment (TEXT_ALIGN_CENTER);

    width_slider = std::make_shared<UISlider> (WIDTH_SLIDER, MIN_WIDTH, MAX_WIDTH, renderer, 24, width_label->getBottom() + 14, width/2 - 8 - 24 );

    height_label = std::make_shared<UILabel> ("Height", font->getFont(), textColor, renderer, width/2 + 16, width_label->getTop(), (width/2 - 32) - 16);

    height_label->setAlignment (TEXT_ALIGN_CENTER);

    height_slider = std::make_shared<UISlider> (HEIGHT_SLIDER, MIN_HEIGHT, MAX_HEIGHT, renderer, width/2 + 8, height_label->getBottom() + 14, width/2 - 8 - 24 );

    auto_marks_box = std::make_shared<UICheckbox> (AUTO_MARKS, "Auto flags", font->getFont(), textColor, renderer, 20, size_bevel->getBottom() + 16, (width/2 - 30) );

    qmarks_box = std::make_shared<UICheckbox> (QMARKS_BOX, "Question marks", font->getFont(), textColor, renderer, auto_marks_box->getRight() + 12, 
                                    auto_marks_box->getTop(), (width - 20 - auto_marks_box->getRight() - 12) );

    batch_box = std::make_shared<UICheckbox> (BATCH_BOX, "Chord by pressing left and right mouse buttons at the same time", 
                        font->getFont(), textColor, renderer, 20, qmarks_box->getBottom() + 12, (width - 40) );

    visuals_bevel = std::make_shared<UIBevel> ( renderer, 8, batch_box->getBottom() + 16, width - 16, 80);

    visuals_label = std::make_shared<UILabel> ( "Visuals", font->getFont(), textColor, renderer, 20, visuals_bevel->getTop() + 12, 90);

    scale_box = std::make_shared<UICheckbox> (SCALE_BOX, "Scale window (2x)", font->getFont(), textColor, renderer, 120, visuals_bevel->getTop() + 16, (width - 140) );

    hilite_box = std::make_shared<UICheckbox> (HILITE_BOX, "Highlight cells", font->getFont(), textColor, renderer, 120, scale_box->getBottom() + 12, (width -140) );

    visuals_bevel->resize ( width - 16, hilite_box->getBottom() + 16 - visuals_bevel->getTop() );

    volume_bevel = std::make_shared<UIBevel> ( renderer, 8, visuals_bevel->getBottom() + 16, width - 16, 80);

    volume_label = std::make_shared<UILabel> ("Sound volume", font->getFont(), textColor, renderer, 20, volume_bevel->getTop() + 12, -1);

    volume_slider = std::make_shared<UISlider> (VOLUME_SLIDER, 0, 8, renderer, 20, volume_label->getBottom() + 14, width - 40 );

    ok_button = std::make_shared<UIButton> (BTN_CANCEL, "Cancel", font->getFont(), buttonBgColor, cancelColor, button_corner_radius, renderer, 20, volume_bevel->getBottom() + 30, width / 2 - 30);

    cancel_button = std::make_shared<UIButton> (BTN_OK, "OK", font->getFont(), buttonBgColor, greenColor, button_corner_radius, renderer, width/2 + 10, volume_bevel->getBottom() + 30, width /2 - 30);

    SDL_SetWindowSize(modal_window, width, cancel_button->getBottom() + 20);

    // now reposition and then show.
    SDL_SetWindowPosition(modal_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_ShowWindow (modal_window);


    level_slider->showRuler (false);

    level_slider->setPosition (input.level);

    volume_slider->setPosition (input.sound_volume / VOLUME_SCALE );

    volume_slider->showRuler (true);

    hex_box->setGroup();
    sqr_box->setGroup();

    hex_box->setChecked (input.game_hex);
    sqr_box->setChecked (!input.game_hex);

    qmarks_box->setChecked (input.show_qmarks);
    auto_marks_box->setChecked (input.auto_marks);
    hilite_box->setChecked (input.highlight);
    scale_box->setChecked (input.scale);
    width_slider->setPosition (input.width);
    height_slider->setPosition (input.height);
    batch_box->setChecked (input.batch_mode);

    setlevelLabel (level_label_dynamic, level_slider);
    setVolumelabel (volume_label, volume_slider);
    setWidthLabel (width_label, width_slider);
    setHeightLabel (height_label, height_slider);

    levelIcon = getLevelIcon (level_slider->getPosition());

    manager = std::make_unique<UIManager> (modal_window);

    manager->addControl (game_label.get() );

    manager->addControl (level_label_dynamic.get() );

    manager->addControl (ok_button.get() );
    manager->addControl (cancel_button.get() );
    manager->addControl (game_bevel.get() );
    manager->addControl (size_bevel.get() );

    manager->addControl (hex_box.get() );
    manager->addControl (sqr_box.get() );

    manager->addControl (level_slider.get() );

    manager->addControl (width_slider.get() );
    manager->addControl (width_label.get() );

    manager->addControl (height_slider.get() );
    manager->addControl (height_label.get() );

    manager->addControl (auto_marks_box.get() );
    
    manager->addControl (qmarks_box.get() );
    manager->addControl (batch_box.get() );

    manager->addControl (visuals_bevel.get() );

    manager->addControl (scale_box.get() );
    manager->addControl (hilite_box.get() );
    manager->addControl (visuals_label.get() );

    manager->addControl (volume_bevel.get() );

    manager->addControl (volume_slider.get()) ;
    manager->addControl (volume_label.get());

    return true;
}

int SettingsDialog::getSettings (gameSettings & output)
{
    output.game_hex = hex_box->getChecked();
    output.level = level_slider->getPosition();
    output.auto_marks = auto_marks_box->getChecked();
    output.show_qmarks = qmarks_box->getChecked();
    output.sound_volume = volume_slider->getPosition() * VOLUME_SCALE;
    output.highlight = hilite_box->getChecked();
    output.batch_mode = batch_box->getChecked();
    output.scale = scale_box->getChecked();
    output.height = height_slider->getPosition();
    output.width = width_slider->getPosition();

    return 0;
}

// return 0 (CANCEL) or 1 (OK) to close window; return -1 to keep it open.
int SettingsDialog::handleEvent (SDL_Event & event)
{
    int ret = -1;

    if (event.type == SDL_WINDOWEVENT)
    {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            return 0;    
        }
    }
    // User requests quit
    if (event.type == SDL_QUIT)
    {			                
        return 0;
    }
    // escape button also cancels dialog.
    else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
    {
            return 0;
    }

    int clicked = manager->handleEvent (event);

    if (clicked == GAME_HEX)
    {
        sqr_box->setChecked(false);
    }
    else if (clicked == GAME_SQUARES)
    {
        hex_box->setChecked (false);
    }
    else if (clicked == WIDTH_SLIDER)
    {
        setWidthLabel (width_label, width_slider);
    }
    else if (clicked == HEIGHT_SLIDER)
    {
        setHeightLabel (height_label, height_slider);
    }
    else if (clicked == VOLUME_SLIDER)
    {
        setVolumelabel (volume_label, volume_slider);
    }

    else if (clicked == LEVEL_SLIDER)
    {
        setlevelLabel (level_label_dynamic, level_slider);

        levelIcon = getLevelIcon (level_slider->getPosition());
    }

    SDL_SetRenderDrawColor(renderer, 0xEB, 0xEB, 0xEB, 255);

    SDL_RenderFillRect (renderer, NULL);

    manager->renderAll();

    levelIcon->draw (renderer, (level_slider->getLeft() + level_label_dynamic->getRight())/2 - 16, level_label_dynamic->getTop() - 34, 32, 32);

    SDL_RenderPresent(renderer);

    if (clicked == BTN_OK || clicked == BTN_CANCEL)
    {   
        ret = (clicked == BTN_OK) ? 1 : 0;
        return ret;
    }

    return -1;
}
