/*
 *  MusicChunk.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#if !defined (_MSC_VER)
#include "SDL2/SDL_mixer.h"
#include <SDL2/SDL.h>
#else 
#include "SDL_mixer.h"
#include <SDL.h>
#endif

#include <memory>
#include "fileUtils.h"

class MusicChunk
{
    public:
    MusicChunk (const char *file)
    {
        std::string full_path  = FileUtils::getPrefixedPath (file);

        mSound = Mix_LoadWAV (full_path.c_str());

	    if (mSound == NULL)
	    {
		    SDL_LogError (SDL_LOG_CATEGORY_AUDIO, "Failed to load sound effect! SDL_mixer Error: %s", Mix_GetError());
	    }
    }

    ~MusicChunk ()
    {
        Mix_FreeChunk(mSound);
    }

    // forbid coping since we have raw pointer in class.
    MusicChunk (const MusicChunk &) = delete;
    MusicChunk & operator = (const MusicChunk &) = delete;

    bool isValid() const { return (mSound != nullptr); }

    void play (int volume) const
    {
        Mix_VolumeChunk (mSound, volume);
				
	    Mix_PlayChannel (-1, mSound, 0);
    }

    private:
    Mix_Chunk *mSound = nullptr;
};

typedef std::unique_ptr<MusicChunk> MusicChunkUPtr;