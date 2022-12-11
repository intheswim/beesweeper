#include "roundRect.h"
#include <cmath>

static void imagesetpixel (SDL_Renderer *renderer, float x, float y, int color_with_alpha)
{
    int r = (color_with_alpha >> 16) & 0xFF, 
        g = (color_with_alpha >> 8) & 0xFF, 
        b = (color_with_alpha & 0xFF);

    int alpha = (color_with_alpha >> 24) & 0xFF;

    SDL_SetRenderDrawColor (renderer, r, g, b, 255 - alpha );
    SDL_RenderDrawPoint (renderer, x, y);
}

static void imagesolidline (SDL_Renderer *renderer, float x1, float y1, float x2, float y2, int color_with_alpha)
{
    int r = (color_with_alpha >> 16) & 0xFF, 
        g = (color_with_alpha >> 8) & 0xFF, 
        b = (color_with_alpha & 0xFF);

    SDL_SetRenderDrawColor (renderer, r, g, b, 255 );
    SDL_RenderDrawLine (renderer, x1, y1, x2, y2);
}

static void imagesolidrect (SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int color_with_alpha)
{
    int r = (color_with_alpha >> 16) & 0xFF, 
        g = (color_with_alpha >> 8) & 0xFF, 
        b = (color_with_alpha & 0xFF);

    SDL_SetRenderDrawColor (renderer, r, g, b, 255 );

    SDL_Rect rect = { x1, y1, x2 - x1, y2 - y1 };
    SDL_RenderFillRect (renderer, &rect);
}

// helper function, draws pixel and mirrors it
static void setpixel4 (SDL_Renderer * img, float centerX, float centerY, float deltaX, float deltaY, int color_with_alpha, bool connect = false)
{
    imagesetpixel(img, centerX + deltaX, centerY + deltaY, color_with_alpha);
    imagesetpixel(img, centerX - deltaX, centerY + deltaY, color_with_alpha);

    if (connect)
    {
        imagesolidline (img, centerX - deltaX, centerY + deltaY, centerX + deltaX, centerY + deltaY, color_with_alpha);
    }

    imagesetpixel(img, centerX + deltaX, centerY - deltaY, color_with_alpha);
    imagesetpixel(img, centerX - deltaX, centerY - deltaY, color_with_alpha);

    if (connect)
    {
        imagesolidline (img, centerX - deltaX, centerY - deltaY, centerX + deltaX, centerY - deltaY, color_with_alpha);
    }
}

static void SDL_DrawEllipseInternal (SDL_Renderer *img, Uint32 color, SDL_Rect & rect, bool fill )
{
    int x1 = rect.x, y1 = rect.y;
    int x2 = rect.w + x1, y2 = rect.h + y1;

    float width = x2 - x1, height = y2 - y1;
    float centerX = width / 2 + x1; 
    float radiusX = (width - 2) / 2;
    float centerY = height / 2 + y1; 
    float radiusY = (height - 2) / 2;
    const int maxTransparency = 0xEF; 

    const float radiusX2 = radiusX * radiusX;
    const float radiusY2 = radiusY * radiusY;

    // upper and lower halves
    float quarter = round(radiusX2 / std::sqrt(radiusX2 + radiusY2));
    for (float x = 0; x <= quarter; x++)
    {
        float y = radiusY * std::sqrt(1 - x*x/radiusX2);
        float error = y - std::floor(y);
        int transparency = std::round(error * maxTransparency);
        int alpha  = color | (transparency << 24);
        int alpha2 = color | ((maxTransparency - transparency) << 24);
        setpixel4(img, centerX, centerY, x, std::floor(y), alpha, fill);
        setpixel4(img, centerX, centerY, x, std::floor(y) + 1, alpha2);


    }
    // right and left halves
    quarter = std::round(radiusY2 / std::sqrt(radiusX2 + radiusY2));
    for (float y = 0; y <= quarter; y++)
    {
        float x = radiusX * std::sqrt(1-y*y/radiusY2);
        float error = x - std::floor(x);
        int transparency = std::round(error * maxTransparency);
        int alpha  = color | (transparency << 24);
        int alpha2 = color | ((maxTransparency - transparency) << 24);
        setpixel4(img, centerX, centerY, std::floor(x),   y, alpha, fill);
        setpixel4(img, centerX, centerY, std::floor(x)+1, y, alpha2);
    }
}

void SDL_DrawEllipse (SDL_Renderer *renderer, Uint32 color, SDL_Rect & rect)
{
    SDL_DrawEllipseInternal (renderer, color, rect, false);
}

void SDL_FillEllipse (SDL_Renderer *renderer, Uint32 color, SDL_Rect & rect)
{
    SDL_DrawEllipseInternal (renderer, color, rect, true);
}


////////////////////////////////////////////////////////////////////////////////////////////////

static void SDL_DrawRoundRectInternal (SDL_Renderer *img, Uint32 color, const SDL_Rect & rect, int radius, bool fill)
{
    int x1 = rect.x, y1 = rect.y;
    int x2 = rect.w + x1 - 1, y2 = rect.h + y1 - 1;

    float radius2 = radius * radius;
    const int maxTransparency = 0xFF; 

    float width = x2 - x1, height = y2 - y1;
    float centerX = width / 2 + x1; 
    float centerY = height / 2 + y1;

    float extraX = width / 2 - radius;
    float extraY = height / 2 - radius; 

    // upper and lower halves
    float quarter = round(radius2 / std::sqrt(radius2 + radius2));
    for (float x = 0; x <= quarter; x++)
    {
        float y = radius * std::sqrt(1 - x*x/radius2);
        float error = y - std::floor(y);
        int transparency = std::round(error * maxTransparency);
        int alpha  = color | (transparency << 24);
        int alpha2 = color | ((maxTransparency - transparency) << 24);
        setpixel4(img, centerX, centerY, extraX + x, extraY + std::floor(y), alpha, fill);
        setpixel4(img, centerX, centerY, extraX + x, extraY + std::floor(y) + 1, alpha2);
    }

    // right and left halves
    quarter = std::round(radius2 / std::sqrt(radius2 + radius2));
    for (float y = 0; y <= quarter; y++)
    {
        float x = radius * std::sqrt(1-y*y/radius2);
        float error = x - std::floor(x);
        int transparency = std::round(error * maxTransparency);
        int alpha  = color | (transparency << 24);
        int alpha2 = color | ((maxTransparency - transparency) << 24);
        setpixel4(img, centerX, centerY, extraX + std::floor(x),   extraY + y, alpha, fill);
        setpixel4(img, centerX, centerY, extraX + std::floor(x)+1, extraY + y, alpha2);
    }

    imagesolidline (img, centerX - extraX, y1, centerX + extraX, y1, color);
    imagesolidline (img, centerX - extraX, y2, centerX + extraX, y2, color);

    imagesolidline (img, x1, centerY - extraY, x1, centerY + extraY, color);
    imagesolidline (img, x2, centerY - extraY, x2, centerY + extraY, color);

    if (fill)
    {
        imagesolidrect (img, x1, y1 + radius, x2 + 1, y2 - radius, color);
    }
}

void SDL_DrawRoundRect (SDL_Renderer *renderer, Uint32 color, const SDL_Rect & rect, int radius)
{
    SDL_DrawRoundRectInternal (renderer, color, rect, radius, false);
}
void SDL_FillRoundRect (SDL_Renderer *renderer, Uint32 color, const SDL_Rect & rect, int radius)
{
    SDL_DrawRoundRectInternal (renderer, color, rect, radius, true);
}