#pragma once

#include <string>

#if !defined (_MSC_VER)
#include <SDL2/SDL.h>
#else 
#include <SDL.h>
#endif


class UIControl
{
    public:

    static const bool RAISED = true;
    static const bool LOWERED = false;

    static const int RGB_WHITE = 0xFFFFFF, RGB_GRAY = 0x707070;  

    static const SDL_Color pressedColor ;
    static const SDL_Color focusedColor ;
    
    UIControl (const int ID, SDL_Renderer *g) : control_id (ID), bHasFocus (false), mRenderer(g), mLeft(0), mTop(0), mWidth(0), mHeight(0) { };

    virtual ~UIControl() {} ;

    UIControl(const UIControl &) = delete;

    UIControl& operator = (const UIControl &) = delete;

    virtual bool hasFocus() const = 0;
    virtual bool acceptsFocus () const = 0;
    virtual bool hitTest (int x, int y) const = 0;

    virtual SDL_Rect getFrameRect () const;

    virtual void setFocus (bool value) { bHasFocus = value; }

    virtual bool onMouseDown (int x, int y) { return false; }
    virtual bool onMouseUp (int x, int y) { return false; }

    virtual bool onMouseMove (int x, int y) { return false; }

    virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;

    int getBottom () const { return mTop + mHeight; }
    int getRight () const { return mLeft + mWidth; }
    int getTop() const { return mTop; };
    int getLeft () const { return mLeft; }

    virtual void render(SDL_Rect *clip = NULL) const = 0;

    int controlID () const { return control_id; }

    protected:
    int control_id;
    bool bHasFocus;

    SDL_Renderer *mRenderer;
    int mLeft, mTop, mWidth, mHeight;

    static void InflateRect (SDL_Rect & rect, int delta)
    {
        rect.x -= delta, rect.y -= delta;
        rect.w += (delta + delta), rect.h += (delta + delta);
    }

    static void Frame3D(SDL_Renderer * r, const SDL_Rect & rect, bool direction, int width);
    static void drawLine (SDL_Renderer * renderer, int x1, int y1, int x2, int y2);
    static void drawBevel (SDL_Renderer * r, SDL_Rect & rect); 

    static void setColor (SDL_Renderer * renderer, int color, float alpha = 1.0);
    static int SetRenderDrawColor2 (SDL_Renderer *renderer, const Uint32 & color);

    static int SetRenderDrawColor (SDL_Renderer *renderer, const SDL_Color & color);

    static float ColorLuminance (Uint32 & c);
    static bool isCloserToBlack (Uint32 & c);
    static Uint32 RGBA (int r, int g, int b, int a);
    static Uint32 RGBA (const SDL_Color &);
    static Uint32 HalfRGB (const Uint32 & c);
};