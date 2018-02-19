
#include "tetris.h"
#include "app.h"

namespace {
	const int mwx		= 440;
	const int mwy		= 480;
	const int vx		= 100;
	const int vy		= 50;
	const int zSize		= 15;
	const int xField	= 10+2;
	const int yField	= 20+4;
	const int zField	= xField * yField;
	Tetrimino tet;
	// 0:‚È‚µ -1:•Ç 1<:‰½‚©‚ ‚é
}

bool GameInit(){
	tet.init(zField, xField, yField);
	return true;
}

bool GameExit(){
	return true;
}

bool GameCalc(){
	return true;
}

bool GameDraw(HDC hDC){
	String str(16);
	int x1, y1, x2, y2;

//	DrawRect(hDC, 0, 0, mwx, mwy, DR_FILL);

	for(int j = 0; j < yField; j++)
		for(int i = 0; i < xField; i++){
			x1 = (i-1)*zSize + vx;
			y1 = (j-1)*zSize + vy;
			x2 = i*zSize + vx;
			y2 = j*zSize + vy;
			if (tet.get(j*xField+i) > 0){
				DrawRect(hDC, x1, y1, x2, y2, DR_CFRAME);
				Print(hDC, strf(str, "%d", (j*xField+i)), x1, y1);
			}else if (tet.get(j*xField+i) == 0) {
				DrawRect(hDC, x1, y1, x2, y2, DR_CFRAME);
			}
		}

	return true;
}

