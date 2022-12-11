#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL_ttf.h>
#else 
#include <SDL_ttf.h>
#endif

#include <string>
#include "UIControl.h"

enum { TEXT_ALIGN_LEFT = 0, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT };

class UILabel : public UIControl
{
public:
	// Initializes variables
	UILabel (const std::string text, TTF_Font *font, const SDL_Color & textColor, SDL_Renderer * g, int left, int top, int width = -1) ;

	// Deallocates memory
	~UILabel();

    UILabel(const UILabel &) = delete;

    UILabel& operator = (const UILabel &) = delete;

    void move (int x, int y) { mLeft = x, mTop = y; }

    void changeTextColor (const SDL_Color & color);

	// Renders texture at given point
	void render(SDL_Rect *clip = NULL) const;

    void setAlignment (int value);

	// Gets image dimensions
	int getWidth() const;
	int getHeight() const;

    bool hasFocus() const { return false; }

    bool acceptsFocus() const { return false; }

    bool hitTest (int x, int y) const { return false; }

    UILabel& operator=(const char* text)
    {
        if (m_text.compare(text) != 0)
        {
            SetLabelText (text);
        }

        return *this;
    }

    UILabel& operator=(const std::string & text)
    {
        if (text != m_text)
        {
            SetLabelText (text);
        }

        return *this;
    }

private:

    bool SetLabelText (const std::string labelText);

    // Deallocates texture
	void free();

	// The actual hardware texture
	SDL_Texture *mTexture;

    std::string m_text;

    TTF_Font * mFont;

	// Image dimensions
    SDL_Color m_textColor;

    int max_width;
    int m_alignment;
    int m_textWidth ;
};
