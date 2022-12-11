/*
 *  BaceButton.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "BaseButton.h"

bool BaseButton::onLeftMouseDown ( int x, int y)
{
    SDL_Point pt = {x, y};

    if (SDL_PointInRect (&pt, &m_frame))
    {
        pressed = true;
        has_focus = true;
        return true;
    }

    return false;
}


bool BaseButton::onMouseMove (int x, int y)
{
    if (has_focus) 
    {
        SDL_Point pt = {x, y};

        bool inside_frame = false;

        if (SDL_PointInRect (&pt, &m_frame))
        {
            inside_frame = true;
        }

        if (pressed != inside_frame)
        {
            pressed = inside_frame;
            return true;
        }
    }

    return false;
}
