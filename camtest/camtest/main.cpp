
//#define _DEBUG

// ライブラリのリンク（不要な物はコメントアウト
#pragma comment(lib, "opencv_core248.lib")
#pragma comment(lib, "opencv_highgui248.lib")
#pragma comment(lib, "opencv_imgproc248.lib")
#pragma comment(lib, "opencv_calib3d248.lib")
#pragma comment(lib, "opencv_objdetect248.lib")

#include <opencv2\opencv.hpp>
#include <stdio.h>
#include <ctype.h>
#include <Windows.h>

#define SCALE 1.3

// 顔検出(動画)
int main(int argc, char* argv[]) {

	// ビデオキャプチャ構造体
	CvCapture *capture = 0;
	// フレーム単位データ
	IplImage *frame = 0;
	// フレーム単位データコピー用
	IplImage *frame_copy = 0;
	// 縦横サイズ
	double height = 320;
	double width = 480;
	// 入力キー受け付け用
	int c;
	
	// 正面顔検出器の読み込み
	CvHaarClassifierCascade* cvHCC = (CvHaarClassifierCascade*)cvLoad("C:\\opencv248\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml");

	// 検出に必要なメモリストレージを用意する
	CvMemStorage* cvMStr = cvCreateMemStorage(0);

	// 検出情報を受け取るためのシーケンスを用意する
	CvSeq* face;
	
	// 0番号のカメラに対するキャプチャ構造体を生成するる
	capture = cvCreateCameraCapture (0);
	
	// キャプチャのサイズを設定する。ただし、この設定はキャプチャを行うカメラに依存するので注意る
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, width);
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, height);
	cvNamedWindow ("capture_face_detect", CV_WINDOW_AUTOSIZE);
	
	int avecng = 0, count = 0, average = 0, oldave = 0;
	char str[64], moved = 0;
	HWND hhspwnd = FindWindow("hspwnd0", "camtext");
	// 停止キーが押されるまでカメラキャプチャを続ける
	while (1) {
		if (!hhspwnd) break;
		else if (!IsWindow(hhspwnd)) break;
		frame = cvQueryFrame (capture);

		// フレームコピー用イメージ生成
		frame_copy = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, frame->nChannels);
		if(frame->origin == IPL_ORIGIN_TL) {
			cvCopy(frame, frame_copy);
		} else {
			cvFlip(frame, frame_copy);
		}

		// 読み込んだ画像のグレースケール化、及びヒストグラムの均一化を行う
		IplImage* gray = cvCreateImage(cvSize(frame_copy->width, frame_copy->height), IPL_DEPTH_8U, 1);
		IplImage* detect_frame = cvCreateImage(cvSize((frame_copy->width / SCALE), (frame_copy->height / SCALE)), IPL_DEPTH_8U, 1);
		cvCvtColor(frame_copy, gray, CV_BGR2GRAY);
		cvResize(gray, detect_frame, CV_INTER_LINEAR);
		cvEqualizeHist(detect_frame, detect_frame);

		// 画像中から検出対象の情報を取得する
		face = cvHaarDetectObjects(detect_frame, cvHCC, cvMStr, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

		for (int i = 0; i < face->total; i++) {
			// 検出情報から顔の位置情報を取得
			CvRect* faceRect = (CvRect*)cvGetSeqElem(face, i);

			// 平均値要素取得
			if (i == 0) average = faceRect->x * SCALE;
			// 取得した顔の位置情報に基づき、矩形描画を行う
			cvRectangle(frame_copy,
				cvPoint(faceRect->x * SCALE, faceRect->y * SCALE),
				cvPoint((faceRect->x + faceRect->width) * SCALE, (faceRect->y + faceRect->height) * SCALE),
				CV_RGB(255, 0 ,0),
				1, CV_AA);
		}
		// 顔位置に矩形描画を施した画像を表示
		cvShowImage ("capture_face_detect", frame_copy);

		// 
		if (count) {
			if (count == 18) {
				if (moved == 3)	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
//				else			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
			}else if (count == 10) {
				if (moved == 3)	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
//				else			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
			}
			count--;
		}else {
			wsprintf(str, "[%d] %d", oldave, average);
			if (oldave == 0) oldave = average;

			if (average <= oldave-7) {
				if (moved == 3) {
					moved = 0;
				} else if (moved == 0) {
					moved = 2;	// right
					count = 2;
					wsprintf(str, "right");
				}else moved = 1;
			}
			if (oldave+7 <= average) {
				if (moved == 2) {
					moved = 0;
				} else if (moved == 0) {
					count = 20;
					wsprintf(str, "left");
					moved = 3;	// left
				}else moved = 1;
			}
			if (moved == 1) moved = 0;

			if ((avecng = (avecng+1)%4) == 0) oldave = average;
			SetWindowText(hhspwnd, str);
		}

		// 終了キー入力待ち（タイムアウト付き）
		c = cvWaitKey (10);
		if (c == 'e') break;
	}

	// 生成したメモリストレージを解放
	cvReleaseMemStorage(&cvMStr);

	// キャプチャの解放
	cvReleaseCapture (&capture);
	
	// ウィンドウの破棄
	cvDestroyWindow("capture_face_detect");

	// カスケード識別器の解放
	cvReleaseHaarClassifierCascade(&cvHCC);
	
	return 0;
}
