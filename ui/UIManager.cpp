#include "UIManager.h"

int UIManager::handleEvent (SDL_Event & event)
{
    if (event.window.windowID != mWindowID)
        return 0;

    if (event.window.event == SDL_WINDOWEVENT_LEAVE && event.type == SDL_WINDOWEVENT)
    {
        for (auto & c : list)
        {
            int x = event.button.x;
            int y = event.button.y;
            c->onMouseUp(x, y);
        }

        return 0;
    }

    if (event.type != SDL_MOUSEMOTION && event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_MOUSEBUTTONUP) 
        return 0;

    int retID = 0;

    for (auto & c : list)
    {
        if (event.type == SDL_MOUSEMOTION)
        {
            int x = event.button.x;
            int y = event.button.y;

            if (c->acceptsFocus() && c->hitTest(x, y) && !c->hasFocus() )
            {
                c->setFocus(true);
            }

            if (c->hasFocus() && c->hitTest(x, y) == false)
            {
                c->setFocus (false);
            }

            if (c->hitTest(x, y) && c->onMouseMove (x, y))
            {
                retID = c->controlID();
            }

        }

        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                int x = event.button.x;
                int y = event.button.y;

                if (c->hasFocus() && c->hitTest (x, y))
                {
                    if (c->onMouseDown(x, y))
                    {
                        return c->controlID();
                    }
                }
            }
        }

        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            int x, y;
            x = event.button.x;
            y = event.button.y; 

            if (event.button.button == SDL_BUTTON_LEFT)
            {
            
                if (c->onMouseUp(x, y))
                {
                    return c->controlID();
                }
                
            }
        }
    }

    return retID;
}

void UIManager::renderAll ()
{
    for (auto & c : list)
    {
        c->render();
    }
}