#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL_ttf.h>
#else 
#include <SDL_ttf.h>
#endif

#include <string>
#include "UIControl.h"

#include "UILabel.h"

class UIButton : public UIControl
{
    public:
	// Initializes variables
	UIButton (const int CONTROL_ID, const std::string text, TTF_Font *font, SDL_Color & background, SDL_Color & hilitedColor, int cornerRadius, SDL_Renderer * g, int left, int top, int width = -1) ;

	// Deallocates memory
	~UIButton();

    UIButton(const UIButton &) = delete;

    UIButton& operator = (const UIButton &) = delete;

    // Renders texture at given point
	void render(SDL_Rect *clip = NULL) const;

    // Gets image dimensions
	int getWidth() const;
	int getHeight() const;

    bool hasFocus() const { return bHasFocus; }

    void setFocus (bool v)
    { 
        bHasFocus = v; 
    }

    bool acceptsFocus() const { return true; }

    SDL_Rect getFrameRect () const;

    bool hitTest (int x, int y) const;

    bool onMouseDown (int x, int y) { mPressed = true; return false; }

    bool onMouseUp (int x, int y) ;

    private:

    // Deallocates texture
	void free();

    // The actual hardware texture
	// SDL_Texture *mTexture;

   // TTF_Font * mFont;

    int m_cornerRadius;
    SDL_Color m_backgroundColor, m_hilitedColor;

    int max_width;
    bool mPressed;

    UILabel *label, *white_label;
};