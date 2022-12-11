/*
 *  faceButton.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL.h>
#else 
#include <SDL.h>
#endif

#include <memory>
#include "BaseButton.h"
#include "image.h"

class FaceButton : public BaseButton 
{
    public:
    FaceButton (const SDL_Rect & pos, SDL_Renderer *g);
    ~FaceButton();

    FaceButton(const FaceButton &) = delete;

    FaceButton& operator = (const FaceButton &) = delete;

    void setState (int state) { face = state; }
    void render (SDL_Renderer *g) const;
    bool hasFocus () const { return has_focus; }

    bool onLeftMouseUp ( int x, int y, bool & clicked) ;

    void reloadImages (SDL_Renderer *g, bool scale);

    private:
    void loadImages(SDL_Renderer *g);
    void free ();

    ImagePtr getFaceImage (int value, bool pressed) const ;

    ImagePtr face_smile = nullptr, face_sad = nullptr, 
            face_boss = nullptr, face_shout = nullptr;

    int face = 0;
};

typedef std::shared_ptr<FaceButton> FaceButtonPtr;