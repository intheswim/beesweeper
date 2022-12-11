#pragma once

#include "UIControl.h"
#include <vector>

class UIManager
{
    public:
    UIManager (SDL_Window *w) : mWindowID (0), list()
    { 
        mWindowID = SDL_GetWindowID (w);
    }

    ~UIManager () { list.clear(); }

    void addControl (UIControl *c)
    {
        list.push_back (c);
    }

    int handleEvent (SDL_Event & event);

    void renderAll ();

    private:
    Uint32 mWindowID;
    std::vector<UIControl *> list;
};