/*
 *  Graphics.h
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


class Graphics
{
    public:

	static const bool RAISED = true;
    static const bool LOWERED = false;

	static const int RGB_WHITE = 0xFFFFFF, RGB_GRAY = 0x707070;  

    static int  drawLine (SDL_Renderer * renderer, int from_x, int from_y, int to_x, int to_y);
	static void Frame3D (SDL_Renderer * renderer, SDL_Rect rect, bool direction, int Width);
	
	static void InflateRect (SDL_Rect & rect, int dx, int dy);
	static int  setColor (SDL_Renderer * context, int color, float falpha);

	static int  setFillColor (SDL_Renderer * context, int color, float falpha);

	static int  setColor (SDL_Renderer * context, int color);
	
	static int  drawRect (SDL_Renderer * context, SDL_Rect * rect);
	static int  fillRect (SDL_Renderer * context, SDL_Rect * rect);

	static bool RectIntersectsRect (SDL_Rect & A, SDL_Rect & B);

};