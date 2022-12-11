/*
 *  GenericField.h
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

#include "image.h"
#include "GameClasses.h"
#include <unordered_set>
#include <cassert>
#include <ctime>

#define USE_BATCHED

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator () (std::pair<T1, T2> const &pair) const
    {
        std::size_t h1 = std::hash<T1>()(pair.first);
        std::size_t h2 = std::hash<T2>()(pair.second);
 
        return h1 ^ (h2 << 8);
    }
};

typedef SDL_Renderer* CGContextRef;
typedef SDL_Rect  CGRect;

enum USER_EVENT_TYPES { TYPE_TIMER = 0, TYPE_SETTINGS, TYPE_SOUND_BLAST, TYPE_SOUND_VICTORY, TYPE_MINES_CHANGED, TYPE_FACE_CHANGED };
enum GameLevel { LEVEL_BEGINNER = 0, LEVEL_INTERMED, LEVEL_ADVANCED, LEVEL_MAX };

class GenericField 
{
    public:
    GenericField (int w, int h, Uint32 userEventID, SDL_Renderer * r) : width(w), height(h), m_userEventType (userEventID)
    { 
        m_flag_mode = false;

        memset (cell_digits, 0, CELL_DIGITS_MAX * sizeof(Image *));

        init (r);
    }

    virtual ~GenericField();

    // forbid coping since we have raw pointer in class.
    GenericField (const GenericField &) = delete;
    GenericField& operator = (const GenericField &) = delete;

    static const char GAME_SQUARES = 1;
	static const char GAME_HEXAGONS = 0;

    virtual char gameType() const = 0;

    void onNewGame(GameLevel level, bool auto_marks, bool show_qmarks, bool flag_mode);

    void setBatchMode (bool value) { m_batch_mode = value; }
    bool getBatchMode() const { return m_batch_mode; }

    bool isTimerEnabled () const { return bTimerEnable; }
    int getMinesCount() const { return iMinesCount; }
    int getTimerSeconds () const { return iSeconds; }
    void setTimerSeconds (int v) { iSeconds = v; }
    FaceState getFaceState () const { return iFaceState; }

    bool getFlagMode() const { return m_flag_mode; }
    
    bool getAutoMarks () const { return m_auto_marks; }
    void setAutoMarks (bool v) { m_auto_marks = v; }

    bool getShowQMarks () const { return m_show_qmarks; }
    void setShowQMarks (bool v) { m_show_qmarks = v; }

    GameLevel getGameLevel() const { return m_game_level; }
    bool getHighlight () const { return m_highlight; }
 
    void Paint (CGContextRef g, CGRect rect) const;
    void changeHighlightCells (CGContextRef g, bool);

    virtual void reloadImages (SDL_Renderer *g, bool scale) = 0;

    int getScreenHeight() const;
    int getScreenWidth() const;

    // mouse events
    void leftButtonDown (CGContextRef g, bool rightPressed, int X, int Y);

    bool leftButtonUp (CGContextRef g, int X, int Y);

    void rightButtonDown (CGContextRef g, bool leftPressed, int X, int Y) ;
    void rightButtonUp (CGContextRef g);

    bool MouseMove(CGContextRef g, int X, int Y); 
    void informController (int message_type) const;

    static const int mPanelHeight = 37;

    static const int mBevelFlat = 6;
    static const int mBevelEdge = 3;
    static const int mBevelWidth = (mBevelFlat + (mBevelEdge * 2)); // 3 + 3 + 6

    static const int mHeaderMargin = mPanelHeight + mBevelFlat + mBevelEdge + 9;

    protected:

    static const int CELL_DIGITS_MAX = 8;

    Image *mineBmp = nullptr, *errorBmp = nullptr, *flagBmp = nullptr, *qmarkBmp = nullptr;

    CellElement & Element (int A, int B) const 
    {
        assert (CellEl != NULL);

        assert (!(A < 0 || B < 0));

        return *(CellEl + A * DimL + B);
    }

    bool bCollapse = false; // when true, prevent mouse events till new game.
    bool bVictGame = false; // flag indicating that game was won.
    int iSeconds = 0, 
        iFreeCount = 0, // tracks # of unopened cells without mines. When this # gets to zero, user wins the game. 
        iMinesCount = 0; // tracks # of unmarked cells with mines. This number (it can become negative when user marks more mines than there is) is displayed on top left. 

    bool fCaptured = false, fBatched = false;

#ifdef USE_BATCHED
    int BatchedA = -1, BatchedB = -1;
#endif 

    // keeps track of the depressed cell when mouse is moved.
    int PressedA = -1, PressedB = -1;

    FaceState iFaceState = fsSmile;

    int width = 0, height = 0;

    // flag mode is needed only on devices without right mouse
    // such as mobile phone. when set to true, simple (left) clock (touch) is equivalent to
    // right mouse click on computer.

    bool m_flag_mode = false;                       // flag mode used to be in mobile versions where there's no right mouse to flag a cell. It's always false in this version.
    GameLevel m_game_level = LEVEL_BEGINNER;        // conrols how many mines are distributed.
    bool bTimerEnable = false;
    bool m_show_qmarks = false;      // same as in original game. Right mouse click shows flags, then question marks.
    bool m_auto_marks = false;       // automatically mark obvious mines.
    bool m_batch_mode = false;       // controls "chording"
    bool m_highlight = false;        // when true, unopned cells are shown with light blue background.

    Uint32 m_userEventType = 0;

    std::unordered_set<std::pair<int,int>, pair_hash>  opened_cells = {};

    protected:

    int DimK = 0, DimL = 0;

    Image *cell_digits[CELL_DIGITS_MAX];

    void reallocCells (int count);

    bool initImages (SDL_Renderer *);

    void reinitImages (SDL_Renderer *g, bool scale);

    void initCellDigitsBitmaps (SDL_Renderer *, int count, bool scaled = true);
    void freeImages();
    void freeCellDigitsBitmaps ();

    virtual void closedCellImage (SDL_Renderer *, bool highlight_cells) = 0;

    void drawBitmap (SDL_Renderer *r, int x, int y, int w, int h, Image *, bool flip_vertically = false) const;
    void drawCellDigit (CGContextRef context, int x, int y, int digit) const;

    virtual bool IsRealCell (int A, int B) const = 0;

    virtual void XYtoAB (int X, int Y, int & A, int & B) const  = 0;

    int Cell_State(int A, int B) const ;
    void Cell_Set(int A, int B, int v) ;

    bool CellIsClosed (int A, int B) const ;

    bool IsMined (int A, int B) const ;
    void IsMined_Set (int A, int B, bool v) ;

    void IncMines(int A, int B); 
    void SubMine(int A, int B) ;

    void Mines_Set(int A, int B, int v) ;

    int getMines(int A, int B) const;

    bool IsBatched(int A, int B) const;
	void IsBatched_Set(int A, int B, bool v);

    static int nextInt (int max); // generates next random integer (used when distributing mines)

    virtual bool autoMark (CGContextRef g, int A, int B) = 0;
    void autoMarkAll (CGContextRef g);

    void MinesChange (CGContextRef g, CGRect *rect) ;

    virtual void DistributeMines() = 0;
 
    virtual void MoveMineSomewhereElse (int A, int B) = 0;

    virtual void openCellsOnVictory(CGContextRef g) = 0 ;

    virtual void CallReaction(CGContextRef g,int A, int B) = 0;

    void Blast();

    void Victory(CGContextRef g);

    void MouseUp(CGContextRef g, int PressedA, int PressedB) ;

    void RightMouseDown(CGContextRef g, int X, int Y);

    void playErrorSound() const;
	void playVictorySound() const;

    void HandlePressing(int A,int B, bool IsPressed) const;

#ifdef USE_BATCHED
    void RestoreBatched (CGContextRef g, int A, int B);
    void ProbeCell(int A, int B, int & Cnt, bool & MayCollapse) ;
    void DiscloseBatched (CGContextRef g, int A, int B);
    void BatchMouseMove (CGContextRef g, int X, int Y);
    virtual void HandleBatchPressing(CGContextRef g, int A, int B) = 0;

    void ToggleBatched (int A, int B) ;

    void DrawBatched (int A, int B);
    void DrawPressed(int A, int B) const ;
    
#endif 

    private:

    CellElement *CellEl = nullptr;

    void init (SDL_Renderer * renderer);

    void paintBackground (CGContextRef g, CGRect rect) const;

    void drawOutlines (CGContextRef g) const;

    // virtual private
    virtual void PaintCells (CGContextRef g, CGRect rect) const = 0;

    virtual void DrawCell (CGContextRef g, int A, int B, CGRect * rect) const = 0;

    virtual void setupField () = 0; // allocates memory for cells based on field size.

#ifdef USE_BATCHED

    virtual void DiscloseBatch(CGContextRef g) = 0;

#endif 

    void UpdateFace ();


};