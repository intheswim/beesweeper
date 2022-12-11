/*
 *  SquareGameField.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

#include "GenericField.h"
#include "GameClasses.h"
#include "image.h"

class SquareGameField : public GenericField
{
	public:
	SquareGameField(int w, int h, Uint32 userEventID, SDL_Renderer *r, bool highlight_cells);
	~SquareGameField();

	// forbid coping since we have raw pointer in class.
    SquareGameField(const SquareGameField &) = delete;
    SquareGameField& operator = (const SquareGameField &) = delete;

	char gameType() const { return GAME_SQUARES; }

	protected:

	bool IsRealCell(int A, int B) const;
	void DrawCellOutline(CGContextRef g, int A, int B) const;
	void DrawCell(CGContextRef g, int A, int B, CGRect *rect) const;
	
	IntPoint ABtoXY (const int A, const int B) const;
	void ABtoXY (int A, int B, int & X, int & Y) const;

	IntPoint XYtoAB (int X, int Y) const;
	void XYtoAB (int X, int Y, int & A, int & B) const;

	void CallReaction (CGContextRef g, int A, int B);
	void MoveMineSomewhereElse (int A, int B);

	bool autoMark(CGContextRef g, int A, int B);
	void openCellsOnVictory(CGContextRef g);

	bool initImages(SDL_Renderer *, bool highlight_cells);
	void freeImages();

	void reloadImages (SDL_Renderer *g, bool scale) ;

	void closedCellImage(SDL_Renderer *r, bool highlight_cells);

	private:

	int left_field_shift = 0;
	int top_field_shift = 0;

	Image *openedBmp = nullptr;

	Image *blastBmp = nullptr;
	Image *closedBmp = nullptr;

	Image *bottom_square = nullptr, *right_square = nullptr;

	void PaintCells(CGContextRef g, CGRect rect) const;

	static void getDimensions(int width, int height, int &dimK, int &dimL);

	void setupField();

	void DistributeMines();

	static int getInitailMinesCount (int total_cells, GameLevel level);

#ifdef USE_BATCHED
	void DiscloseBatch(CGContextRef g);
	void HandleBatchPressing(CGContextRef g, int A, int B) ;
#endif 

};
