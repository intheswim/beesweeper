/*
 *  HexGameField.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "HexGameField.h"
#include "graphics.h"

HexGameField::HexGameField(int w, int h, Uint32 userEventID, SDL_Renderer *r, bool highlight_cells) : GenericField(w, h, userEventID, r)
{
	m_highlight = highlight_cells;
	
	if (initImages(r, highlight_cells))
	{
		setupField();
	}
}

HexGameField::~HexGameField()
{
	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "HexGame destructor entered");

	freeImages ();

	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "HexGame destructor exit\n");
}

void HexGameField::getDimensions(int width, int height, int & dimK, int & dimL) 
{
	int area = height - mBevelWidth - mHeaderMargin;
	dimL = area / 18;
	area = width - mBevelWidth * 2;
	dimK = 1 + (area - 20) / 32;
}

void HexGameField::setupField ()
{	
	getDimensions (width, height, DimK, DimL);

    int ics = (DimK < DimL - 1) ? (DimK & 1) : (DimL & 1) ;
	IsCornerSuit = (ics != 0);

    reallocCells ((DimK * DimL) << 1);
	
	int area = width - mBevelWidth * 2;
	
	int hex_area = (DimK - 1) * 32 + 20;
	left_field_shift = (area - hex_area) / 2;
	
	left_field_shift += (getScreenWidth() - width) / 2 - 1;
	
	top_field_shift = (height - mBevelWidth - mHeaderMargin - DimL * 18) / 2 - 1;
	
	top_field_shift += (getScreenHeight() - height) / 2;	
}

bool HexGameField::initImages (SDL_Renderer *r, bool highlight_cells)
{
	blastBmp = new Image (r, "cell5_blast.png");
		
	openedBmp = new Image (r, "cell5_empty.png");
			
	initCellDigitsBitmaps (r, 6);
	
	closedCellImage (r, highlight_cells);
	
	hex_bottom       = new Image (r, "hex_bottom.png");
	hex_right_bottom = new Image (r, "hex_right_bottom.png");
	hex_right_top    = new Image (r, "hex_right_top.png");
	
	return true;
}

void HexGameField::closedCellImage (SDL_Renderer *r, bool highlight_cells)
{
	if (closedBmp != nullptr)
	{
		delete closedBmp;
	}
	
	closedBmp = (highlight_cells ? new Image (r, "cell6.png") : new Image (r, "cell5.png"));
}

void HexGameField::freeImages ()
{
	delete blastBmp ;
	blastBmp = nullptr;
	
	delete openedBmp ;
	openedBmp = nullptr;
		
	delete closedBmp ;
	closedBmp = nullptr;
	
	delete hex_bottom ;
	hex_bottom = nullptr;
	
	delete hex_right_bottom;
	hex_right_bottom = nullptr;
	
	delete hex_right_top;	
	hex_right_top = nullptr;
}

void HexGameField::reloadImages (SDL_Renderer *r, bool scale)
{
	this->reinitImages (r, scale);

	blastBmp->reload(r, scale);
		
	openedBmp->reload (r, scale);
			
	freeCellDigitsBitmaps ();
	initCellDigitsBitmaps (r, 6, scale);
	
	closedBmp->reload (r, scale);
	
	hex_bottom->reload (r, scale);
	hex_right_bottom->reload (r, scale);
	hex_right_top->reload (r, scale);
}


void HexGameField::PaintCells (CGContextRef g, CGRect rect) const
{
	int A = 0, B = 0;
	
	IntPoint pt = XYtoAB ( rect.x - mBevelWidth - 6, rect.y - mHeaderMargin );
	
	int minA = pt.X ; 
	int minB = pt.Y - 1;
	
	if (minA < 1) minA = 1;
	if (minB < 1) minB = 1;
	
	IntPoint pt2 = XYtoAB ( rect.x - mBevelWidth + rect.w + 6, rect.y - mHeaderMargin + rect.h );
	
	int maxA = pt2.X + 1;
	int maxB = pt2.Y + 1;
	
	if (maxA > (DimK << 1)) maxA = (DimK << 1);
	if (maxB > DimL) maxB = DimL;
				
	for (B = minB; B <= maxB; B++)
    {
		for (A = minA; A < maxA; A++) 
		{
			if (IsRealCell(A,B))
            {
				DrawCell(g, A, B, &rect);
            }
		}  
    }
}


IntPoint HexGameField::ABtoXY (const int A, const int B) const
{
	int X, Y ;

	ABtoXY (A, B, X, Y);
	
	return IntPoint (X, Y);
}

void HexGameField::ABtoXY (int A, int B, int & X, int & Y) const
{	
	X = (A << 4) - 4;  // -6+2, 2 as LeftTopX
	Y = 18 * B + 2;   //  2 as LeftTopY }
	
	if ((IsCornerSuit) ^ ((A & 1)==0)) Y = Y - 9;
	
	X += left_field_shift;
	Y += top_field_shift;
}

IntPoint HexGameField::XYtoAB (int X, int Y) const
{
	int A, B;
	
	XYtoAB (X, Y, A, B);

	return IntPoint(A, B);
}

void HexGameField::XYtoAB (int X, int Y, int & A, int & B) const
{
	int T;
	
	X -= left_field_shift;
	Y -= top_field_shift;
	
	X = X - 2; // 2 as TopLeftX
	Y = Y - 2 - 2; // 2 as TopLeftY
	
    // here (X=0, Y=0) is TopLeftCorner of surrounder
	A = (X + 16) / 16;
	T = 0;
	
    if (IsCornerSuit ^ ((A & 1)==0)) T = -9;
	T = Y - T;
	// T now is a distance to the center of virtual (A, 0) cell
	B = (T + 9) / 18;
	
    if ((X & 15) > 3)
    {
		return ;
    }
	
    X = X - (A << 4) + 6;
	Y = Y - 18 * B;
	if (IsCornerSuit ^ ((A & 1)==0)) Y = Y + 9;
	
	if ((Y<0) && ((X<<1) + Y < -21)) 
    {
		if (((A & 1)==0) ^ IsCornerSuit) B--;
		A--;
	}
	else if((Y>=0) && ((X<<1) - Y < -21)) 
    {
		if (((A & 1)==1) ^ IsCornerSuit) B++;
		A--;
	}
}

bool HexGameField::IsRealCell (int A, int B) const
{
	bool Result = false;
	bool FullField = true;
	
	if (FullField)
	{
		if (!IsCornerSuit)
			Result = (A > 0) && (B > 0) && (A < (DimK << 1)) &&
			((B < DimL) || (((A & 1) == 0) && (B == DimL)));
		else
			Result = (A > 0) && (B > 0) && (A < (DimK << 1)) &&
			((B < DimL) || (((A & 1) == 1) && (B == DimL))) &&
			(! (((A==1) || (A==(DimK << 1) - 1)) && ((B==1) || (B==DimL))));
	}
	else 
    {
		if (DimK < DimL - 1)
			Result =  (A > 0) && (A < (DimK << 1)) &&
			(A - (B << 1) < DimK) && (A + (B << 1) > DimK) &&
			(A - (B << 1) >= DimK - (DimL << 1)) &&
			(A + (B << 1) <= DimK + (DimL << 1)) ;
		else
			Result =  (A > 0) && (A < (DimK << 1)) &&
			(B > 0) &&
			((B < DimL) ||
			 ((B == DimL) && (((A & 1) == 0) ^ IsCornerSuit))) &&
			(A - (B << 1) >= - DimL - 2) && (A + (B << 1) > DimL - 2) &&
			(A - (B << 1) <  - DimL + (DimK << 1) + 2) &&
			(A + (B << 1) <= DimL + (DimK << 1) + 2) ;
	}
	
	return Result;
}

void HexGameField::DrawCellOutline (CGContextRef g, int A, int B) const
{
	int X, Y, B1;

	B1 = ((IsCornerSuit ^ (A & 1)) == 0) ? B : B + 1; // B for Right-Lower Cell

	if ((!IsRealCell (A, B + 1)) ||
		(!IsRealCell (A + 1, B1)) || (!IsRealCell (A + 1, B1 - 1))) 
	{
		IntPoint pt = ABtoXY (A, B);
			
		X = pt.X + 2;
		Y = pt.Y + 46;
						
		if (!IsRealCell(A, B + 1)) 
		{
			drawBitmap(g, X - 1, Y, 22, 19, hex_bottom, true);
		}
		if (!IsRealCell(A + 1, B1)) 
		{
			drawBitmap(g, X - 1, Y, 22, 19, hex_right_bottom, true);			
		}
		if (!IsRealCell(A + 1, B1 - 1)) 
		{        
			drawBitmap(g, X - 1, Y, 22, 19, hex_right_top, true);
		}
	
	}
}

void HexGameField::DrawCell (CGContextRef g, int A, int B, CGRect * rect) const
{
	if (g == NULL || rect == NULL) 
        return;
		
	int X, Y;	
	ABtoXY(A, B, X, Y);
		
	X += 2;
	Y += 46;
		
	CGRect rect2 = { X - 1, Y, 22, 19 };
	
    if (Graphics::RectIntersectsRect(*rect, rect2) == false)
		return;
	
	int State = Cell_State (A, B);
	
	bool Mined = IsMined(A, B);
	
	bool draw_outline = false;

#ifdef USE_BATCHED
	if (fBatched && IsBatched (A, B) && (State == csClosed))
	{
		drawBitmap(g, X - 1, Y, 22, 19, openedBmp, true);
		DrawCellOutline (g, A, B);
		return;
	}
#endif 

    if (fCaptured && A == PressedA && B == PressedB && State == csClosed)
	{
		drawBitmap(g, X - 1, Y, 22, 19, openedBmp, true);
		DrawCellOutline(g, A, B);
		return;
	}
	
	if (Mined && bCollapse && ((State == csClosed) || (State == csQuest)))
	{
		drawBitmap(g, X - 1, Y, 22, 19, openedBmp, true);	
		drawBitmap(g, X, Y, 20, 18, mineBmp, true);
		draw_outline = true;
	}
	else if ((State == csMarked) && !Mined && bCollapse)      
	{
		// error
		drawBitmap(g, X - 1, Y, 22, 19, openedBmp, true);	
		drawBitmap(g, X, Y, 20, 18, errorBmp, true);
		draw_outline = true;	
	}
	else if ((State == csOpened) && Mined && bCollapse)
	{
		drawBitmap(g, X - 1, Y, 22, 19, blastBmp, true);
		drawBitmap(g, X, Y, 20, 18, mineBmp, true);	
		draw_outline = true;	
	}
	else if (State == csOpened)
	{
		drawBitmap(g, X - 1, Y, 22, 19, openedBmp, true);
		draw_outline = true;
		
        int mCount = getMines(A,B) ;
		if (mCount > 0)
		{
			drawCellDigit (g, X, Y, mCount);
		}
	}
	else
	{      
        // closed, marked, quest
		drawBitmap(g, X, Y, 20, 18, closedBmp, true);        
		if (State == csMarked)
			drawBitmap(g, X + 6, Y + 4, 8, 10, flagBmp, true);
		else if (State == csQuest)
			drawBitmap(g, X + 6, Y + 4, 8, 10, qmarkBmp, true);        
	}  
	
	if (draw_outline)
	{
	    DrawCellOutline (g, A, B);
	}			
}

int HexGameField::getInitailMinesCount (int total_cells, GameLevel level)
{
	int Count = total_cells;

    if (level == LEVEL_BEGINNER)
        Count = Count / 6;
	else if (level == LEVEL_INTERMED)
        Count = Count / 5;
	else if (level == LEVEL_ADVANCED)
        Count = Count / 4;  

	return Count;
}


void HexGameField::DistributeMines() 
{
	bool FullField = true;
	int A, B, RParam;
	RParam = (DimK << 1) - 1;
    int Count = 0;
	
	for (A = 1; A <= RParam; A++)
    {
		for (B = 1; B <= DimL; B++) 
        {
			IsMined_Set(A, B, false) ;
			Mines_Set(A, B, 0);
			Cell_Set(A, B, csClosed);
			IsBatched_Set(A, B, false);
		}
    }
	
	if (FullField) 
    {
		Count = ((DimK * DimL) << 1) - DimK - DimL;
		if (IsCornerSuit) Count = Count - 3;
	}
	else 
    {
		if (DimK < DimL - 1)
			Count = ((DimL << 1) - DimK + 1) * DimK - DimL;
		else 
            Count = DimL * ((DimK << 1) - DimL + 5) - DimK - 8;
	}

	iFreeCount = Count;

    iMinesCount = getInitailMinesCount (iFreeCount, m_game_level);
		
	iFreeCount = iFreeCount - iMinesCount;
	
	srand (time(NULL));

    Count = iMinesCount;
	
	while (Count > 0) 
	{
		A = nextInt (RParam) + 1;
		B = nextInt (DimL) + 1;
		
		if (!IsMined(A,B) && IsRealCell(A,B)) 
        {
			IsMined_Set (A, B, true);
			IncMines (A, B - 1);
			IncMines (A, B + 1);
			IncMines (A - 1, B);
			IncMines (A + 1, B);
			B = ((IsCornerSuit ^ (A & 1))==0) ? B - 1 : B + 1;
			IncMines (A - 1, B);
			IncMines (A + 1, B);
			Count--;
		}
	}
}

void HexGameField::MoveMineSomewhereElse (int A1, int B1)
{
    IsMined_Set (A1, B1, false);
	SubMine (A1, B1 - 1);
	SubMine (A1, B1 + 1);
	SubMine (A1 - 1, B1);
	SubMine (A1 + 1, B1);
	int B2 = ((IsCornerSuit ^ (A1 & 1))==0) ? B1 - 1 : B1 + 1;
	SubMine (A1 - 1, B2);
	SubMine (A1 + 1, B2);
    
    int RParam = (DimK << 1) - 1;
	
	srand (time(NULL));
	
    while (true) 
	{
        int A = nextInt (RParam) + 1;  
        int B = nextInt (DimL) + 1;
        
        if (A1 == A || B1 == B) continue;
        
        if (!IsMined(A, B) && IsRealCell(A,B)) 
		{
            IsMined_Set(A, B,true);
			
            IncMines (A, B - 1);
            IncMines (A, B + 1);
            IncMines (A - 1, B);
            IncMines (A + 1, B);
            B = ((IsCornerSuit ^ (A & 1))==0) ? B - 1 : B + 1;
            IncMines (A - 1, B);
            IncMines (A + 1, B);
            break;
		}        
	}    
}        


bool HexGameField::autoMark (CGContextRef g, int A, int B)
{
    int mines = getMines(A, B);

    if (mines == 0) 
        return true;
    
    if (mines > 0)
	{
        int cnt = 0;
        // count closed mines around (A,B)
        if (CellIsClosed(A, B - 1)) cnt++;
        if (CellIsClosed(A, B + 1)) cnt++;
        if (CellIsClosed(A - 1, B)) cnt++;
        if (CellIsClosed(A + 1, B)) cnt++;
        
        int B2 = ((IsCornerSuit ^ (A & 1)) == 0) ? B-1 : B+1;
		
        if (CellIsClosed(A - 1, B2)) cnt++;
        if (CellIsClosed(A + 1, B2)) cnt++;
		
        if (cnt == mines)
		{
            int sub = 0;
            if (CellIsClosed(A, B - 1))
			{
                if (Cell_State(A, B - 1) != csMarked) 
				{
					sub++;
					Cell_Set(A, B - 1, csMarked);
				}
			}
            
            if (CellIsClosed(A, B + 1))
			{
                if (Cell_State(A, B + 1) != csMarked) 
				{
					sub++;
					Cell_Set(A, B + 1, csMarked);
				}
			}
            
            if (CellIsClosed(A - 1, B))
			{
                if (Cell_State(A - 1, B) != csMarked) 
				{
					sub++;
					Cell_Set(A - 1, B, csMarked);
				}
			}
            
            if (CellIsClosed (A + 1, B))
			{
                if (Cell_State (A + 1, B) != csMarked) 
				{
					sub++;
					Cell_Set (A + 1, B, csMarked);
				}
			}
            
            if (CellIsClosed (A - 1, B2))
			{
                if (Cell_State (A - 1, B2) != csMarked) 
				{
					sub++;
					Cell_Set (A - 1, B2, csMarked);
				}
			}
            
            if (CellIsClosed (A + 1, B2))
			{
                if (Cell_State (A + 1, B2) != csMarked) 
				{
					sub++;
					Cell_Set (A + 1, B2,csMarked);    
				}
			}
            
            iMinesCount -= sub;
            MinesChange(g, nullptr);
            return true; 
		}
    }
    return false;
}

void HexGameField::openCellsOnVictory(CGContextRef g) 
{
	int A,B;
	
	for (B = 1; B <= DimL; B++)
    {
		for (A = 1; A < (DimK << 1); A++)
        {
			if (IsRealCell(A,B)) 
            {
				if (Cell_State (A,B) != csMarked && IsMined(A,B)) 
                {
					Cell_Set(A, B, csMarked);
					DrawCell(g, A, B, nullptr);
				}
			}
        }
    }
}

void HexGameField::CallReaction(CGContextRef g,int A, int B) 
{
	if (!IsRealCell (A, B)) 
        return;
    
	if (Cell_State(A,B) == csOpened || Cell_State(A,B) == csMarked) 
        return;
	
	Cell_Set (A, B, csOpened);
	
	iFreeCount--;
	
    DrawCell (g, A, B, nullptr);
	
    if (getMines(A, B) == 0) 
    {
		CallReaction (g, A, B - 1);
		CallReaction (g, A, B + 1);
		CallReaction (g, A - 1, B);
		CallReaction (g, A + 1, B);
		int B2 = ((IsCornerSuit ^ (A & 1)) == 0) ? B - 1 : B + 1;
		CallReaction (g, A - 1, B2);
		CallReaction (g, A + 1, B2);
	}
	else
    {  
		opened_cells.insert( {A, B} );  
    }
}

#ifdef USE_BATCHED

void HexGameField::DiscloseBatch (CGContextRef g) 
{
    int A = BatchedA;
    int B = BatchedB;
    
    BatchedA = -1;
    BatchedB = -1;
  
    if (IsRealCell(A, B)) 
    {
        int B1 = (IsCornerSuit ^ ((A & 1) == 0)) ? B : B + 1;

        int MarkedCnt = 0;
        bool bMayCollapse = false;
    
        if (Cell_State (A,B) == csOpened) 
        {
            ProbeCell (A, B - 1, MarkedCnt, bMayCollapse);
            ProbeCell (A, B + 1, MarkedCnt, bMayCollapse);
            ProbeCell (A - 1, B1 - 1,MarkedCnt, bMayCollapse);
            ProbeCell (A - 1, B1, MarkedCnt, bMayCollapse);
            ProbeCell (A + 1, B1 - 1,MarkedCnt, bMayCollapse);
            ProbeCell (A + 1, B1, MarkedCnt, bMayCollapse);
        }

        if ((getMines (A,B) != MarkedCnt) || (Cell_State (A,B) != csOpened)) 
        {
            RestoreBatched (g, A, B);
            RestoreBatched (g, A, B - 1);
            RestoreBatched (g, A, B + 1);
            RestoreBatched (g, A-1, B1 - 1);
            RestoreBatched (g, A - 1, B1);
            RestoreBatched (g, A + 1, B1 - 1);
            RestoreBatched (g, A + 1, B1);
        }

        else 
        {

            bCollapse = bMayCollapse;
            DiscloseBatched (g, A, B - 1);
            DiscloseBatched (g, A, B + 1);
            DiscloseBatched (g, A - 1, B1 - 1);
            DiscloseBatched (g, A - 1, B1);
            DiscloseBatched (g, A + 1, B1 - 1);
            DiscloseBatched (g, A + 1, B1);

        }
    }
}

void HexGameField::HandleBatchPressing(CGContextRef g, int A, int B) 
{
    int B1;
    
    if (IsRealCell(A, B)) 
    {
        B1 = (IsCornerSuit ^ ((A & 1) == 0)) ? B : B + 1;

        ToggleBatched (A, B);
        ToggleBatched (A, B - 1);
        ToggleBatched (A, B + 1);
        ToggleBatched (A - 1, B1 - 1);
        ToggleBatched (A - 1, B1);
        ToggleBatched (A + 1, B1 - 1);
        ToggleBatched (A + 1, B1);
    }

    if (IsRealCell(BatchedA, BatchedB)) 
    {
        B1 = (IsCornerSuit ^ ((BatchedA & 1) == 0)) ? BatchedB : BatchedB+1;

        RestoreBatched (g, BatchedA, BatchedB);
        RestoreBatched (g, BatchedA, BatchedB-1);
        RestoreBatched (g, BatchedA, BatchedB+1);
        RestoreBatched (g, BatchedA - 1, B1 - 1);
        RestoreBatched (g, BatchedA - 1, B1);
        RestoreBatched (g, BatchedA + 1, B1 - 1);
        RestoreBatched (g, BatchedA + 1, B1);
    }

    if (IsRealCell(A, B)) 
    {
        B1 = (IsCornerSuit ^ ((A & 1) == 0)) ? B : B + 1;

        DrawBatched (A, B);
        DrawBatched (A, B - 1);
        DrawBatched (A, B + 1);
        DrawBatched (A - 1, B1 - 1);
        DrawBatched (A - 1, B1);
        DrawBatched (A + 1, B1 - 1);
        DrawBatched (A + 1, B1);
    }
  
    BatchedA = A;
    BatchedB = B;
 }


#endif // BATCHED



