/*
 *  GenericField.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "GenericField.h"
#include "graphics.h"
#include "image.h"
#include <vector>

#if !defined (_MSC_VER)
#define sprintf_s(buf, ...) snprintf((buf), sizeof(buf), __VA_ARGS__)
#endif

GenericField::~GenericField()
{
	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "GenericField destructor entered");

	freeImages();

	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "GenericField destructor post images");

	delete [] CellEl;

	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "GenericField destructor exit");

	opened_cells.clear(); // not really needed as this is done automatically.
}

void GenericField::onNewGame (GameLevel level, bool auto_marks, bool show_qmarks, bool flag_mode)
{
	bCollapse = false;
	
	bTimerEnable = false;

	iSeconds = 0;

	m_show_qmarks = show_qmarks;
	m_auto_marks = auto_marks;
	
	iFaceState = fsSmile;

	bVictGame = false;  

	opened_cells.clear();

	m_flag_mode = flag_mode;

	m_game_level = level;
	
	DistributeMines();  
}

void GenericField::init (SDL_Renderer * renderer)
{
	initImages (renderer);
}

void GenericField::reallocCells (int count)
{
	delete[] CellEl;

	CellEl = nullptr;

	CellEl = new CellElement[count];
}

bool GenericField::initImages(SDL_Renderer *r)
{
	mineBmp = new Image (r, "bomb.png");
		
	flagBmp = new Image (r, "flag.png");

	qmarkBmp = new Image (r, "qmark.png");
		
	errorBmp = new Image (r, "error.png");	
	
	return true;
}

void GenericField::reinitImages (SDL_Renderer *g, bool scale)
{
	mineBmp->reload (g, scale);
		
	flagBmp->reload (g, scale);

	qmarkBmp->reload (g, scale);
		
	errorBmp->reload (g, scale);	
}

// init small images showing # of mines around given cell, if any.
void GenericField::initCellDigitsBitmaps (SDL_Renderer *r, int count, bool scaled)
{
	for (int i=0; i < count; i++)
	{
		char buffer[64];
		
		sprintf_s (buffer, "digit%d.png", i + 1);
		        
		cell_digits[i] = new Image (r, buffer, scaled);
	}
}

void GenericField::freeCellDigitsBitmaps ()
{
	for (int i = 0; i < CELL_DIGITS_MAX; i++)
	{
		delete cell_digits[i];
		cell_digits[i] = nullptr;
	}
}

void GenericField::changeHighlightCells (CGContextRef g, bool highlight)
{
	this->closedCellImage (g, highlight);
}

void GenericField::freeImages()
{
	delete mineBmp;
	mineBmp = nullptr;
	
	delete flagBmp;
	flagBmp = nullptr;
	
	delete qmarkBmp;
	qmarkBmp = nullptr;
		
	delete errorBmp;
	errorBmp = nullptr;
		
	freeCellDigitsBitmaps();
}

int GenericField::getScreenHeight() const
{
	return height;
}

int GenericField::getScreenWidth() const
{
	return width;
}

void GenericField::drawBitmap (SDL_Renderer *r, int x, int y, int w, int h, Image *img, bool flip_vertically) const
{
	img->draw (r, x, y, w, h, flip_vertically);
}

void GenericField::paintBackground (CGContextRef g, CGRect rect) const
{
    SDL_SetRenderDrawColor (g, 192, 192, 192, 255);	
		
	SDL_RenderClear (g);
}

void GenericField::drawOutlines (CGContextRef g) const
{
    int left0 = 0;
    int top0 = 0;

	int FramePlusFlat = mBevelEdge + mBevelFlat;
	
    CGRect rect = { left0, top0, width, height };
    
    Graphics::Frame3D(g, rect, Graphics::RAISED, mBevelEdge);
    Graphics::InflateRect(rect, -FramePlusFlat, -FramePlusFlat);
		
    int top = rect.y; 
    int bottom = top + rect.h; 
    
    rect.y = (mHeaderMargin - mBevelEdge) + top0; 
	rect.h = (bottom - rect.y);

    Graphics::Frame3D(g, rect, Graphics::LOWERED, mBevelEdge);
    
    if (m_flag_mode)
    {	
		if (m_game_level == LEVEL_BEGINNER)
			Graphics::setFillColor (g, 0x00ff00, 0.5);
		else if (m_game_level == LEVEL_INTERMED)
			Graphics::setFillColor (g, 0x005174, 0.5);
		else if (m_game_level == LEVEL_ADVANCED) 
			Graphics::setFillColor(g, 0x800040, 0.5); 
		
		CGRect trect = { rect.x + mBevelEdge, rect.y + mBevelEdge, rect.w - 2 * mBevelEdge + 1, rect.h - 2 * mBevelEdge + 1 } ;
		
		Graphics::fillRect (g, &trect);
    }  
        
    rect.y = FramePlusFlat + top0;
    rect.h = mPanelHeight;
    Graphics::Frame3D(g, rect, Graphics::LOWERED, 2);
    
}

void GenericField::drawCellDigit (CGContextRef context, int x, int y, int digit) const
{
	drawBitmap (context, x + 5, y + 4, 11, 10, cell_digits[digit-1], true );	
}

void GenericField::Paint (CGContextRef g, CGRect rect) const
{
	paintBackground(g, rect);
	
	drawOutlines (g); 
	
	PaintCells (g, rect);
}

int GenericField::Cell_State(int A, int B) const 
{
	return Element (A - 1, B - 1).State;
}

void GenericField::Cell_Set(int A, int B, int v) 
{
//	bool add = (Element(A-1,B-1).State != v); 
	
    Element(A - 1, B - 1).State = v;	
}

int GenericField::getMines(int A, int B) const 
{
    return Element (A - 1, B - 1).MineCnt;
}

void GenericField::Mines_Set(int A, int B, int v) 
{
    Element (A - 1, B - 1).MineCnt = v;
}

bool GenericField::IsMined (int A, int B) const 
{
    return Element (A - 1, B - 1).IsMined;
}

void GenericField::IsMined_Set (int A, int B, bool v) 
{
    Element (A - 1, B - 1).IsMined = v;
}

void GenericField::IncMines(int A, int B) 
{
	if (IsRealCell(A, B)) Mines_Set(A, B, getMines(A, B) + 1);
}

void GenericField::SubMine(int A, int B) 
{
	if (IsRealCell(A, B)) Mines_Set(A, B, getMines(A, B) - 1);
}

bool GenericField::IsBatched(int A, int B) const
{
    return Element (A - 1, B - 1).Batched;
}

void GenericField::IsBatched_Set(int A, int B, bool v)
{       
	Element (A - 1, B - 1).Batched = v;
}

bool GenericField::CellIsClosed (int A, int B) const
{
	if (IsRealCell(A, B))
	{
		if (Cell_State(A, B) != csOpened) 
			return true;
	}

	return false;
}      

int GenericField::nextInt(int max)
{
	return (rand() % max);
}

void GenericField::informController (Sint32 message_code) const
{
	SDL_Event event;
	SDL_UserEvent userevent;

	/* In this example, our callback pushes an SDL_USEREVENT event
	into the queue, and causes our callback to be called again at the
	same interval: */

	userevent.type = m_userEventType;
	userevent.code = message_code;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
}

void GenericField::autoMarkAll (CGContextRef g)
{
    if (m_auto_marks == false) 
		return;

	std::vector<std::pair<int,int> > to_remove;
    
    for (auto & cell : opened_cells)
    {
        if (autoMark (g, cell.first, cell.second))
		{
			to_remove.push_back (cell);
		}
    }

	for (auto & cell : to_remove)
	{
		opened_cells.erase (cell);
	}
}        

void GenericField::MinesChange(CGContextRef g, CGRect *rect) 
{
	informController (TYPE_MINES_CHANGED);
}

void GenericField::Blast() 
{
	bTimerEnable = false;
		
	playErrorSound ();    
}

void GenericField::Victory(CGContextRef g)
{
	bTimerEnable = false;
	
	iMinesCount = 0;
	
	playVictorySound();
    
	MinesChange(g, nullptr);
	
	bVictGame = true;
	bCollapse = true; // Mouse events are prevented from here
    
	openCellsOnVictory(g);
	
	if (iSeconds <= 1999)
	{
		// [parent_controller onVictory];
	}
}

void GenericField::UpdateFace ()
{
	// send message to controller to change face.
	informController (TYPE_FACE_CHANGED);
}

void GenericField::rightButtonDown (CGContextRef g, bool leftAlsoPressed, int X, int Y)
{
	if (bCollapse) 
		return ;

	if (leftAlsoPressed && m_batch_mode) 
	{
#ifdef USE_BATCHED
        fBatched = true;
        fCaptured = false;

        PressedA = 0;
        PressedB = 0;
        
		BatchMouseMove (g, X - mBevelWidth, Y - mHeaderMargin );

        iFaceState = fsShout;
		UpdateFace();
#endif 
    }
	else 
	{		
		RightMouseDown (g, X - mBevelWidth, Y - mHeaderMargin );    
	}     	
}

void GenericField::rightButtonUp (CGContextRef g)
{   
   if (fBatched)
   {  
#ifdef USE_BATCHED
        DiscloseBatch (g);
#endif 		

        fBatched = false;
        
		if (bVictGame) 
			iFaceState = fsBoss;
    
	    else if (bCollapse) 
			iFaceState = fsWeep;
    
	    else 
			iFaceState = fsSmile;
    
	    UpdateFace();       
    }                
}


void GenericField::MouseUp(CGContextRef g, int PressedA, int PressedB)
{
	int A = PressedA, B = PressedB;

	PressedA = PressedB = 0;
	
	if (IsRealCell(A, B)) 
	{
		if ((Cell_State(A, B) == csMarked) && IsMined(A, B)) 
		{
			SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "Enabling timer");

			if (bTimerEnable == false)
			{
				bTimerEnable = true;
			}
		}
		else if (Cell_State(A, B) == csClosed || Cell_State(A, B) == csQuest) 
		{   
			if (bTimerEnable == false && IsMined(A, B))
			{
				MoveMineSomewhereElse (A, B);
			}
				
			if (bTimerEnable == false)
			{
				bTimerEnable = true;
			}
			
			if (!IsMined(A, B) && getMines(A, B)==0) 
			{   
				CallReaction(g, A, B);

				autoMarkAll(g);
					
				if (0 == iFreeCount) 
					Victory(g);
			}                                     
			else 
			{                                  
				Cell_Set(A, B, csOpened);
				
				if (IsMined(A, B)) 
				{                   
					bCollapse = true;
					DrawCell(g, A, B, nullptr);
					Blast();
				}                                 
				else 
				{                                 
					DrawCell(g,A,B, nullptr);
												
					opened_cells.insert( {A, B} );
					autoMarkAll(g);
						
					iFreeCount--;

					if (0 == iFreeCount) 
						Victory (g);
				}                                 
			}                                   
		}                                        
	}                                          
}

void GenericField::RightMouseDown (CGContextRef g, int X, int Y) 
{
	int A, B;

	XYtoAB (X, Y, A, B);
	
	if (IsRealCell(A, B)) 
	{
		if (Cell_State(A, B) != csOpened) 
		{
			int StartState = Cell_State(A, B);

			switch (StartState) 
			{
				case csClosed:

                    Cell_Set(A, B, csMarked);
                    iMinesCount--;
                    MinesChange(g, nullptr);

                    break;
			
				case csMarked:
                    if (m_show_qmarks)
					{ 
                        Cell_Set(A, B, csQuest);
					}
                    else
					{ 
					    Cell_Set(A, B, csClosed);
					}
                    
					iMinesCount++;
                    MinesChange(g, nullptr);
                    
					break;
			
				case csQuest:
                    
					Cell_Set(A,B, csClosed);

                    break;
			} // end of switch

			DrawCell (g,A,B, nullptr);
		}
	}
}



void GenericField::playVictorySound() const
{
	informController (TYPE_SOUND_VICTORY);
}

void GenericField::playErrorSound() const
{
	informController (TYPE_SOUND_BLAST);
}

////////////////////////////////////////////////////////////////////////////

#ifdef USE_BATCHED
void GenericField::DiscloseBatched (CGContextRef g, int A, int B)
{
	if (IsRealCell (A, B)) 
	{
    	IsBatched_Set(A, B, false);

    	if ((Cell_State(A,B) == csClosed || Cell_State (A,B) == csQuest)) 
		{ 
    		if (!IsMined(A,B) && getMines(A,B) == 0) 
			{     
      			CallReaction (g, A, B);

				autoMarkAll(g);

      			if (0 == iFreeCount) 
					Victory (g);
      		}                                       
    		else 
			{                                    
      			Cell_Set(A, B, csOpened);
      			
				if (IsMined(A,B)) 
				{                     
            		DrawCell(g, A, B, nullptr);
           	 		Blast();
           		}                                  
      			else 
				{                                  
            		DrawCell(g, A, B, nullptr);

					opened_cells.insert( {A,B} );
					autoMarkAll (g);
            		
					iFreeCount--;

            		if (0 == iFreeCount) 
						Victory(g);
           		}                                  
         	}                                    
       	}	                                      
    }
}

void GenericField::RestoreBatched (CGContextRef g, int A, int B) 
{
  if (IsRealCell (A,B) && IsBatched(A,B)) 
  {
    IsBatched_Set(A, B, false);

    if (Cell_State (A,B) == csClosed || Cell_State (A,B) == csQuest) 
	 	DrawCell (g, A,B, nullptr);
  }
}

void GenericField::ProbeCell(int A, int B, int & cnt, bool & MayCollapse) 
{
 	if (IsRealCell(A,B)) 
	{
   		if (Cell_State (A,B) == csMarked) 
			cnt++;
   		else if (IsMined(A, B)) 
			MayCollapse = true;
   }
}

void GenericField::BatchMouseMove(CGContextRef g, int X, int Y) 
{
	int A, B;
  	XYtoAB (X, Y, A, B);

  	if ((A != BatchedA) || (B != BatchedB)) 
		HandleBatchPressing (g, A, B);
}

void GenericField::ToggleBatched (int A, int B) 
{
    if (IsRealCell(A,B))
    {
        bool v = IsBatched (A, B); 
        IsBatched_Set (A,B, !v);
    }
}

void GenericField::DrawBatched (int A, int B) 
{
  	if (IsRealCell(A, B)) 
  	{
  		if (IsBatched (A, B)) 
		{
    		if (Cell_State (A, B) == csClosed || Cell_State (A, B) == csQuest) 
				DrawPressed(A, B);
    	}
    	else
		{ 
			IsBatched_Set (A, B, true);
		}
    }
}

void GenericField::DrawPressed(int A, int B) const
{
}

#endif

////////////////////////////////////////////////////////////////////

void GenericField::leftButtonDown (CGContextRef g, bool rightAlsoPressed, int X, int Y)
{
    if (bCollapse) 
		return ;
        
    if (rightAlsoPressed && m_batch_mode) 
	{
        fBatched = true;

		PressedA = -1;
        PressedB = -1;

#ifdef USE_BATCHED        
		BatchMouseMove (g, X - mBevelWidth, Y - mHeaderMargin );
#endif 

		if (iFaceState != fsShout) 
		{
            iFaceState = fsShout;
            UpdateFace();
        }
    }
    else if (!fCaptured) 
	{
        fCaptured = true;

		X = X - mBevelWidth;
		Y = Y - mHeaderMargin;

		int A, B;
  		XYtoAB (X, Y, A, B);

		PressedA = A, PressedB = B;

		if (iFaceState != fsShout) 
		{
            iFaceState = fsShout;
            UpdateFace();
        }
    }
}

bool GenericField::leftButtonUp (CGContextRef g, int X, int Y)
{
	bool ret = false;
	
	if (fCaptured || fBatched) 
	{
		if (bCollapse) 
			return (ret);
		
		if (fBatched) 
		{
#ifdef USE_BATCHED
            DiscloseBatch (g);
#endif 			
            fBatched = false;
		}
		else if (fCaptured) 
		{
			int A, B;
			XYtoAB(X - mBevelWidth, Y - mHeaderMargin, A, B);
            
			MouseUp (g, A, B);
			ret = true;
            fCaptured = false;
		}

		if (bVictGame) 
			iFaceState = fsBoss;
		else if (bCollapse) 
			iFaceState = fsWeep;
		else 
			iFaceState = fsSmile;

		UpdateFace();           
	}
	
	return (ret);
}

void GenericField::HandlePressing(int A,int B, bool IsPressed) const
{
#ifdef USE_BATCHED  	
	if ( IsRealCell(A, B) && (Cell_State (A, B) == csClosed || Cell_State (A, B) == csQuest) )
  	{
    	if (!IsPressed) 
			DrawCell (nullptr, A, B, nullptr);
    	else 
			DrawPressed(A, B);
  }
#endif 

}

bool GenericField::MouseMove (CGContextRef g, int X, int Y) 
{    
	if (bCollapse) 
		return false;

    if (fBatched || fCaptured) 
	{
        X = X - mBevelWidth;
        Y = Y - mHeaderMargin;

#ifdef USE_BATCHED
        if (fBatched)
		{
        	BatchMouseMove (g, X, Y);
			return true;
		}
#endif 

        if (fCaptured)
		{
			int A, B;
  			XYtoAB (X, Y, A, B);

			if ((A != PressedA) || (B != PressedB)) 
			{
    			HandlePressing (PressedA, PressedB, false );

    			HandlePressing (A, B, true);

				PressedA = A;
    			PressedB = B;

				return true;
  			}
		}
        
		if (iFaceState != fsShout) 
		{
              iFaceState = fsShout;
              UpdateFace();
        }
    }
    
	return false;
}
