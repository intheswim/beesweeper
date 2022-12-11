/*
 *  SDLFont.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#if !defined (_MSC_VER)
#include <SDL2/SDL_ttf.h>
#else 
#include <SDL_ttf.h>
#endif

#include <memory>
#include "fileUtils.h"

class SDLFont 
{
    public:
    SDLFont (const char *file_name, int size, bool bold = false) : m_font(nullptr)
    {
        std::string full_path = FileUtils::getPrefixedPath (file_name); 

        m_font = TTF_OpenFont(full_path.c_str(), size);

        if (m_font == nullptr)
        {
            SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Cannot open font: %s", file_name);
        }

        if (m_font && bold)
        {
            TTF_SetFontStyle(m_font, TTF_STYLE_BOLD);
        }
    }
    ~SDLFont ()
    {
        SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "SDL font destructor\n");

        TTF_CloseFont(m_font);
    }

    TTF_Font * getFont() const { return m_font; }

    // forbid coping since we have raw pointer in class.
    SDLFont(const SDLFont &) = delete;
    SDLFont& operator = (const SDLFont &) = delete;

    private:
    TTF_Font * m_font = nullptr;
};

typedef std::shared_ptr<SDLFont> FontPtr;