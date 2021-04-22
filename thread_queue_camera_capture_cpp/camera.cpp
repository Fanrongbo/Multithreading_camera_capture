#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include<vector>

//#include <time.h> 
#include <conio.h>
#include<mutex>

#include <iostream>
#include <stdio.h>
#include <thread>
#include <string>
#include <queue>
#include <Windows.h>

//#include "camera.h"

using namespace std;
using namespace cv;
bool recording_control=false;//拍摄标志位
vector<bool> video_save_processing = { 0,0 };
std::vector<std::queue<cv::Mat>> vecimgs(2);

void camera_reading(int camera_num, int width, int height)
{
	VideoCapture cap(camera_num);
	cap.set(CAP_PROP_FRAME_WIDTH, width);//设置摄像头采集图像分辨率1280.0
	cap.set(CAP_PROP_FRAME_HEIGHT, height);
	Mat frame;
	int flagg = 0;
	char key;
	long int total_capture_num = 0;//每一次录制获取的图像数目，用于对比实际保存的图像数目
	bool flag = false;
	clock_t end = clock();
	//int loss_frame_num = 0;
	string window_name;

	cout << "reading: imgs address of " << camera_num << " is " << &vecimgs[camera_num] <<endl;

	if (cap.isOpened())
	{
		//获取整个帧数
		//long totalFrameNumber = cap.get(CAP_PROP_FRAME_COUNT);
		//cout << "整个视频共" << totalFrameNumber << "帧" << endl;
		while (cap.read(frame))
		{
			if (vecimgs[camera_num].size() < 99999)//检测缓冲区是否超出2000幅图像
			{
				if (recording_control)//录制
				{
					key = waitKey(1);
					window_name = "Camera_" + std::to_string(camera_num) + "_Recording";
					imshow(window_name, frame);
					vecimgs[camera_num].push(frame);
					total_capture_num++;
					flag = true;
					/*flagg++;
					if ((clock() - end) >= 1000)
					{

					cout << "Camera" << camera_num << ": Recording!!: Image acquisition rate  fps=" << flagg <<
					"    number in queue:" << vecimgs[camera_num].size() << endl;
					flagg = 0;
					end = clock();
					}*/
				}
				else//播放，相机挂起
				{
					if (flag == true)
					{
						cout << "Camera-"<< camera_num <<":Stop Capture,total capturing frame number =" << total_capture_num << endl;
						cap.release();
						waitKey(100);
						VideoCapture cap(camera_num);
						cap.set(CAP_PROP_FRAME_WIDTH, width);//设置摄像头采集图像分辨率1280.0
						cap.set(CAP_PROP_FRAME_HEIGHT, height);
						if (!cap.isOpened())
						{
							cout << "!!!!!!!!!!!!!!!!!!!!Camera-" << camera_num << ":No Signal" << endl;
						}
						cap.read(frame);
						waitKey(10);
					}
					else
					{
						flagg++;
						waitKey(1);
						window_name = "Camera_" + std::to_string(camera_num) + "_Live";
						imshow(window_name, frame);
						if ((clock() - end) >= 1000)
						{
							cout << "Camera-" << camera_num << ": Living!!: Image acquisition rate  fps=" << flagg <<
								"    number in queue:" << vecimgs[camera_num].size() << endl;
							flagg = 0;
							end = clock();
						}
					}
				}
				flag = false;

			}
			else  //缓冲区已满，挂起等待
			{
				Sleep(10);
				//cout << "Camera-1 Dropped Frame!!" << endl;
			}
		}
	}
	else
	{
		Sleep(10);
		cout << "Camera-" << camera_num << ": No Camera Signal!!!" << endl;
	}
	cap.release();
}
void camera_process(int camera_num,int width, int height, double fps, string _fourcc, string save_path)
{
	clock_t end = clock();
	int flagg = 0;//记录每秒多少帧数
	int video_num = 0;//记录第几个视频
	Sleep(10);//等待摄像头
	long int frame_number = 0;

	cout << "process: vecimgs[camera_num] address of " << camera_num << " is " << &vecimgs[camera_num] << endl;

	for (;;)
	{	
		
		if (recording_control)//监控录像标志位
		{
			VideoWriter writer;
			char str[100];
			sprintf_s(str, "C:/Users/86187/Desktop/test/multi_%d_c%d.avi", video_num, camera_num);
			string filename = string(str);
			
			Size size = Size(width, height);
			int codec = VideoWriter::fourcc(_fourcc[0], _fourcc[1], _fourcc[2], _fourcc[3]);
			
			//int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
			writer.open(filename, codec, fps, size, true);
			
			if (!writer.isOpened()) {
				cerr << "Camera-" << camera_num << ": Could not open the output video file for write\n";
			}
			while (1)
			{
				if (!vecimgs[camera_num] .empty())//如果缓冲区有图像就保存
				{
					//Mat img = vecimgs[camera_num]_2.front();
					//GaussianBlur(vecimgs[camera_num]_2.front(), vecimgs[camera_num]_2.front(), Size(5, 5), 0, 0);
					//cout << camera_num << "-number in queue : " << vecimgs[camera_num].size() << endl;
					writer.write(vecimgs[camera_num].front());
					vecimgs[camera_num].pop();
					waitKey(1);
					flagg++;
					frame_number++;
					if ((clock() - end) >= 1000)
					{
						cout << "Camera-" << camera_num << ": Recording!!: save image rate fps=" << flagg <<
							"    number in queue:" << vecimgs[camera_num].size() << endl;
						//printf("Camera-1 fps=%1d \n", flagg);
						flagg = 0;
						end = clock();
					}

				}
				else if (vecimgs[camera_num].empty() && !recording_control)
				{
					cout << "Camera-" << camera_num << ": Recording Stop!!   Video-" << video_num << endl;
					cout << "Camera-" << camera_num << ": Total saving frame number =" << frame_number << ";  Writing videofile: " << filename << endl;
					writer.release();
					waitKey(10);
					video_save_processing[camera_num] = 0;
					//video_2_save_processing = false;//视频保存操作完成
					video_num++;
					break;
				}
				else if (vecimgs[camera_num].empty() && recording_control)
				{
					Sleep(10);
				}
			}

		}
		else
		{
			Sleep(10);
		}

	}
	//writer.release();
}