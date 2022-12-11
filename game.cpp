/*
 *  game.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "game.h"

#if !defined (_MSC_VER)
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#else 
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif

#include "SquareGameField.h"
#include "HexGameField.h"

#include "settings.h"

Game::Game() 
{
	m_settings = { 0 };
}

Game::~Game() 
{
    clean();

    Mix_CloseAudio();

	TTF_Quit();

    IMG_Quit();
	SDL_Quit();
}

void Game::clean()
{
	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "Clean enter");

    // Destroy window
	SDL_DestroyRenderer(renderer);
		
	SDL_DestroyWindow(window);

	window = NULL;
	renderer = NULL;

	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "Delete game_field");

	delete game_field;
	game_field = NULL;

    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "Delete digits");

	secondsView = NULL;

	minesView = NULL;

	faceButton = NULL;

	settingsBtn = NULL;

	timer = nullptr;

	gErrorSound = nullptr;

	gVictorySound = nullptr;

	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "clean exit");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool Game::loadMedia()
{
	bool ret = true;

	gErrorSound = std::make_unique<MusicChunk> ("sounds/mid_confirm.wav");

	if (gErrorSound->isValid() == false)
	{
		ret = false;
	}

	gVictorySound = std::make_unique<MusicChunk> ("sounds/gps_success.wav");

	if (gVictorySound->isValid() == false)
	{
		ret = false;
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Game::getRequiredScreenDimensions (bool hex_game, int cell_cols, int cell_rows, int & req_width, int & req_height)
{
	const int mDigitsHeaderHeight = GenericField::mHeaderMargin;
    const int mBevelWidth = GenericField::mBevelWidth;

    int ScreenWidth = 0, ScreenHeight = 0;

    if (hex_game)
    {
		if ((cell_cols & 1) == 0) // even number of cell columns. Make it odd (it must be odd for symmetrical layout).
			cell_cols++;

        ScreenWidth = 16 * cell_cols + (mBevelWidth * 2) + 7; 
        ScreenHeight = 18 * cell_rows + (mBevelWidth + 6) + mDigitsHeaderHeight;
    }
	else 
	{
		ScreenWidth = 16 * cell_cols + (mBevelWidth * 2) + 2;
    	ScreenHeight = 16 *  cell_rows + (mBevelWidth + 2) + mDigitsHeaderHeight;
	}

	req_width = ScreenWidth;
	req_height = ScreenHeight;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool Game::init (gameSettings & settings)
{
	m_settings = settings;

	return init (m_settings.game_hex, settings.scale, settings.width, settings.height, settings.highlight);
}

bool Game::init (bool hex_game, bool scale, int cell_cols, int cell_rows, bool highlight)
{
    int ScreenWidth = 0, ScreenHeight = 0;

	getRequiredScreenDimensions (hex_game, cell_cols, cell_rows, ScreenWidth, ScreenHeight);

	return init (hex_game, scale, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, highlight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// here width and height is in pixels.

bool Game::init (bool hex_game, bool scale, int xpos, int ypos, int width, int height, bool highlight_cells)
{
    bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL Error: %s", SDL_GetError());
		success = false;
	}
	else
	{
		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		{
			SDL_LogWarn (SDL_LOG_CATEGORY_APPLICATION, "Linear texture filtering not enabled!");
		}

		// Create window
        Uint32 flags = SDL_WINDOW_SHOWN;

		const std::string title = getWindowTitle (hex_game);

		window = SDL_CreateWindow(title.c_str(), xpos, ypos, scaledValue (width, scale), scaledValue (height, scale), flags );

		if (window == NULL)
		{
			SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL Error: %s", SDL_GetError());
			success = false;
		}
		else
		{
			// Create renderer for window
#if defined(_MSC_VER)
			const Uint32 flags = SDL_RENDERER_ACCELERATED;
#else 
			const Uint32 flags = SDL_RENDERER_SOFTWARE; 
#endif 
			renderer = SDL_CreateRenderer(window, -1, flags);
			if (renderer == NULL)
			{
				SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
					success = false;
				}

				// Load sound effects

				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
					success = false;
				}

				// Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
					success = false;
				}

				if (success)
    			{
	    			if (0 > SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND))
					{
						SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Could not enable (alpha) blend mode");
					}
				}
			}
		}
	}

    m_isRunning = success;

    if (success)
    {
		m_userEventID = SDL_RegisterEvents(1); // user event has to be registerred (I guess to avoid some conflicts).

		setupLayout (hex_game, width, height, highlight_cells, scale);

		success = loadMedia();
    }

	return success;
}

void Game::setupLayout (bool hex_game, int width, int height, bool highlight_cells, bool scale)
{
	if (0 > SDL_RenderSetLogicalSize (renderer, width, height))
	{
		SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Could not set logical size");
	}

	if (hex_game)
	{
		game_field = new HexGameField (width, height, m_userEventID, renderer, highlight_cells);	
	}
	else 
	{
		game_field = new SquareGameField (width, height, m_userEventID, renderer, highlight_cells);
	}

	int xpos = GenericField::mBevelWidth + 6; 

	// should be 17
	int ypos = GenericField::mBevelFlat + GenericField::mBevelEdge + (GenericField::mPanelHeight - 23) / 2 + 1;

	secondsView = std::make_shared <Digits> (0, ypos, DIGIT_GREEN, 3);
	secondsView->loadImage (renderer, scale);

	int xpos2 = width - xpos - secondsView->getWidth (); 
	secondsView->move (xpos2, ypos);

	minesView = std::make_shared <Digits> (xpos, ypos, DIGIT_GREEN, 3);
	minesView->loadImage (renderer, scale);

	SDL_Rect face_pos =  {width / 2 - 12, ypos - 1, 24, 24};

	faceButton = std::make_shared <FaceButton> ( face_pos, renderer );

	SDL_Rect settings_pos =  { secondsView->getLeft() - 20, secondsView->getTop() + 10, 13, 13 };

	settingsBtn = std::make_shared <SettingsButton> ( settings_pos, renderer); 

	// in case default scale is false, we need to reload.
	this->reloadImages (renderer, scale);
}

bool Game::startTimer() 
{
	if (timer == nullptr)
	{
		timer = std::make_unique<SDLTimer> (1000, m_userEventID, TYPE_TIMER);

		if (false == timer->isValid ())
			return false;
	}

	return true;
}

void Game::timerFiredMethod () 
{
    //remove a second from the display
	if (!game_field->isTimerEnabled() ) 
		return ;

	int value = (int)std::round(timer->msSinceStart() / 1000.0);
		
	secondsView->setValue (value);
	game_field->setTimerSeconds (value);
}

void Game::stopTimer() 
{
	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "Stop timer");

	timer = nullptr;
}

void Game::handleEvents()
{
    SDL_Event event;

    if (SDL_WaitEvent (&event) != 0 )
	{
		if (settingsDialog != nullptr && settingsDialog->sameWindow (event.window.windowID))
		{
			int closed = 0;
			closed = settingsDialog->handleEvent (event);

			if (closed >= 0)
			{
				gameSettings gs;
				settingsDialog->getSettings (gs);
				delete settingsDialog;
				settingsDialog = nullptr;

				if (closed > 0)
				{
					gs.save ();
					applyNewSettings (gs);
					m_redraw = true;
				}
			}
			return;
		}

		// User requests quit
		if (event.type == SDL_QUIT)
		{
			m_isRunning = false;
			m_redraw = false;
			return ;
		}
		else if (event.type == m_userEventID)
		{
			if (event.user.code == TYPE_TIMER)
			{
				timerFiredMethod();
				
				m_redraw = true;

				return ;
			}
			else if (event.user.code == TYPE_SETTINGS)
			{
				struct gameSettings & gs = m_settings;

				if (settingsDialog == nullptr)
				{
					settingsDialog = new SettingsDialog ();

					if (!settingsDialog->show (getWindow(), gs))
					{
						delete settingsDialog;
						settingsDialog = nullptr;
						// show error.
					}
        		
        		}
			}
			else if (event.user.code == TYPE_MINES_CHANGED)
			{
				minesView->setValue (game_field->getMinesCount()); 
			}
			else if (event.user.code == TYPE_SOUND_BLAST)
			{				
				gErrorSound->play (m_settings.sound_volume);
			}
			else if (event.user.code == TYPE_SOUND_VICTORY)
			{
				gVictorySound->play (m_settings.sound_volume);
			}
			else if (event.user.code == TYPE_FACE_CHANGED)
			{
				faceButton->setState (game_field->getFaceState());

				m_redraw = true;

				return ;
			}
		}
        else if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_EXPOSED)
			{
				m_redraw = true;
                return ;
			}
        }
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				int tx, ty;
				Uint32 buttons = SDL_GetMouseState(&tx, &ty);

				bool rightPressed = ((buttons & SDL_BUTTON_RMASK) != 0);

				onLeftMouseDown (rightPressed, event.button.x, event.button.y);

				m_redraw = true;

				// used to return
			}
			else if (event.button.button == SDL_BUTTON_RIGHT)
			{
				int tx, ty;
				Uint32 buttons = SDL_GetMouseState(&tx, &ty);

				bool leftPressed = ((buttons & SDL_BUTTON_LMASK) != 0);

				onRightMouseDown (leftPressed, event.button.x, event.button.y);

				m_redraw = true;
				return; 
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				bool repaint = onLeftMouseUp (event.button.x, event.button.y);

				m_redraw |= repaint;

				return; 
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			int tx, ty;
			Uint32 buttons = SDL_GetMouseState (&tx, &ty);

			// remove any pending mousemove so they dont cause too many redraws.
			SDL_FlushEvent(SDL_MOUSEMOTION);

			bool leftPressed = ((buttons & SDL_BUTTON_LMASK) != 0);

			if (leftPressed)
			{
				if (onMouseMove (event.button.x, event.button.y))
				{
					m_redraw = true;
				}
			}
		}	
	}

    return ;
}

std::string Game::getWindowTitle (bool game_hex)
{
	if (game_hex) 
		return "Beesweeper Hex";
	else 
		return "Beesweeper Classic";
}

DigitColor Game::DigitColorFromLevel(GameLevel level)
{
	switch (level)
	{	
		case LEVEL_BEGINNER:
			return DIGIT_GREEN;
		case LEVEL_INTERMED:
			return DIGIT_BLUE;
		case LEVEL_ADVANCED:
			return DIGIT_PINK;
		default:
			return DIGIT_GREEN;
	}
}

void Game::startNewGame () // same parameters as before
{
	bool flag_mode = false;

	bool auto_marks = m_settings.auto_marks;
	bool show_qmarks = m_settings.show_qmarks;
	GameLevel level = (GameLevel)m_settings.level;
	bool batch_mode = m_settings.batch_mode;

	this->startNewGame (level, batch_mode, auto_marks, show_qmarks, flag_mode);
}

void Game::startNewGame (GameLevel level, bool batch_mode, bool auto_marks, bool show_qmarks, bool flag_mode)
{
	game_field->onNewGame(level, auto_marks, show_qmarks, flag_mode);

	game_field->setBatchMode (batch_mode);

	stopTimer();

	DigitColor color = DigitColorFromLevel (level);

	minesView->changeColor (color, renderer);
	secondsView->changeColor (color, renderer);
	
	minesView->setValue (game_field->getMinesCount()); 
	secondsView->setValue (game_field->getTimerSeconds());

	faceButton->setState (game_field->getFaceState());

	m_redraw = true;
}

// return true to repaint
bool Game::onLeftMouseUp(int x, int y)
{
	bool clicked = false;

	if (faceButton->onLeftMouseUp (x, y, clicked))
	{
		if (clicked)
		{
			this->startNewGame();
		}

		return true;
	}

	if (settingsBtn->onLeftMouseUp (x, y, clicked))
	{
		game_field->informController (TYPE_SETTINGS);

		return true;
	}
		
	bool ret = false;
				
	int minesWas = game_field->getMinesCount();

	bool timerWasEnabled = game_field->isTimerEnabled();
		
	if (game_field->getFlagMode()) 
	{
		game_field->rightButtonDown (renderer, false, x, y);
	}
	else  
	{
		game_field->leftButtonUp (renderer, x, y);

		ret = true;
	}
		
	if (game_field->isTimerEnabled() && timerWasEnabled == false)
	{
		startTimer();
	}
	else if (timerWasEnabled && game_field->isTimerEnabled() == false)
	{
		stopTimer();
	}

	if (minesWas != game_field->getMinesCount() )
	{			
		minesView->setValue (game_field->getMinesCount() );

		ret = true;
	}
		
	if (game_field->isTimerEnabled() == false)
	{				
		ret = true;
	}

	return (ret);
}

bool Game::onLeftMouseDown (bool rightAlsoPressed, int x, int y)
{
	if (faceButton->onLeftMouseDown(x,y))
	{
		return true;
	}

	if (settingsBtn->onLeftMouseDown(x,y))
	{
		return true;
	}

	game_field->leftButtonDown (renderer, rightAlsoPressed, x, y);
	return false;
}

bool Game::onRightMouseDown (bool leftAlsoPressed, int x, int y)
{
	if (faceButton->hasFocus())
		return false;

	game_field->rightButtonDown (renderer, leftAlsoPressed, x, y);

	return true;
}

bool Game::onRightMouseUp (int x, int y)
{
	game_field->rightButtonUp (renderer);

	return true;
}

bool Game::onMouseMove (int x, int y)
{
	if (faceButton->hasFocus ())
	{
		return faceButton->onMouseMove(x, y);
	}

	if (settingsBtn->hasFocus())
	{
		return settingsBtn->onMouseMove(x, y);
	}

	return game_field->MouseMove (renderer, x, y);
}

void Game::render()
{
    int width = 0, height = 0;

	m_redraw = false;

	SDL_GetWindowSize (window, &width, &height);

	SDL_Rect rect = { 0, 0, width, height};

	game_field->Paint (renderer, rect);

	secondsView->render (renderer);

	minesView->render (renderer);

	faceButton->render (renderer);

	settingsBtn->render (renderer);

    SDL_RenderPresent(renderer);
}

///////////////////////////////////////////////////////////////////////////////////

 void Game::applyNewSettings (const struct gameSettings newSettings)
 {
	// start with most "serious changes": game type change, width or height change

	if (newSettings.game_hex != m_settings.game_hex || newSettings.width != m_settings.width || newSettings.height != m_settings.height)
	{
		// obtain new screen size
		// discard old field.
		// resize screen
		// create new field
		// move controls.
		// start new game. 

		int width = 0, height = 0;

		getRequiredScreenDimensions (newSettings.game_hex, newSettings.width, newSettings.height, width, height);

		delete game_field;
		game_field = nullptr; 

		bool hex_game = newSettings.game_hex;

		const std::string title = getWindowTitle (hex_game);
		SDL_SetWindowTitle (window, title.c_str() );

		bool scale = newSettings.scale ;
		bool highlight_cells = newSettings.highlight;

		SDL_SetWindowSize (window, scaledValue (width, scale), scaledValue (height, scale) );

		setupLayout (hex_game, width, height, highlight_cells, scale);

		int level = newSettings.level;
		bool batch_mode = newSettings.batch_mode;
		bool auto_marks = newSettings.auto_marks;
		bool show_qmarks = newSettings.show_qmarks;

		bool reload_images = (newSettings.scale != m_settings.scale);

		m_settings = newSettings;

		this->startNewGame((GameLevel)level, batch_mode, auto_marks, show_qmarks);

		if (reload_images)
		{
			this->reloadImages (renderer, newSettings.scale);
		}
	}
	else // no change in size or type, so no need to resize the window or move controls.
	{
		bool new_game = (newSettings.level != m_settings.level);
		
		int level = newSettings.level;

		bool batch_mode = newSettings.batch_mode;
		bool auto_marks = newSettings.auto_marks;
		bool show_qmarks = newSettings.show_qmarks;

		if (newSettings.scale != m_settings.scale)
		{
			int scale = newSettings.scale;

			int width, height;
			SDL_RenderGetLogicalSize (renderer, &width, &height);
			SDL_SetWindowSize (window, scaledValue (width, scale), scaledValue (height, scale) );

			this->reloadImages (renderer, newSettings.scale);
		}

		if (newSettings.highlight != m_settings.highlight)
		{
			game_field->changeHighlightCells (renderer, newSettings.highlight);
		}

		if (newSettings.sound_volume != m_settings.sound_volume)
		{
			// int volume = newSettings.sound_volume;
			// do nothing, new valume is set in m_settings.
		}

		m_settings = newSettings;

		if (new_game)
		{
			this->startNewGame ((GameLevel)level, batch_mode, auto_marks, show_qmarks);
		}
		else 
		{
			game_field->setBatchMode (m_settings.batch_mode);
			game_field->setAutoMarks (m_settings.auto_marks);
			game_field->setShowQMarks (m_settings.show_qmarks);
		}
	}

 }

 void Game::reloadImages (SDL_Renderer *g, bool scaled)
 {
	settingsBtn->reloadImages (g, scaled);
	faceButton->reloadImages (g, scaled);

	game_field->reloadImages (g, scaled);

	secondsView->reloadImages (g, scaled);
	minesView->reloadImages (g, scaled);
 }
