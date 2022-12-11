/*
 *  SDLTimer.h
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

class SDLTimer 
{
    public:
    SDLTimer (Uint32 millis, Uint32 userEventID, Sint32 event_code) : 
                    timer_id(0), m_userEventID (userEventID), m_code(event_code), m_started(0)
    {
        timer_id = SDL_AddTimer (millis, timer_callback, this);

		if (timer_id == 0)
		{
			SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "Timer not started.");
		}

        m_started = SDL_GetTicks();

    }
    ~SDLTimer ()
    {
        SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "SDL timer destructor\n");

        if (timer_id > 0)
	    {
		    SDL_RemoveTimer (timer_id);
	    }
    }

    bool isValid () const { return (timer_id != 0); }

    int msSinceStart () const 
    {
        return (SDL_GetTicks() - m_started);
    }

    // forbid coping since we have raw pointer in class.
    SDLTimer(const SDLTimer &) = delete;

    SDLTimer& operator = (const SDLTimer &) = delete;

    private:

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /* 	If the returned value is the same as the one passed in, the periodic alarm continues, 
	    otherwise a new alarm is scheduled. If the callback returns 0, the periodic alarm is cancelled. */
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    static Uint32 timer_callback(Uint32 interval, void *param)
    {
	    SDL_Event event;
	    SDL_UserEvent userevent;

	    /* In this example, our callback pushes an SDL_USEREVENT event
	    into the queue, and causes our callback to be called again at the
	    same interval: */

	    SDLTimer * timer = (SDLTimer *)param;

	    userevent.type = timer->m_userEventID;
	    userevent.code = timer->m_code;
	    userevent.data1 = NULL;
	    userevent.data2 = NULL;

	    event.type = SDL_USEREVENT;
	    event.user = userevent;

	    SDL_PushEvent(&event);

	    return (interval);
    }

    SDL_TimerID timer_id = 0;

    Uint32 m_userEventID = 0;
    Sint32 m_code = 0;
    Uint32 m_started = 0;
};

typedef std::unique_ptr<SDLTimer> TimerPtr;
