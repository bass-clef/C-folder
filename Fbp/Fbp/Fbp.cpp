
#include "Fbp.h"


// 初期化処理
void Initialize(){
}

// 終了処理
void Exitialize(){
}

// 計算
void Calc(){
}

// 描画
void Draw(){
	static int count;
	count ++;
	if (count == 1)
		DrawBox(0, 0, -100, -100, NULL);
	else
		DrawBox(0, 0, 0, 0, 1);
}

// 四角形
void DrawBox(float x1, float y1, float x2, float y2, short sFlag){
	static VERTEX3D	Vertex[4];	// 頂点
	WORD Index[] = {0, 1, 2, 3, 2, 1};	// 三角形の頂点*個数
	if (sFlag == NULL) {
	
		// ４つ頂点の場所 var.pos = VGet( x座標 , y座標 , z座標)
		Vertex[0].pos = VGet(x1, y1, 0.0f);	// 左上
		Vertex[1].pos = VGet(x2, y1, 0.0f); // 右上
		Vertex[2].pos = VGet(x1, y2, 0.0f); // 左下
		Vertex[3].pos = VGet(x2, y2, 0.0f);	// 右下
	
		for(int i = 0; i < 4; i++) {
			Vertex[i].norm	= VGet(0.0f, 0.0f, -1.0f);
			Vertex[i].dif	= GetColorU8(255, 255, 255, 255);
			Vertex[i].spc	= GetColorU8(  0,   0,   0,   0);
		}
	}
	// 四角形
	DrawPolygonIndexed3D(Vertex, 4, Index, 2, DX_NONE_GRAPH, FALSE);
}
