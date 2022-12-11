#include "UILabel.h"

UILabel::UILabel (const std::string text, TTF_Font *font, const SDL_Color & textColor, SDL_Renderer * g, int left, int top, int width) : UIControl (0, g) ,
        mTexture (NULL), m_text(text), mFont(font), m_textColor(textColor), max_width (width), m_alignment (TEXT_ALIGN_LEFT), m_textWidth (0)
{
	// Initialize

	mWidth = 0;
	mHeight = 0;

    mLeft = left, mTop = top; 

    SetLabelText (text);
}

UILabel::~UILabel()
{
	// Deallocate
	free();
}

void UILabel::free ()
{
    if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void UILabel::setAlignment (int value)
{
	m_alignment = value;
}

int UILabel::getWidth() const
{
	return mWidth;
}

int UILabel::getHeight() const
{
	return mHeight;
}

void UILabel::changeTextColor (const SDL_Color & color)
{
	if (RGBA(color) != RGBA(m_textColor))
	{
		m_textColor = color;
		SetLabelText (m_text);
	}
}

bool UILabel::SetLabelText (const std::string labelText)
{
    // Get rid of preexisting texture
	free();

    if (mFont == NULL)
	{
        return false;
	}

	m_text = labelText;

    SDL_Surface *textSurface = nullptr;

	// Render text surface
	if (max_width <= 0)
    {
        textSurface = TTF_RenderText_Blended (mFont, labelText.c_str(), m_textColor);
    }
    else 
    {
        textSurface = TTF_RenderText_Blended_Wrapped (mFont, labelText.c_str(), m_textColor, max_width);
    }

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		// Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get image dimensions
			m_textWidth = textSurface->w;

			mWidth = std::max(max_width, m_textWidth);

			mHeight = textSurface->h;
		}

		// Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	// Return success
	return mTexture != NULL;
}

void UILabel::render(SDL_Rect *clip) const
{
    int x = mLeft, y = mTop;

	if (m_alignment == TEXT_ALIGN_CENTER)
	{
		if (max_width - m_textWidth > 0)
			x += (max_width - m_textWidth) / 2;
	}
	else if (m_alignment == TEXT_ALIGN_RIGHT)
	{
		if (max_width - m_textWidth > 0)
			x += (max_width - m_textWidth);
	} 
    
    // Set rendering space and render to screen
	SDL_Rect renderQuad = {x, y, m_textWidth, mHeight};

	// Render to screen
	SDL_RenderCopy(mRenderer, mTexture, clip, &renderQuad);
}