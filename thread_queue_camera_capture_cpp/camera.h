#pragma once
#ifndef _camera_h_
#define _camera_h_
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include<vector>
#include<string>
#include<iostream>
#include <queue>
//#include <opencv2/core.hpp>
//bool recording_control = false;

using namespace std;
//using namespace cv;
void camera_reading(int camera_num, int width, int height);
void camera_process(int camera_num, int width, int height, double fps, string _fourcc, string save_path);



#endif