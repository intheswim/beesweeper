#include "UIControl.h"

const SDL_Color UIControl::pressedColor = { (0xF0 * 4/5), (0x77 * 4/5), (0x46 * 4/5), 255 };

const SDL_Color UIControl::focusedColor = { 128, 128, 128, 255 };

SDL_Rect UIControl::getFrameRect () const
{
    SDL_Rect rect = { mLeft, mTop, mWidth, mHeight};

    return rect;
}

void UIControl::setColor (SDL_Renderer * renderer, int color, float alpha)
{
	int blue = color & 255;
	int green = (color >> 8) & 255;
	int red = (color >> 16) & 255;

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255 * alpha);		
}

void UIControl::drawLine (SDL_Renderer * renderer, int x1, int y1, int x2, int y2)
{
    SDL_RenderDrawLine (renderer, x1, y1, x2, y2);
}

///////////////////////////////////////////////////////////////////////////////

void UIControl::Frame3D(SDL_Renderer * r, const SDL_Rect & rect, bool direction, int width)
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


void UIControl::drawBevel (SDL_Renderer * r, SDL_Rect & rect)
{
    Frame3D (r, rect, LOWERED, 1);
    InflateRect (rect, -1);
    Frame3D (r, rect, RAISED, 1);
}

int UIControl::SetRenderDrawColor2 (SDL_Renderer *renderer, const Uint32 & color)
{
	int r = (color >> 16) & 0xFF;
	int g = (color >> 8) & 0xFF;
	int b = (color & 0xFF);

	return SDL_SetRenderDrawColor (renderer, r, g, b, 255);
}

int UIControl::SetRenderDrawColor (SDL_Renderer *renderer, const SDL_Color & color)
{
	int r = color.r;
	int g = color.g;
	int b = color.b;

	return SDL_SetRenderDrawColor (renderer, r, g, b, 255);
}

float UIControl::ColorLuminance (Uint32 & c)
{
	int r = (c >> 16) & 0xFF;
	int g = (c >> 8) & 0xFF;
	int b = (c & 0xFF);

	return (0.2126 * r + 0.7152 * g + 0.0722 * b);
}

bool UIControl::isCloserToBlack (Uint32 & c)
{
	float lum = ColorLuminance (c);
	return (lum < 128); 
}

Uint32 UIControl::RGBA (int r, int g, int b, int a)
{
	int ret = (r << 16) | (g << 8) | b | (a << 24);
	return ret;
}

Uint32 UIControl::RGBA (const SDL_Color & c)
{
	Uint32 ret = (c.a << 24) | (c.r << 16) | (c.g << 8) | (c.b);

	return ret;
}

Uint32 UIControl::HalfRGB (const Uint32 & color)
{
	int r = (color >> 16) & 0xFF;
	int g = (color >> 8) & 0xFF;
	int b = (color & 0xFF);
	int a = (color >> 24) & 0xFF;

	return RGBA (r >> 1, g >> 1, b >> 1, a);
}