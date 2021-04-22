import threading
import cv2
from threading import Thread, enumerate
from queue import Queue
import time
class VideoCapture_hs:

  def __init__(self, name,width,height,fps):
    self.cap = cv2.VideoCapture(name)
    self.cap.set(5,fps)#帧数
    self.cap.set(3,width)
    self.cap.set(4,height)
    self.q = Queue()
    self.ret=True
    t = Thread(target=self._reader)
    t.daemon = True
    t.start()

  # read frames as soon as they are available, keeping only most recent one
  def _reader(self):
    while True:
      self.ret, frame = self.cap.read()
      if not self.ret:
        break
      if not self.q.empty():
        try:
          self.q.get_nowait()   # discard previous (unprocessed) frame
        except Queue.Empty:
          pass
      self.q.put(frame)

  def read(self):
    return self.ret, self.q.get()
  def release(self):
      self.cap.release()
      


def camera_reading(camera_num,width,height,fps):
    # cap = VideoCapture_hs(camera_num,width,height,fps)
    cap = cv2.VideoCapture(camera_num)
    cap.set(5,fps)#帧数
    cap.set(3,width)
    cap.set(4,height)
    
    flagg = 0
    total_capture_num = 0 #每一次录制获取的图像数目，用于对比实际保存的图像数目
    flag = False
    end=time.time()
    while cap.isOpened():
        ret, frame = cap.read()
        if frame_queue[camera_num].qsize()<9999:
            if not ret:
                print("!!!!!!!!!!!!!!!!!!!!Camera-" , camera_num , ":No Signal")
                break
            if recording_control:#recording
                # print('Camera:',camera_num,' recording!')
                key = cv2.waitKey(1)
                window_name = "Camera_" + str(camera_num) + "_Recording"
                cv2.imshow(window_name, frame)
                frame_queue[camera_num].put(frame)
                total_capture_num =total_capture_num+1
                flag = True
            else:  #playing
                if flag:
                    print("Camera-",camera_num ,":Stop Capture! Total capturing frame number =" , total_capture_num)
                    cap.release()
                    cv2.waitKey(100)
                    cap = cv2.VideoCapture(camera_num)
                    cap.set(5,fps)#帧数
                    cap.set(3,width)
                    cap.set(4,height)
                    if not cap.isOpened():
                        print("!!!!!!!!!!!!!!!!!!!!Camera-" , camera_num , ":No Signal")
                        break
                    ret, frame = cap.read()
                    cv2.waitKey(10)
                else:
                    flagg =flagg+1
                    cv2.waitKey(1)
                    window_name = "Camera_" + str(camera_num) + "_Live"
                    cv2.imshow(window_name, frame)
                    if (time.time()-end)>=1:
                        print("Camera-" , camera_num , ": Living!!: Image acquisition rate  fps=" , flagg ,"    number in queue:" , frame_queue[camera_num].qsize())
                        flagg = 0
                        end = time.time()
            flag=False
        else:# 缓冲区已满，挂起等待
            time.sleep(0.1)
            print('Camera:',camera_num,'Buffer full, suspend wait!')
    cap.release()
    
def camera_process( camera_num, width,  height,  fps,  fourcc=None,  save_name=''):
    global recording_control
    end = time.time()
    flagg = 0#记录每秒多少帧数
    video_num = 0#记录第几个视频
    time.sleep(5)#等待摄像头
    frame_number = 0
    while True:
        if recording_control:
            fourcc = cv2.VideoWriter_fourcc('M', 'J', 'P', 'G')
            save_path=save_name+'_camera'+str(camera_num)+'_num'+str(video_num)+'.avi'
            out = cv2.VideoWriter(save_path,fourcc, fps, (width,height))
            if not out.isOpened():
                print("Camera-" , camera_num , ": Could not open the output video file for write")
            while True:
                if not frame_queue[camera_num].empty():#如果缓冲区有图像就保存
                    out.write(frame_queue[camera_num].get())#先进先出
                    flagg=flagg+1
                    frame_number=frame_number+1
                    if (time.time()-end)>=1:
                        print("Camera-" , camera_num , ": Recording!!: save image rate fps=" , flagg ,
                                       "    number in queue:" , frame_queue[camera_num].qsize())
                        flagg = 0
                        end=time.time()
                elif frame_queue[camera_num].empty() and (not recording_control):
                    print("Camera-" , camera_num , ": Recording Stop!!   Video-" , video_num)
                    print("Camera-" , camera_num , ": Total saving frame number =" , frame_number , ";  Writing videofile: " ,save_path)
                    out.release()
                    cv2.waitKey(10)
                    video_save_processing[camera_num] = 0#视频保存操作完成
                    video_num=video_num+1
                    break
                elif frame_queue[camera_num].empty() and recording_control:
                    time.sleep(0.01)
        else:
            time.sleep(0.01)


from pynput.keyboard import Listener
from pynput import keyboard
global recording_control 
global video_num
recording_control=False
def pressa(key):
    global recording_control
    print('iok')
    if len(str(key))==3 and key.char=='b':
        if (video_save_processing[0] or video_save_processing[1]):
            print( "Video is being saved! Please wait and try again!" )
        else:
            for i in range(video_num):
                video_save_processing[i] = 1
            # video_save_processing[1] = 1
            recording_control = True
            print( "Begin Recording!!!" )
            cv2.destroyWindow("Camera_0_Live")
            cv2.destroyWindow("Camera_1_Live")
    elif len(str(key))==3 and key.char=='s':
        if (not(video_save_processing[0] or video_save_processing[1])):
            print("No processing or being closing! Please wait and try again! ")
        else:
            recording_control = False
            cv2.destroyWindow("Camera_0_Recording")
            cv2.destroyWindow("Camera_1_Recording")
    elif key== keyboard.Key.esc:
        print("Stop Processing！")
        
        return False
if __name__ == '__main__':
    global video_num
    video_num=2
    video_save_processing=[]
    video_save_processing=[]
    for i in range(video_num):
        video_save_processing.append(0)
        video_save_processing.append(0)
    

    frame_queue = [Queue(),Queue(),Queue()]
    # Thread(target=keyboarda, args=(0,1920)).start()
    c1=Thread(target=camera_reading, args=(0,1920,1080,60)).start()
    c2=Thread(target=camera_reading, args=(1,640,480,30)).start()
    c1_1=Thread(target=camera_process, args=(0, 1920, 1080, 60.0, "MJPG")).start()
    c2_1=Thread(target=camera_process, args=(1, 640, 480, 30.0, "MJPG")).start()

    with Listener(on_press=pressa) as listener:
        listener.join()
    





