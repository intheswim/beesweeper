#include "UIButton.h"
#include "roundRect.h"

UIButton::UIButton (const int CONTROL_ID, const std::string text, TTF_Font *font, SDL_Color & background, 
        SDL_Color & hilitedColor, int cornerRadius, SDL_Renderer * g, int left, int top, int width) : UIControl (CONTROL_ID, g) ,
        m_cornerRadius(cornerRadius), m_backgroundColor (background), m_hilitedColor (hilitedColor), max_width (width), mPressed(false), label(NULL), white_label(NULL)
{
    mWidth = 0;
	mHeight = 0;
    mLeft = left, mTop = top;

    const int xmargins = 8;
    const int ymargins = 6;

    const SDL_Color whiteColor = { 255, 255, 255 };
    const SDL_Color blackColor = { 0, 0, 0 };

    label       = new UILabel (text, font, blackColor, g, 0, 0, width - xmargins * 2);
    white_label = new UILabel (text, font, whiteColor, g, 0, 0, width - xmargins * 2);

    label->setAlignment (TEXT_ALIGN_CENTER);
    white_label->setAlignment (TEXT_ALIGN_CENTER);

    mWidth = label->getWidth() + 2 * xmargins;
    mHeight = label->getHeight() + 2 * ymargins;

    if (mWidth < max_width && max_width > 0) 
        mWidth = max_width;

    int x = (mWidth - label->getWidth()) / 2;
    int y = (mHeight - label->getHeight()) / 2;

    label->move (x + mLeft, y + mTop);
    white_label->move (x + mLeft, y + mTop);
}

UIButton::~UIButton()
{
    delete label;
    delete white_label;
    
	// Deallocate
	free();
}

void UIButton::free ()
{
}

int UIButton::getWidth() const
{
	return mWidth;
}

int UIButton::getHeight() const
{
	return mHeight;
}

SDL_Rect UIButton::getFrameRect () const
{
    SDL_Rect rect = { mLeft, mTop, mWidth, mHeight};

    return rect;
}

bool UIButton::hitTest (int x, int y) const
{ 
    SDL_Point pt = {x, y};
    SDL_Rect rect = getFrameRect();

    return SDL_PointInRect (&pt, &rect); 
}

bool UIButton::onMouseUp (int x, int y) 
{ 
    if (mPressed) 
    { 
        mPressed = false; 

        if (bHasFocus)
            return true;
    } 

    return false; 
}

void UIButton::render(SDL_Rect *clip) const
 {
    // Set rendering space and render to screen
    int x = mLeft, y = mTop;

	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	// Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

    
	// Render to screen

    Uint32 bgColor = RGBA ((bHasFocus || mPressed) ? m_hilitedColor : m_backgroundColor);

    if (m_cornerRadius > 0)
    {
        SDL_FillRoundRect (mRenderer, bgColor, renderQuad, m_cornerRadius);
    }
    else
    { 
        SetRenderDrawColor2 (mRenderer, bgColor);

        SDL_RenderFillRect (mRenderer, &renderQuad);
    }

    bool use_white = isCloserToBlack (bgColor);

    if (use_white)
    {
        white_label->render(clip);
    }
    else 
    {
        label->render (clip);
    }

    if (m_cornerRadius > 0)
    {
        SDL_DrawRoundRect (mRenderer, (bHasFocus ? HalfRGB (bgColor) : bgColor), renderQuad, m_cornerRadius);
    }
    else
    { 
        SetRenderDrawColor2 (mRenderer, HalfRGB (bgColor ));
        SDL_RenderDrawRect (mRenderer, &renderQuad); 
    }     
    
    if (mPressed)
    {
        InflateRect (renderQuad, -1);

        if (m_cornerRadius > 0)
        {
            SDL_DrawRoundRect (mRenderer, (bHasFocus ? HalfRGB (bgColor) : bgColor), renderQuad, m_cornerRadius - 1);
        }
        else
        { 
            SetRenderDrawColor2 (mRenderer, bHasFocus ? HalfRGB (bgColor) : 0);
            SDL_RenderDrawRect (mRenderer, &renderQuad);
        }         
    }  
 }

