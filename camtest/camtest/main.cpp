
//#define _DEBUG

// ���C�u�����̃����N�i�s�v�ȕ��̓R�����g�A�E�g
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

// �猟�o(����)
int main(int argc, char* argv[]) {

	// �r�f�I�L���v�`���\����
	CvCapture *capture = 0;
	// �t���[���P�ʃf�[�^
	IplImage *frame = 0;
	// �t���[���P�ʃf�[�^�R�s�[�p
	IplImage *frame_copy = 0;
	// �c���T�C�Y
	double height = 320;
	double width = 480;
	// ���̓L�[�󂯕t���p
	int c;
	
	// ���ʊ猟�o��̓ǂݍ���
	CvHaarClassifierCascade* cvHCC = (CvHaarClassifierCascade*)cvLoad("C:\\opencv248\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml");

	// ���o�ɕK�v�ȃ������X�g���[�W��p�ӂ���
	CvMemStorage* cvMStr = cvCreateMemStorage(0);

	// ���o�����󂯎�邽�߂̃V�[�P���X��p�ӂ���
	CvSeq* face;
	
	// 0�ԍ��̃J�����ɑ΂���L���v�`���\���̂𐶐������
	capture = cvCreateCameraCapture (0);
	
	// �L���v�`���̃T�C�Y��ݒ肷��B�������A���̐ݒ�̓L���v�`�����s���J�����Ɉˑ�����̂Œ��ӂ�
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, width);
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, height);
	cvNamedWindow ("capture_face_detect", CV_WINDOW_AUTOSIZE);
	
	int avecng = 0, count = 0, average = 0, oldave = 0;
	char str[64], moved = 0;
	HWND hhspwnd = FindWindow("hspwnd0", "camtext");
	// ��~�L�[���������܂ŃJ�����L���v�`���𑱂���
	while (1) {
		if (!hhspwnd) break;
		else if (!IsWindow(hhspwnd)) break;
		frame = cvQueryFrame (capture);

		// �t���[���R�s�[�p�C���[�W����
		frame_copy = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, frame->nChannels);
		if(frame->origin == IPL_ORIGIN_TL) {
			cvCopy(frame, frame_copy);
		} else {
			cvFlip(frame, frame_copy);
		}

		// �ǂݍ��񂾉摜�̃O���[�X�P�[�����A�y�уq�X�g�O�����̋ψꉻ���s��
		IplImage* gray = cvCreateImage(cvSize(frame_copy->width, frame_copy->height), IPL_DEPTH_8U, 1);
		IplImage* detect_frame = cvCreateImage(cvSize((frame_copy->width / SCALE), (frame_copy->height / SCALE)), IPL_DEPTH_8U, 1);
		cvCvtColor(frame_copy, gray, CV_BGR2GRAY);
		cvResize(gray, detect_frame, CV_INTER_LINEAR);
		cvEqualizeHist(detect_frame, detect_frame);

		// �摜�����猟�o�Ώۂ̏����擾����
		face = cvHaarDetectObjects(detect_frame, cvHCC, cvMStr, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

		for (int i = 0; i < face->total; i++) {
			// ���o��񂩂��̈ʒu�����擾
			CvRect* faceRect = (CvRect*)cvGetSeqElem(face, i);

			// ���ϒl�v�f�擾
			if (i == 0) average = faceRect->x * SCALE;
			// �擾������̈ʒu���Ɋ�Â��A��`�`����s��
			cvRectangle(frame_copy,
				cvPoint(faceRect->x * SCALE, faceRect->y * SCALE),
				cvPoint((faceRect->x + faceRect->width) * SCALE, (faceRect->y + faceRect->height) * SCALE),
				CV_RGB(255, 0 ,0),
				1, CV_AA);
		}
		// ��ʒu�ɋ�`�`����{�����摜��\��
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

		// �I���L�[���͑҂��i�^�C���A�E�g�t���j
		c = cvWaitKey (10);
		if (c == 'e') break;
	}

	// ���������������X�g���[�W�����
	cvReleaseMemStorage(&cvMStr);

	// �L���v�`���̉��
	cvReleaseCapture (&capture);
	
	// �E�B���h�E�̔j��
	cvDestroyWindow("capture_face_detect");

	// �J�X�P�[�h���ʊ�̉��
	cvReleaseHaarClassifierCascade(&cvHCC);
	
	return 0;
}
