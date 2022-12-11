#include "UICheckbox.h"

UICheckbox::UICheckbox (const int CONTROL_ID, const std::string text, TTF_Font *font, 
        SDL_Color & textColor, SDL_Renderer * g, int left, int top, int width) : UIControl (CONTROL_ID, g) ,
        mChecked(false), mPressed(false), max_width (width), label(NULL), isGroup (false)
{
    int xmargin = box_height + 8;

    mLeft = left, mTop = top;

    label = new UILabel (text, font, textColor, g, 0, 0, width - xmargin);

    mWidth = label->getWidth() + xmargin;
    mHeight = (std::max) (label->getHeight() , box_height) ;

    if (mWidth < max_width && max_width > 0) 
        mWidth = max_width;

    int y = (mHeight - label->getHeight()) / 2;

    label->move (mLeft + xmargin, y + mTop);
}

UICheckbox::~UICheckbox()
{
    delete label;
	// Deallocate
}

bool UICheckbox::hitTest (int x, int y) const
{ 
    SDL_Point pt = {x, y};
    SDL_Rect rect = getFrameRect();

    return SDL_PointInRect (&pt, &rect); 
}

void UICheckbox::render (SDL_Rect *clip) const
{
    label->render (clip);

    SDL_Rect renderQuad = { mLeft, mTop, mWidth, mHeight };

    if (mPressed)
    {
        SetRenderDrawColor (mRenderer, pressedColor);
        InflateRect (renderQuad, 4);
        SDL_RenderDrawRect (mRenderer, &renderQuad);
    }
    else if (bHasFocus)
    {
        SetRenderDrawColor (mRenderer, focusedColor);
        InflateRect (renderQuad, 4);
        SDL_RenderDrawRect (mRenderer, &renderQuad);
    }

    SDL_Rect boxRect = { mLeft, mTop + (mHeight - box_height) / 2, 16, 16 };

    if (mPressed)
    {
        SetRenderDrawColor (mRenderer, pressedColor);
    }
    else 
    {
        SDL_SetRenderDrawColor (mRenderer, 0, 0, 0, 255);
    }

    SDL_RenderDrawRect (mRenderer, &boxRect );

    if (mChecked)
    {
        InflateRect (boxRect, -3);
        SDL_RenderFillRect (mRenderer, &boxRect );
    }
}
