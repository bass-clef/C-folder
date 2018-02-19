
#ifndef TETRIS_H
#define TETRIS_H

#include <windows.h>

bool GameInit();
bool GameExit();
bool GameCalc();
bool GameDraw(HDC hDC);

class Tetrimino{
	int *type;
public:
	Tetrimino(){
		type = NULL;
	}
	~Tetrimino(){
		if (type != NULL) delete[] type;
	}
	void init(int num, int xField, int yField){
		int xFieldm = xField-1, yFieldm = yField-1;
		type = new int[num];
		for(int j = 0; j < yField; j++)
		for(int i = 0; i < xField; i++){
			if (i == 0 || j == 0) type[j*xField+i] = -1;
			else if (xFieldm == i || yFieldm == j) type[j*xField+i] = -1;
			else type[j*xField+i] = 0;
		}
	}
	void set(int num, int value){
		type[num] = value;
	}
	int get(int num){
		return type[num];
	}
	void move(int to, int from){
		int i = type[to];
		type[to] = type[from];
		type[from] = i;
	}
};

#endif
