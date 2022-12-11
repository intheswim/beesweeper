/*
 *  BaseButton.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL.h>
#else 
#include <SDL.h>
#endif


class BaseButton
{
    public:
    BaseButton (const SDL_Rect & pos) : m_frame(pos) {};
    virtual ~BaseButton() {};

    virtual void render (SDL_Renderer *g) const = 0;
    virtual bool hasFocus () const { return has_focus; }
    virtual bool onLeftMouseDown ( int x, int y) ;
    virtual bool onLeftMouseUp ( int x, int y, bool & clicked) = 0;
    virtual bool onMouseMove (int x, int y) ;

    protected: 
    virtual void loadImages(SDL_Renderer *g) = 0;
    virtual void free () = 0;

    bool pressed = false;
    bool has_focus = false;
    SDL_Rect m_frame;
};
