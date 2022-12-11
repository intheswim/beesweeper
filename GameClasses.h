/*
 *  GameClasses.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once

enum { csClosed = 0, csOpened, csMarked, csQuest };
enum FaceState { fsPressed = 0, fsSmile, fsWeep, fsShout, fsBoss  };

struct cellInfo
{
public:
	int A, B ;
	cellInfo (int _A, int _B) : A(_A), B(_B) { }
};


struct CellElement 
{	
	int State = 0;
	bool IsMined = false;
	int MineCnt = 0; // number of mines surrounding given cell.
	bool Batched = false;
	
	CellElement() { }
};

struct IntPoint
{
public:
	int X = 0, Y = 0;
	IntPoint() { }
	IntPoint(int x, int y) : X(x), Y(y) {}
};
