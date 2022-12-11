#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL.h>
#else 
#include <SDL.h>
#endif

#include <string>
#include <cmath>
#include "UIControl.h"

class UISlider : public UIControl
{
    public:
	// Initializes variables
	UISlider (const int CONTROL_ID, const int low, const int high, SDL_Renderer * g, int left, int top, int width = -1) ;

	// Deallocates memory
	~UISlider ();

    int getPosition () const { return (int)std::round(mPosition); }

    void setPosition (int pos); 

    void showRuler (bool value);

    // Renders texture at given point
	void render (SDL_Rect *clip = NULL) const;

    bool hitTest (int x, int y) const;

    int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

    bool acceptsFocus() const { return true; }

    bool hasFocus() const { return bHasFocus; }

    bool onMouseDown (int x, int y) ;

    bool onMouseMove (int x, int y) ;

    bool onMouseUp (int x, int y) ;

    private:

    static const int handleHalfWidth = 5;

    SDL_Rect getHandleRect () const;

    float getCoordinateFromPos (float pos) const;
    float getPosFromCoordinate (int x) const;

    int getRulerStep (int range) const;

    float mLow, mHigh;

    bool mPressed, draggedByMouse;

    int m_dx; // holds offset from mouse x pos to handle center when dragging.

    int mRulerStep;

    float mPosition;
};
