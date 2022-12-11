/*
 *  SquareGameField.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "SquareGameField.h"
#include "graphics.h"

SquareGameField::SquareGameField(int w, int h, Uint32 userEventID, SDL_Renderer *r, bool highlight_cells) : GenericField(w, h, userEventID, r)
{
	m_highlight = highlight_cells;

	if (initImages(r, highlight_cells))
	{
		setupField();
	}
}

////////////////////////////////////////////////////////////////////////////////

SquareGameField::~SquareGameField()
{
	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "SquaresGame destructor entered");

	freeImages();

	SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "SquaresGame destructor exit\n");
}

void SquareGameField::getDimensions (int width, int height, int &dimK, int &dimL)
{
	int area = width - (mBevelWidth * 2); // 12 is an "edge" (on one side)
	dimK = (area - 2) / 16;

	area = height - mBevelWidth - mHeaderMargin; // 12 is bottom edge, 55 is top part
	dimL = (area - 2) / 16;
}

void SquareGameField::setupField()
{
	getDimensions(width, height, DimK, DimL);

	reallocCells (DimK * DimL);

	int hex_area = DimK * 16;

	left_field_shift = (width - hex_area) / 2;

	left_field_shift += (getScreenWidth() - width) / 2;

	top_field_shift = mHeaderMargin + (height - mBevelWidth - mHeaderMargin - DimL * 16) / 2;

	top_field_shift += (getScreenHeight() - height) / 2;
}

bool SquareGameField::initImages(SDL_Renderer *r, bool highlight_cells)
{
	blastBmp = new Image(r, "blast_square.png");

	openedBmp = new Image(r, "opened_square.png");

	initCellDigitsBitmaps(r, CELL_DIGITS_MAX);

	closedCellImage(r, highlight_cells);

	bottom_square = new Image(r, "bottom_square.png");
	right_square  = new Image(r, "right_square.png");

	return true;
}

void SquareGameField::closedCellImage(SDL_Renderer *r, bool highlight_cells)
{
	if (closedBmp != nullptr)
	{
		delete closedBmp;
		closedBmp = nullptr;
	}

	closedBmp = (highlight_cells ? new Image(r, "closed_square_blue.png") : new Image(r, "closed_square.png"));
}

////////////////////////////////////////////////////////////////////////////////

void SquareGameField::freeImages()
{
	delete blastBmp;
	blastBmp = nullptr;

	delete openedBmp;
	openedBmp = nullptr;

	delete closedBmp;
	closedBmp = nullptr;

	delete bottom_square;
	bottom_square = nullptr;

	delete right_square;
	right_square = nullptr;
}

void SquareGameField::reloadImages (SDL_Renderer *g, bool scale)
{
	this->reinitImages (g, scale);

	blastBmp->reload(g, scale);

	openedBmp->reload (g, scale);

	freeCellDigitsBitmaps ();
	initCellDigitsBitmaps(g, CELL_DIGITS_MAX, scale);

	closedBmp->reload(g, scale);

	bottom_square->reload (g, scale);
	right_square->reload (g, scale);
}

void SquareGameField::PaintCells(CGContextRef g, CGRect rect) const
{
	int minA, minB;

	XYtoAB(rect.x - mBevelWidth, rect.y - mHeaderMargin, minA, minB );

	if (minA < 1)
		minA = 1;
	if (minB < 1)
		minB = 1;

	int maxA, maxB;

	XYtoAB(rect.x - mBevelWidth + rect.w, rect.y - mHeaderMargin + rect.h, maxA, maxB);

	if (maxA > DimK)
		maxA = DimK;
	if (maxB > DimL)
		maxB = DimL;


	for (int B = minB; B <= maxB; B++)
	{
		for (int A = minA; A <= maxA; A++)
		{
			if (IsRealCell(A, B))
				DrawCell(g, A, B, &rect);
		}
	}
}

IntPoint SquareGameField::ABtoXY (const int A, const int B) const
{
	int X, Y ;

	ABtoXY (A, B, X, Y);

	return IntPoint (X, Y);
}

void SquareGameField::ABtoXY (int A, int B, int & X, int & Y) const
{
	X = (A << 4) - 16;
	Y = (B << 4) - 16;

	X += left_field_shift;
	Y += top_field_shift;
}

IntPoint SquareGameField::XYtoAB (int X, int Y) const
{
	int A, B;

	XYtoAB (X, Y, A, B);

	return IntPoint(A, B);
}

void SquareGameField::XYtoAB (int X, int Y, int & A, int & B) const
{
	Y += mHeaderMargin;
	X += mBevelWidth;

	X -= left_field_shift;
	Y -= top_field_shift;

	A = (X + 16) >> 4;
	B = (Y + 16) >> 4;
}

bool SquareGameField::IsRealCell(int A, int B) const
{
	return (A > 0 && A <= DimK && B > 0 && B <= DimL);
}

void SquareGameField::DrawCellOutline(CGContextRef g, int A, int B) const
{
	if ((!IsRealCell(A, B + 1)) || (!IsRealCell(A + 1, B)))
	{
		int X, Y;

		ABtoXY(A, B, X, Y);

		if (!IsRealCell(A, B + 1))
		{
			drawBitmap(g, X, Y, 17, 17, bottom_square, true);
		}
		if (!IsRealCell(A + 1, B))
		{
			drawBitmap(g, X, Y, 17, 17, right_square, true);
		}
	}
}

void SquareGameField::DrawCell(CGContextRef g, int A, int B, CGRect *rect) const
{
	if (g == NULL || rect == NULL)
		return;

	int X, Y;
	ABtoXY(A, B, X, Y);

	CGRect rect2 = { X, Y, 16, 16 };

	if (Graphics::RectIntersectsRect(*rect, rect2) == false)
		return;

	int State = Cell_State(A, B);

	bool Mined = IsMined(A, B);

#ifdef USE_BATCHED
	if (fBatched && IsBatched (A,B) && (State == csClosed))
	{
		drawBitmap(g, X, Y, 16, 16, openedBmp, true);
		DrawCellOutline(g, A, B);
		return;
	}
#endif 

	if (fCaptured && A == PressedA && B == PressedB && State == csClosed)
	{
		drawBitmap(g, X, Y, 16, 16, openedBmp, true);
		DrawCellOutline(g, A, B);
		return;
	}

	if (Mined && bCollapse && ((State == csClosed) || (State == csQuest)))
	{
		drawBitmap(g, X, Y, 16, 16, openedBmp, true);
		drawBitmap(g, X - 2, Y - 1, 20, 18, mineBmp, true);
	}
	else if ((State == csMarked) && !Mined && bCollapse)
	{
		// error
		drawBitmap(g, X, Y, 16, 16, openedBmp, true);
		drawBitmap(g, X - 2, Y - 1, 20, 18, errorBmp, true);
	}
	else if ((State == csOpened) && Mined && bCollapse)
	{
		drawBitmap(g, X, Y, 16, 16, blastBmp, true);
		drawBitmap(g, X - 2, Y - 1, 20, 18, mineBmp, true);
	}
	else if (State == csOpened)
	{
		drawBitmap(g, X, Y, 16, 16, openedBmp, true);
		int mCount = getMines(A, B);

		if (mCount > 0)
		{
			drawCellDigit(g, X - 2, Y - 1, mCount);
		}
	}
	else
	{
		// closed, marked, quest
		drawBitmap(g, X, Y, 16, 16, closedBmp, true);
		if (State == csMarked)
			drawBitmap(g, X + 4, Y + 3, 8, 10, flagBmp, true);
		else if (State == csQuest)
			drawBitmap(g, X + 4, Y + 3, 8, 10, qmarkBmp, true);
	}

	if (true)
	{
		DrawCellOutline(g, A, B);
	}
}

int SquareGameField::getInitailMinesCount (int total_cells, GameLevel level)
{
	int Count = total_cells;

	if (level == LEVEL_BEGINNER)
		Count = Count * 10 / 80;
	else if (level == LEVEL_INTERMED)
		Count = Count * 40 / 256;
	else if (level == LEVEL_ADVANCED)
		Count = Count * 100 / 480;

	return Count;
}

void SquareGameField::DistributeMines()
{
	int A, B;

	for (A = 1; A <= DimK; A++)
	{
		for (B = 1; B <= DimL; B++)
		{
			IsMined_Set (A, B, false);
			Mines_Set (A, B, 0);
			Cell_Set (A, B, csClosed);
			IsBatched_Set (A, B, false);
		}
	}

	iFreeCount = (DimK * DimL);

	iMinesCount = getInitailMinesCount (iFreeCount, m_game_level);

	iFreeCount = iFreeCount - iMinesCount;

	srand((unsigned int)time(NULL));

	int Count = iMinesCount;

	while (Count > 0)
	{
		A = nextInt(DimK) + 1;
		B = nextInt(DimL) + 1;

		if (!IsMined(A, B) && IsRealCell(A, B))
		{
			IsMined_Set(A, B, true);

			IncMines(A, B - 1);
			IncMines(A, B + 1);
			IncMines(A - 1, B);
			IncMines(A + 1, B);

			IncMines(A - 1, B - 1);
			IncMines(A + 1, B + 1);

			IncMines(A + 1, B - 1);
			IncMines(A - 1, B + 1);

			Count--;
		}
	}
}

void SquareGameField::MoveMineSomewhereElse(int A, int B)
{
	IsMined_Set(A, B, false);
	SubMine(A, B - 1);
	SubMine(A, B + 1);
	SubMine(A - 1, B);
	SubMine(A + 1, B);

	SubMine(A - 1, B + 1);
	SubMine(A - 1, B - 1);

	SubMine(A + 1, B + 1);
	SubMine(A + 1, B - 1);

	srand((unsigned int)time(NULL));

	while (true)
	{
		int A1 = nextInt(DimK) + 1;
		int B1 = nextInt(DimL) + 1;

		if (A1 == A || B1 == B)
			continue;

		if (!IsMined(A1, B1) && IsRealCell(A1, B1))
		{
			IsMined_Set(A1, B1, true);

			IncMines(A1, B1 - 1);
			IncMines(A1, B1 + 1);
			IncMines(A1 - 1, B1);
			IncMines(A1 + 1, B1);
			IncMines(A1 - 1, B1 + 1);
			IncMines(A1 - 1, B1 - 1);
			IncMines(A1 + 1, B1 + 1);
			IncMines(A1 + 1, B1 - 1);

			break;
		}
	}
}

bool SquareGameField::autoMark(CGContextRef g, int A, int B)
{
	int mines = getMines(A, B);

	if (mines == 0)
		return true;

	if (mines > 0)
	{
		int cnt = 0;
		// count closed mines around (A,B)
		if (CellIsClosed(A, B - 1))
			cnt++;
		if (CellIsClosed(A, B + 1))
			cnt++;
		if (CellIsClosed(A - 1, B))
			cnt++;
		if (CellIsClosed(A + 1, B))
			cnt++;

		if (CellIsClosed(A - 1, B - 1))
			cnt++;
		if (CellIsClosed(A + 1, B - 1))
			cnt++;

		if (CellIsClosed(A - 1, B + 1))
			cnt++;
		if (CellIsClosed(A + 1, B + 1))
			cnt++;

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

			if (CellIsClosed(A + 1, B))
			{
				if (Cell_State(A + 1, B) != csMarked)
				{
					sub++;
					Cell_Set(A + 1, B, csMarked);
				}
			}

			if (CellIsClosed(A - 1, B - 1))
			{
				if (Cell_State(A - 1, B - 1) != csMarked)
				{
					sub++;
					Cell_Set(A - 1, B - 1, csMarked);
				}
			}

			if (CellIsClosed(A + 1, B - 1))
			{
				if (Cell_State(A + 1, B - 1) != csMarked)
				{
					sub++;
					Cell_Set(A + 1, B - 1, csMarked);
				}
			}

			if (CellIsClosed(A - 1, B + 1))
			{
				if (Cell_State(A - 1, B + 1) != csMarked)
				{
					sub++;
					Cell_Set(A - 1, B + 1, csMarked);
				}
			}

			if (CellIsClosed(A + 1, B + 1))
			{
				if (Cell_State(A + 1, B + 1) != csMarked)
				{
					sub++;
					Cell_Set(A + 1, B + 1, csMarked);
				}
			}

			iMinesCount -= sub;

			MinesChange(g, nullptr);

			return true;
		}
	}
	return false;
}

void SquareGameField::openCellsOnVictory(CGContextRef g)
{
	int A, B;

	for (B = 1; B <= DimL; B++)
	{
		for (A = 1; A <= DimK; A++)
		{
			if (IsRealCell(A, B))
			{
				if (Cell_State(A, B) != csMarked && IsMined(A, B))
				{
					Cell_Set(A, B, csMarked);
					DrawCell(g, A, B, nullptr);
				}
			}
		}
	}
}

void SquareGameField::CallReaction(CGContextRef g, int A, int B)
{
	if (!IsRealCell(A, B))
		return;

	if (Cell_State(A, B) == csOpened || Cell_State(A, B) == csMarked)
		return;

	Cell_Set(A, B, csOpened);

	iFreeCount--;
	DrawCell(g, A, B, nullptr);

	if (getMines(A, B) == 0)
	{
		CallReaction(g, A, B - 1);
		CallReaction(g, A, B + 1);
		CallReaction(g, A - 1, B);
		CallReaction(g, A + 1, B);

		CallReaction(g, A - 1, B - 1);
		CallReaction(g, A + 1, B + 1);

		CallReaction(g, A - 1, B + 1);
		CallReaction(g, A + 1, B - 1);
	}
	else
	{
		opened_cells.insert({A, B});
	}
}

#ifdef USE_BATCHED

void SquareGameField::DiscloseBatch(CGContextRef g)
{   
	int A = BatchedA;
    int B = BatchedB;

    BatchedA = -1;
    BatchedB = -1;

  	if (IsRealCell(A, B)) 
	{	
		int MarkedCnt = 0;
    	bool bMayCollapse = false;

    	if (Cell_State (A,B) == csOpened) 
		{
      		ProbeCell (A, B - 1, MarkedCnt, bMayCollapse);
      		ProbeCell (A, B + 1, MarkedCnt, bMayCollapse);
      		ProbeCell (A - 1, B - 1, MarkedCnt, bMayCollapse);
      		ProbeCell (A - 1, B, MarkedCnt, bMayCollapse);
      		ProbeCell (A + 1, B - 1, MarkedCnt, bMayCollapse);
      		ProbeCell (A + 1, B, MarkedCnt, bMayCollapse);

			ProbeCell (A + 1, B + 1, MarkedCnt, bMayCollapse);
			ProbeCell (A - 1, B + 1, MarkedCnt, bMayCollapse);
     	}

    	if ((getMines(A,B) != MarkedCnt) || (Cell_State(A,B) != csOpened)) 
		{
      		RestoreBatched (g, A, B);
      		RestoreBatched (g, A, B - 1);
      		RestoreBatched (g, A, B + 1);
      		RestoreBatched (g, A - 1, B - 1);
      		RestoreBatched (g, A - 1, B);
      		RestoreBatched (g, A + 1, B - 1);
      		RestoreBatched (g, A + 1, B);

			RestoreBatched (g, A + 1, B + 1);
			RestoreBatched (g, A - 1, B + 1);
     	}

    	else 
		{
      		bCollapse = bMayCollapse;

      		DiscloseBatched (g, A, B - 1);
      		DiscloseBatched (g, A, B + 1);
      		DiscloseBatched (g, A - 1, B - 1);
            DiscloseBatched (g, A - 1, B);
            DiscloseBatched (g, A + 1, B - 1);
            DiscloseBatched (g, A + 1, B);

			DiscloseBatched (g, A + 1, B + 1);
			DiscloseBatched (g, A - 1, B + 1);
    	}
  	}
}

void SquareGameField::HandleBatchPressing (CGContextRef g, int A, int B) 
{  	
	if (IsRealCell(A,B)) 
	{
    	ToggleBatched (A, B);
    	ToggleBatched (A, B - 1);
    	ToggleBatched (A, B + 1);
    	ToggleBatched (A - 1, B - 1);
    	ToggleBatched (A - 1, B);
    	ToggleBatched (A + 1, B - 1);
    	ToggleBatched (A + 1, B);

		ToggleBatched (A + 1, B + 1);
		ToggleBatched (A - 1, B + 1);
  	}

  	if (IsRealCell(BatchedA, BatchedB)) 
	{
    	RestoreBatched (g, BatchedA, BatchedB);
    	RestoreBatched (g, BatchedA, BatchedB - 1);
    	RestoreBatched (g, BatchedA, BatchedB + 1);
    	RestoreBatched (g, BatchedA - 1, BatchedB - 1);
    	RestoreBatched (g, BatchedA - 1, BatchedB);
    	RestoreBatched (g, BatchedA + 1, BatchedB - 1);
    	RestoreBatched (g, BatchedA + 1, BatchedB);

		RestoreBatched (g, BatchedA + 1, BatchedB + 1);
		RestoreBatched (g, BatchedA - 1, BatchedB + 1);
    }

    if (IsRealCell(A,B)) 
	{
		// this name is somewhat confusing : it sets batch to true when it's not set.
    	DrawBatched (A, B);
    	DrawBatched (A, B - 1);
    	DrawBatched (A, B + 1);
    	DrawBatched (A - 1, B - 1);
    	DrawBatched (A - 1, B);
    	DrawBatched (A + 1, B - 1);
    	DrawBatched (A + 1, B);

		DrawBatched (A + 1, B + 1);
		DrawBatched (A - 1, B + 1);
    }

  	BatchedA = A;
  	BatchedB = B;
 }


#endif // BATCH
