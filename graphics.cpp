/*
 *  Graphics.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "graphics.h"

int Graphics::drawLine (SDL_Renderer * renderer, int x1, int y1, int x2, int y2)
{
    return SDL_RenderDrawLine (renderer, x1, y1, x2, y2);
}

///////////////////////////////////////////////////////////////////////////////

void Graphics::Frame3D(SDL_Renderer * r, SDL_Rect rect, bool direction, int width)
{
	int i;
	
	setColor(r, direction ? RGB_WHITE : RGB_GRAY);
		
	int rect_top = rect.y;
	int rect_bottom = rect.y + rect.h + 1;
	int rect_left = rect.x;
	int rect_right = rect.x + rect.w + 1;
	
	for(i = 0; i < width; i++) 
	{		
		drawLine(r, rect_left + i, rect_bottom - i - 2, rect_left + i, rect_top + i);
		drawLine(r, rect_left + i, rect_top + i,rect_right - i - 1, rect_top + i);
    }
	
	setColor(r, direction ? RGB_GRAY : RGB_WHITE);	
	
	for(i = 0; i < width; i++) 
    {		
		drawLine (r, rect_left + i + 1, rect_bottom - i - 1, rect_right - i - 1, rect_bottom - i - 1);
		drawLine (r, rect_right - i - 1, rect_bottom - i - 1, rect_right - i - 1, rect_top + i);
    }
}

///////////////////////////////////////////////////////////////////////////////

void Graphics::InflateRect (SDL_Rect & rect, int dx, int dy)
{
	rect.x -= dx;
	rect.y -= dy;
	rect.w += (dx * 2);
	rect.h += (dy * 2);
}

///////////////////////////////////////////////////////////////////////////////

int Graphics::setColor (SDL_Renderer * renderer, int color, float alpha)
{
	int blue = color & 255;
	int green = (color >> 8) & 255;
	int red = (color >> 16) & 255;

    return SDL_SetRenderDrawColor(renderer, red, green, blue, 255 * alpha);		
}

///////////////////////////////////////////////////////////////////////////////

int Graphics::setFillColor (SDL_Renderer * renderer, int color, float alpha)
{
	int blue = color & 255;
	int green = (color >> 8) & 255;
	int red = (color >> 16) & 255;
	
    return SDL_SetRenderDrawColor (renderer, red, green, blue, 255 * alpha);	
}

///////////////////////////////////////////////////////////////////////////////

int Graphics::setColor (SDL_Renderer * renderer, int color)
{
	return setColor (renderer, color, 1.0);
}

///////////////////////////////////////////////////////////////////////////////

int Graphics::drawRect (SDL_Renderer * renderer, SDL_Rect * rect)
{
    return SDL_RenderDrawRect (renderer, rect);	
}

///////////////////////////////////////////////////////////////////////////////

int Graphics::fillRect (SDL_Renderer * renderer, SDL_Rect * rect)
{
    return SDL_RenderFillRect (renderer, rect);
}

bool Graphics::RectIntersectsRect (SDL_Rect & A, SDL_Rect & B) 
{
	return (SDL_TRUE == SDL_HasIntersection(&A, &B));
}
