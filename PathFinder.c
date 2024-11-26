
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>

int main(void) {
	int width = 500;
	int height = 500;
	double dt = 1 / 30;
	int key = 0;
	IplImage* img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);	// width x height サイズのカラー画像を生成
	cvNamedWindow("Output");

	typedef struct field {
		int cost;
		int 
	};

	while (1) {
		field fields[50][50];
		cvSetZero(img);		
		cvShowImage("Output", img);




		
		
		// 次フレームまで待つ + キー入力受付
		key = cvWaitKey(dt * 1000);// キー入力待ち(ms 単位)
		if (key == ' ')// スペースキーを押したら終了
			break;
	}

	return 0;
}
