/*
 *  digits.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#if !defined (_MSC_VER)
#include <SDL2/SDL_image.h>
#else 
#include <SDL_image.h>
#endif

#include <cassert>
#include "digits.h"
#include "graphics.h"
#include <cmath>
#include "fileUtils.h"

Digits::Digits(int left, int top, DigitColor color, int numDigits) : 
					mTexture(nullptr), mNumDigits (numDigits), mLeft(left), mTop(top), mColor (color)
{
	//Initialize

	assert (numDigits >= 2);

	maxValue =  (getPow10(numDigits) - 1); 
	minValue =  -(getPow10 (numDigits - 1) - 1);
}

Digits::~Digits()
{
	//Deallocate
	free();
}

int Digits::getPow10 (int power)
{
	int ret = 1;

	for (int i=0; i < power; i++)
	{
		ret *= 10; 
	}

	return ret;
}

bool Digits::loadImage ( SDL_Renderer *renderer, bool scale )
{
	switch (mColor)
	{
		case DIGIT_GREEN:
			return loadFromFile ("images/counters/beedig.png", renderer, scale);
		case DIGIT_PINK:
		    return loadFromFile ("images/counters/beedig_pink.png", renderer, scale);
		case DIGIT_BLUE:
		    return loadFromFile ("images/counters/beedig_blu2.png", renderer, scale);
		default:
			return loadFromFile ("images/counters/beedig@2x.png", renderer, scale);
	}
}

bool Digits::loadFromFile( std::string path, SDL_Renderer *renderer, bool scale)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	auto full_name = FileUtils::getPrefixedPath (path);

	if (scale)
	{
		std::string path2x = FileUtils::getAt2XPngFile (full_name.c_str() ); 

		if (FileUtils::file_exists (path2x.c_str(), false, false ))
		{
			full_name = path2x;
			mScaled = true;
		}
	}
	else 
	{
		mScaled = false;
	}


	SDL_Surface* loadedSurface = IMG_Load( full_name.c_str() );
	if( loadedSurface == NULL )
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", full_name.c_str(), IMG_GetError() );
		assert (0);
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newTexture == NULL )
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", full_name.c_str(), SDL_GetError() );
			assert (0);
		}
		else
		{
			//Get image dimensions
			imgWidth = loadedSurface->w;
			imgHeight = loadedSurface->h;

			SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "Loaded image %d x %d\n", imgWidth, imgHeight);
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	// Return success
	mTexture = newTexture;

	return mTexture != NULL;
}

void Digits::free()
{
	// Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		imgWidth = 0;
		imgHeight = 0;
	}
}

void Digits::render(SDL_Renderer *renderer) const
{
	SDL_Rect rect = { mLeft - 1, mTop - 1, 13 * mNumDigits + 1, 23 + 1 };

	Graphics::Frame3D (renderer, rect, Graphics::LOWERED, 1);

	// Set rendering space and render to screen
	int x = mLeft, y = mTop;

	int c = std::abs(mNumber);

	for (int i = 0; i < mNumDigits; i++)
	{
		int dig = c % 10;	
		if (i == (mNumDigits - 1) && c == 0 && mNumber < 0) // show minus sign.
			dig = 10;
		c = c / 10;
		SDL_Rect target = { x + 13 * (mNumDigits - 1 - i), y, 13, 23 };
		SDL_Rect srcRect = { 13 * dig, 0, 13, 23};
		if (imgHeight == 46)
		{
			srcRect.x <<= 1;
			srcRect.y <<= 1;
			srcRect.w <<= 1;
			srcRect.h <<= 1;
		}
		SDL_RenderCopy( renderer, mTexture, &srcRect, &target );
	}
}

bool Digits::changeColor (DigitColor toColor, SDL_Renderer *renderer)
{
	if (toColor != mColor)
	{
		mColor = toColor;
		return loadImage (renderer, mScaled);
	}

	return false;
}

void Digits::reloadImages (SDL_Renderer *g, bool scale)
{
	loadImage (g, scale);
}