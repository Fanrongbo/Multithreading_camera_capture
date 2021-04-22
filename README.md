# Multithreading_camera_capture
In this project, I achieve to control two high-speed camera (50~200FPS) by the multi-threading, which have cpp and python version.As the high-speed camera can capture hundreds frames per second, it is hard to save images in hard-disk directly. I use the multi-threading to achieve images capturing and storage independently.

Note: In the cpp version,the version of opencv is 4.4.0;

The two python file (mutil_thread_main_capthread.py and mutil_thread_main_no_capthread.py) all can be run independently. The difference is the way to read the image frames from camera. 
