/*
 *  faceButton.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "faceButton.h"
#include "graphics.h"

#include "GameClasses.h"

FaceButton::FaceButton (const SDL_Rect & pos, SDL_Renderer *g) : BaseButton(pos)
{    
    face = fsSmile;

    loadImages (g);
}

FaceButton::~FaceButton()
{
    free ();
}

bool FaceButton::onLeftMouseUp ( int x, int y, bool & clicked) 
{ 
    clicked = false;

    if (has_focus) 
    { 
        pressed = false;
        has_focus = false; 

        SDL_Point pt = {x, y};
        
        if (SDL_PointInRect (&pt, &m_frame))
        {
            clicked = true;
        }

        return true;
    } 
    return false;
}

ImagePtr FaceButton::getFaceImage (int value, bool pressed) const 
{
    if (pressed)
        return face_smile;
        
    switch ( value)
    {
        case fsSmile:
            return face_smile;

        case fsBoss:
            return face_boss;

        case fsShout:
            return face_shout;

        case fsWeep:
            return face_sad;
        
        default:
            return nullptr;
    }
}
    
void FaceButton::render (SDL_Renderer *g) const
{
    Graphics::Frame3D (g, m_frame, !pressed, pressed ? 1 : 2);   
    ImagePtr img = this->getFaceImage (face, pressed);

    if (img)
    {
        int shift = pressed ? 1 : 0;
        img->draw (g, shift + m_frame.x + (m_frame.w - 16)/2, m_frame.y + (m_frame.h - 16)/2, 17, 17);
    }
}

void FaceButton::free ()
{
    face_boss = nullptr;
    face_sad = nullptr;
    face_shout = nullptr;
    face_smile = nullptr;
}

void FaceButton::loadImages (SDL_Renderer *g)
{
   face_smile = std::make_shared <Image> (g, "face_smile16.png");
   face_sad   = std::make_shared <Image> (g, "face_sad16.png");
   face_boss  = std::make_shared <Image> (g, "face_boss16.png");
   face_shout = std::make_shared <Image> (g, "face_shout16.png");
}

void FaceButton::reloadImages (SDL_Renderer *g, bool scale)
{
    face_smile->reload (g, scale);
    face_sad->reload (g, scale);
    face_boss->reload (g, scale);
    face_shout->reload(g, scale);
}
