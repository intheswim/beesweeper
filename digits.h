/*
 *  digits.h
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

enum DigitColor { DIGIT_GREEN = 0, DIGIT_PINK, DIGIT_BLUE };

// Control displaying 3-digit number
class Digits
{
	public:
		//Initializes variables
		Digits(int left, int top, DigitColor = DIGIT_GREEN, int numDigits = 3);

		//Deallocates memory
		~Digits();

		Digits(const Digits &) = delete;

    	Digits& operator = (const Digits &) = delete;

		//Loads image at specified path
		bool loadImage ( SDL_Renderer *renderer, bool scale );

		void move (int x, int y) { mLeft = x, mTop = y; }

		void setValue (int v) 
		{ 
			mNumber = (std::max) ((std::min)(v, maxValue), minValue); 
		}

		//Renders texture at given point
		void render( SDL_Renderer *renderer) const;

		void reloadImages (SDL_Renderer *g, bool scale);

		//Gets image dimensions
		int getWidth () const { return (13 * mNumDigits); }
		int getHeight () const { return 23; }

		int getLeft () const { return mLeft; }
		int getTop () const { return mTop; }

		bool changeColor (DigitColor toColor, SDL_Renderer * renderer);
	protected:
		void free();

	private:
        
		bool loadFromFile( std::string path, SDL_Renderer *renderer, bool scale);

		static int getPow10 (int power);

		//The actual hardware texture
		SDL_Texture* mTexture = nullptr;

		int maxValue = 999, minValue = -99; // adjust for wider/shorter control

		int mNumDigits = 0;
		//Image dimensions
		int mLeft = 0, mTop = 0;
		int imgWidth = 0;
		int imgHeight = 0;
		int mNumber = 0;
		int mColor = 0;
		bool mScaled = false;
};

typedef std::shared_ptr<Digits> DigitsPtr;
