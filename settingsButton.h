/*
 *  settingsButton.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#include <memory>
#include "BaseButton.h"
#include "image.h"

class SettingsButton : public BaseButton
{
    public:
    SettingsButton (const SDL_Rect & pos, SDL_Renderer *g);
    ~SettingsButton ();
    void render (SDL_Renderer *g) const;
    bool onLeftMouseUp ( int x, int y, bool & clicked) ;

    void reloadImages (SDL_Renderer *g, bool scale);

    protected:

    void free();
    void loadImages(SDL_Renderer *g);

    private:
    ImagePtr regularImage = nullptr, pressedImage = nullptr;

};

typedef std::shared_ptr<SettingsButton> SettingsButtonPtr;