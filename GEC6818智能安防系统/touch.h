#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h> 
#include <linux/input.h>
#include <string.h>
#include "lcd.h"
#include "music.h"
#include <pthread.h>
#define Touch_Up 1
#define Touch_Down 2
#define Touch_Left 3
#define Touch_Right 4
#define Touch_Click 5
#define TOUCH_PATH "/dev/input/event0"

extern int touch_fd;//屏幕设备
extern int camera_flag;
//----桌面图标的宏定义
//第一个图标的位置——相册
#define PhotoAlbum_X_START 20
#define PhotoAlbum_Y_START 20
#define PhotoAlbum_X_END 90
#define PhotoAlbum_Y_END 90

//第二个图标的位置——监控 monitor
#define Monitor_X_START 135
#define Monitor_Y_START 20
#define Monitor_X_END 200
#define Monitor_Y_END 90

// 第三个图标的位置——画板 Drawing board

#define DrawingBoard_X_START 240
#define DrawingBoard_Y_START 20
#define DrawingBoard_X_END 310
#define DrawingBoard_Y_END 85

// 第四个图标的位置-- 视频播放 video
#define Vdieo_X_START 350
#define Vdieo_Y_START 20
#define Vdieo_X_END 410
#define Vdieo_Y_END 85

//第五个图标位置-- 智能语音
#define Intelligent_voice_X_START 345
#define Intelligent_voice_Y_START 405
#define intelligent_voice_X_END 430
#define intelligent_voice_Y_END 480

//第六个图标的位置--传感器
#define Sensor_X_START 450
#define Sensor_Y_START 20
#define Sensor_X_END 520
#define Sensor_Y_END 85

//第七个图标的位置--五子棋gobang
#define Gobang_X_START 565
#define Gobang_Y_START 20
#define Gobang_X_END 640
#define Gobang_Y_END 88


//extern int eMusic_albums_touch_flag;
//APP ID
#define PhotoAlbum 1
#define Monitor 2
#define DrawingBoard 3
#define Vdieo 4
#define intelligent_voice 5
#define Sensor 6
#define Gobang 7

// 定义触摸点的结构体//变量不能初始化
typedef struct {
    int x;
    int y;
    int is_long_press;//是否长按
} TouchPoint;

//********lzs--获取方向的方向值************//
typedef enum
{
    TouchError = 0,//一个错误值
    TouchClick,
    TouchUp,
    TouchDown,
    TouchLeft,
    TouchRight,
    TouchOther,
    TouchExit
}TouchDir;


//*********触摸事件初始化*********
void touch_evntInit(void);
//************获取触摸方向*********
//返回值：触摸方向，具体定义在宏定义处
int Get_TouchDirection(void);
//*****************获取触摸点位置*********
//**************获取触摸位置--可以识别长按****
// 返回值是上面的一个TouchPOint的结构体，
//xy为触摸点的位置，
//is_long_press为是否长按，0--未长按  1--长按
TouchPoint Get_TouchPosition();

//销毁TOUCH_FD
void Close_Touchfd();
//桌面图标位置
int desktop_touch();
//monitor 退出
void* camera_touch(void* argv);
//线程---相册时使用获取当前状态
void *GetState(void *argv);
// ****线程-用于退出音乐相册***********//
//void *eMusic_albums_touch(void * argv);
//***********lzs--获取方向*************//
TouchDir GetTouchDir(void);

#endif