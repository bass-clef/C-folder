
#include "Fbp.h"


// ����������
void Initialize(){
}

// �I������
void Exitialize(){
}

// �v�Z
void Calc(){
}

// �`��
void Draw(){
	static int count;
	count ++;
	if (count == 1)
		DrawBox(0, 0, -100, -100, NULL);
	else
		DrawBox(0, 0, 0, 0, 1);
}

// �l�p�`
void DrawBox(float x1, float y1, float x2, float y2, short sFlag){
	static VERTEX3D	Vertex[4];	// ���_
	WORD Index[] = {0, 1, 2, 3, 2, 1};	// �O�p�`�̒��_*��
	if (sFlag == NULL) {
	
		// �S���_�̏ꏊ var.pos = VGet( x���W , y���W , z���W)
		Vertex[0].pos = VGet(x1, y1, 0.0f);	// ����
		Vertex[1].pos = VGet(x2, y1, 0.0f); // �E��
		Vertex[2].pos = VGet(x1, y2, 0.0f); // ����
		Vertex[3].pos = VGet(x2, y2, 0.0f);	// �E��
	
		for(int i = 0; i < 4; i++) {
			Vertex[i].norm	= VGet(0.0f, 0.0f, -1.0f);
			Vertex[i].dif	= GetColorU8(255, 255, 255, 255);
			Vertex[i].spc	= GetColorU8(  0,   0,   0,   0);
		}
	}
	// �l�p�`
	DrawPolygonIndexed3D(Vertex, 4, Index, 2, DX_NONE_GRAPH, FALSE);
}
