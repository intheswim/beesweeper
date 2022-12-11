#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL.h>
#else 
#include <SDL.h>
#endif

#include <string>
#include "UIControl.h"

class UIBevel : public UIControl
{
public:
	// Initializes variables
	UIBevel (SDL_Renderer * g, int left, int top, int width, int height) ;

	// Deallocates memory
	~UIBevel() {} ;

    void resize (int w, int h) { mWidth = w, mHeight = h; }

	// Renders texture at given point
	void render(SDL_Rect *clip = NULL) const;

	// Gets image dimensions
	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

    bool hasFocus() const { return false; }

    bool acceptsFocus() const { return false; }

    bool hitTest (int x, int y) const { return false; }
};
