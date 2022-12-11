#include "UIBevel.h"

UIBevel::UIBevel (SDL_Renderer * g, int left, int top, int _width, int _height) : UIControl (0, g)
{
	// Initialize

    mLeft = left, mTop = top; 
    mWidth = _width, mHeight = _height;
}

void UIBevel::render(SDL_Rect *clip) const
{
    SDL_Rect rect = getFrameRect();

    UIControl::drawBevel (mRenderer, rect);     
}