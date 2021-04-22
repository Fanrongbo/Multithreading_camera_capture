//#include <opencv2/core.hpp>
//#include <opencv2/videoio.hpp>
//#include <opencv2/highgui.hpp>
//#include <iostream>
//#include <thread>
//#include <string>
#include <stdio.h>
#include <time.h> 
#include "camera.h"
#include <conio.h>
#include <Windows.h>
extern bool recording_control ;
extern vector<bool> video_save_processing;
//opencv3.0或者3.1的using namespace cv和windows.h中ACCESS_MASK定义冲突。
//解决方案：注释掉所有的using namespace cv，然后在需要的地方写上cv::。
//using namespace cv;
//using namespace std;
//void camera1_reading(int camera_num, double width, double height, std::queue<cv::Mat> imgs);
//void camera_process(int camera_num, double width, double height, double fps, string _fourcc, string save_path, std::queue<cv::Mat> imgs, vector<bool> video_save_processing);
int main(int argc, char** argv) {
	
	string save_path_0 = "C:/Users/86187/Desktop/test/c0_";
	string save_path_1 = "C:/Users/86187/Desktop/test/c1_";
	//std::queue<cv::Mat> img_0;
	//std::queue<cv::Mat> img_1;
	//vector<bool> video_save_processing = { 0,0 };
	std::thread th1_1(camera_reading, 0, 1920, 1080);
	std::thread th1_2(camera_reading, 1, 640, 480);
	std::thread th2_1(camera_process, 0, 1920, 1080, 60.0, "MJPG", save_path_0);
	std::thread th2_2(camera_process, 1, 640, 480, 5.0, "MJPG", save_path_1);
	th1_1.detach();
	th2_1.detach();
	th1_2.detach();
	th2_2.detach();

	int ch;
	while (1)//主线程
	{
		if (_kbhit())
		{//如果有按键按下，则_kbhit()函数返回真
			ch = _getch();//使用_getch()函数获取按下的键值
			cout << ch;

			if ((ch == 'b')) //当按下空格时循环，ESC键的键值时27.
				if (video_save_processing[0] || video_save_processing[1])

				{
					cout << "Video is being saved! Please wait and try again!" << endl;
				}
				else
				{
					video_save_processing[0] = 1;
					video_save_processing[1] = 1;
					recording_control = true;
					cout << "Begin Recording!!!" << endl;
					cv::destroyWindow("Camera_0_Live");
					cv::destroyWindow("Camera_1_Live");
					//window_name = "Camera_" + std::to_string(camera_num) + "Live";
				}
			else if (ch == 's')//按下ESC停止录像
				if (!(video_save_processing[0] || video_save_processing[1]))
				{
					cout << "No processing or being closing! Please wait and try again! " << endl;
				}
				else
				{
					recording_control = false;
					//cout << "Stop Recording!!!" << endl;
					cv::destroyWindow("Camera_0_Recording");
					cv::destroyWindow("Camera_1_Recording");
				}
			else if (ch == 27)
			{
				cout << "Stop Processing！" << endl;
				break;
			}

		}
		Sleep(10);
		//cout << "ok" << endl;
	}
	//system("pause");
	return 0;
}