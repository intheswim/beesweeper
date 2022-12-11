#pragma once

#include <string>
#include "UIControl.h"

#include "UILabel.h"

class UICheckbox : public UIControl
{
    public:
	// Initializes variables
	UICheckbox (const int CONTROL_ID, const std::string text, TTF_Font *font, SDL_Color & textColor, SDL_Renderer * g, int left, int top, int width = -1) ;

	// Deallocates memory
	~UICheckbox ();

    UICheckbox(const UICheckbox &) = delete;

    UICheckbox& operator = (const UICheckbox &) = delete;

    // Renders texture at given point
	void render (SDL_Rect *clip = NULL) const;

    bool acceptsFocus() const { return true; }

    bool hitTest (int x, int y) const;

    bool onMouseDown (int x, int y) { mPressed = true; return false; }

    bool onMouseUp (int x, int y) 
    { 
        if (mPressed) 
        { 
            mPressed = false; 

            if (bHasFocus)
            {   
                if (isGroup && mChecked)
                    return false;

                setChecked (!mChecked);

                return true;
            }
        } 

        return false; 
    }


    int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

    bool hasFocus() const { return bHasFocus; }

    void setFocus (bool v) { bHasFocus = v; }

    void setGroup () { isGroup = true; }

    void setChecked (bool value) { mChecked = value; }
    bool getChecked () const { return mChecked; }

    private:

    static const int box_height = 16;

    // Deallocates texture
	void free();

    bool mChecked, mPressed;

	// Image dimensions
    int max_width;

    UILabel *label;

    bool isGroup; // when group, manual un-checking does not work.

};