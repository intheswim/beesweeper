/*
 *  settingsButton.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "settingsButton.h"

SettingsButton::SettingsButton(const SDL_Rect & pos, SDL_Renderer *g) : BaseButton(pos)
{
    loadImages (g);
}

SettingsButton::~SettingsButton()
{
    free();
}

bool SettingsButton::onLeftMouseUp ( int x, int y, bool & clicked) 
{ 
    clicked = false;

    if (has_focus) 
    { 
        pressed = false;
        has_focus = false; 

        SDL_Point pt = {x, y};
        
        if (SDL_PointInRect (&pt, &m_frame))
        {
            clicked = true;
        }

        return true;
    } 
    return false;
}


void SettingsButton::free()
{
    pressedImage = nullptr;
    regularImage = nullptr;
}

void SettingsButton::loadImages(SDL_Renderer *g)
{
    regularImage = std::make_shared<Image> (g, "round_gray13.png");

    pressedImage = std::make_shared<Image> (g, "round_gray13blue.png");
}

void SettingsButton::reloadImages (SDL_Renderer *g, bool scale)
{
    regularImage->reload (g, scale);
	pressedImage->reload (g, scale);
}

void SettingsButton::render (SDL_Renderer *g) const 
{
    ImagePtr img = pressed ? pressedImage : regularImage;

    if (img)
    {
        img->draw (g, m_frame.x, m_frame.y, m_frame.w, m_frame.h);
    }
} 