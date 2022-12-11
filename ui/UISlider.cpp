#include "UISlider.h"

UISlider::UISlider (const int CONTROL_ID, const int low, const int high, SDL_Renderer * g, int left, int top, int width) : UIControl (CONTROL_ID,g), 
    mLow (low), mHigh (high), mPressed(false), draggedByMouse(false), m_dx(0), mRulerStep (0), mPosition(mLow)
{
    mWidth = width;
    mHeight = 24;
    mLeft = left, mTop = top;
    draggedByMouse = false;

    mRulerStep = getRulerStep (high - low);
}

UISlider::~UISlider ()
{
}

void UISlider::showRuler (bool value)
{
    if (mRulerStep > 0 && value == false)
    {
        mRulerStep = -mRulerStep;
    }
    else if (mRulerStep < 0 && value == true)
    {
        mRulerStep = -mRulerStep;
    }
}

int UISlider::getRulerStep (int range) const
{
    for (int i = std::min (16, range); i >= 2; i--)
    {
        if ((range % i) == 0)
        {
            return range / i;
        }
    }

    return 0;
}

void UISlider::setPosition (int pos)
{
    mPosition = pos;

    if (mPosition < mLow) 
        mPosition = mLow;
    
    if (mPosition > mHigh) 
        mPosition = mHigh;
}

bool UISlider::hitTest (int x, int y) const
{ 
    SDL_Point pt = {x, y};
    SDL_Rect rect = getFrameRect();

    return SDL_PointInRect (&pt, &rect); 
}

float UISlider::getCoordinateFromPos (float pos) const
{
    float dx = (mWidth - 2 * handleHalfWidth) * (pos - mLow) / (mHigh - mLow) ;

    return dx + handleHalfWidth;
}

float UISlider::getPosFromCoordinate (int x) const
{
    float pos = mLow + (mHigh - mLow) * (x - (mLeft + handleHalfWidth)) / (float)(mWidth - 2 * handleHalfWidth);

    if (pos < mLow) pos = mLow;
    else if (pos > mHigh) pos = mHigh;

    return pos;
}


SDL_Rect UISlider::getHandleRect () const
{
    float dx = getCoordinateFromPos (mPosition) ;

    SDL_Rect handleRect = { (int)std::round (mLeft + dx - handleHalfWidth), mTop, 2 * handleHalfWidth, mHeight};

    return handleRect;
}

bool UISlider::onMouseDown (int x, int y) 
{ 
    mPressed = true;

    SDL_Point pt = { x, y };

    SDL_Rect handle = getHandleRect();

    if (SDL_PointInRect (&pt, &handle))
    {
        draggedByMouse = true;

        m_dx = x - (handle.x + handleHalfWidth);
    }
    else // check if we should move position. 
    {
        if (x > handle.x + handle.w && mPosition < mHigh)
        {
            mPosition = std::round(getPosFromCoordinate (x));
            return true;
        }
        else if (x < handle.x + handle.w && mPosition > mLow)
        {
            mPosition = std::round(getPosFromCoordinate (x));
            return true;
        }
    }

    return false;
}

bool UISlider::onMouseMove (int x, int y)
{
    if (draggedByMouse && mPressed)
    {
        mPosition = getPosFromCoordinate (x - m_dx);

        return (true);
    }

    return false; 
}

bool UISlider::onMouseUp (int x, int y) 
{ 
    if (mPressed) 
    { 
        mPressed = false; 

        draggedByMouse = false;

        mPosition = std::round (mPosition);

        if (bHasFocus)
        {   
            return true;
        }
    } 

    return false; 
}

void UISlider::render(SDL_Rect *clip) const
{
    // draw slider bar

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

    if (mRulerStep > 0)
    {
        SDL_SetRenderDrawColor (mRenderer, 0, 0, 0, 255);

        // ruler markings

        for (int i = (int)mLow; i <= (int)mHigh; i += mRulerStep)
        {
            float fx = getCoordinateFromPos (i) ;

            int x = std::round (fx + mLeft);
            int y = mTop + 1;

            SDL_RenderDrawLine (mRenderer, x, y, x, y + 5);
        }
    }

    SDL_Rect slider_bar = { mLeft + handleHalfWidth, mTop + mHeight/2 - 2, mWidth - 2 * handleHalfWidth, 4 };

    Frame3D (mRenderer, slider_bar, LOWERED, 1);

    SDL_Rect handleRect = getHandleRect();

    if (mPressed)
    {
        SetRenderDrawColor (mRenderer, pressedColor);
    }
    else
    { 
        SDL_SetRenderDrawColor (mRenderer, 224, 224, 224, 255);
    }

    SDL_RenderFillRect (mRenderer, &handleRect );

    Frame3D (mRenderer, handleRect, RAISED, 1);
}