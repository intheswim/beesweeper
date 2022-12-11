#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL.h>
#else 
#include <SDL.h>
#endif


void SDL_DrawEllipse (SDL_Renderer *renderer, Uint32 color, SDL_Rect & rect);

void SDL_FillEllipse (SDL_Renderer *renderer, Uint32 color, SDL_Rect & rect);

void SDL_DrawRoundRect (SDL_Renderer *renderer, Uint32 color, const SDL_Rect & rect, int radius);
void SDL_FillRoundRect (SDL_Renderer *renderer, Uint32 color, const SDL_Rect & rect, int radius);