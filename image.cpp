/*
 *  image.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "image.h"
#include "fileUtils.h"

#if !defined (_MSC_VER)
#include <SDL2/SDL_image.h>
#else 
#include <SDL_image.h>
#endif

#include <cassert>
#include <array>
#include <cstring>
#include <string.h>

bool Image::load (SDL_Renderer *r, const char *filename, bool scale)
{
    const std::array<std::string, 5> paths = { "images/squares", "images/digits", "images/small_images", "images/large_images", "images/faces" }  ;

    std::string file2x = FileUtils::getAt2XPngFile (filename); 

    std::string file = filename;

    std::array<std::string, 2> names = { file2x, file } ;

    if (scale == false) 
        names = { file, file2x};

    for (auto & name : names)
    {
        if (name.empty()) continue;

        for (auto & s : paths)
        {
            std::string buffer = (s + "/" + name);
            std::string full_path = FileUtils::getPrefixedPath (buffer);

            if (false == FileUtils::file_exists (full_path.c_str(), false, false))
                continue;

            mTexture = IMG_LoadTexture (r, full_path.c_str());

            if (mTexture)
            {
                m_scaled = (name == file2x); 
                return true;
            }
        }
    }

    return false;
}

Image::Image (SDL_Renderer *r, const char *filename, bool scale) 
{
    saved_filename = filename;

    this->load (r, filename, scale);

    if (!mTexture)
    {
        SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Cannot load image %s\n", filename);
        assert (0);
    }

    SDL_QueryTexture(mTexture, NULL, NULL, &m_width, &m_height);
}

Image::~Image ()
{
    free();
}

void Image::reload (SDL_Renderer *r, bool scale)
{
    if (scale == m_scaled) return;

    free();

    this->load (r, saved_filename.c_str(), scale);

    if (!mTexture)
    {
        SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Cannot reload image %s\n", saved_filename.c_str());
        assert (0);
    }

    SDL_QueryTexture(mTexture, NULL, NULL, &m_width, &m_height);
}

void Image::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
        m_width = m_height = 0;
	}
}

void Image::draw (SDL_Renderer * renderer, int left, int top, int width, int height, bool flip_vertically) const
{
	SDL_Rect dst_rect = { left, top, width, height }; 

    if (flip_vertically)
        SDL_RenderCopyEx (renderer, mTexture, NULL, &dst_rect, 0, NULL, SDL_FLIP_VERTICAL );
    else 
        SDL_RenderCopy (renderer, mTexture, NULL, &dst_rect);
	
}

void Image::draw (SDL_Renderer * renderer, int left, int top) const
{
    SDL_Rect dst_rect = { left, top, m_width, m_height }; 

    SDL_RenderCopy (renderer, mTexture, NULL, &dst_rect);
}
