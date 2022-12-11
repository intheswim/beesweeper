/*
 *  image.h
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

#include <string>
#include <memory>

class Image
{
    public:
    Image (SDL_Renderer *r, const char *filename, bool scale = true);
    ~Image();

    // reload image when scale changes.
    void reload (SDL_Renderer *r, bool scale);

    Image(const Image &) = delete;

    Image& operator = (const Image &) = delete;

    // draws image inside given rectangle.
    void draw (SDL_Renderer * renderer, int left, int top, int width, int height, bool flip_vertically = false) const;

    // draws image using its own dimensions at given point.
    void draw (SDL_Renderer * renderer, int left, int top) const;

    private:
    SDL_Texture *mTexture = nullptr;
    int m_width = 0, m_height = 0;
    bool m_scaled = false ;         // true when using @2x version of image (similar from iOS). When renderer is scaled we want to use this higher accuracy image version.
    std::string saved_filename = {};

    protected:
    bool load (SDL_Renderer *, const char *filename, bool scale);
    void free();
};

typedef std::shared_ptr<Image> ImagePtr;