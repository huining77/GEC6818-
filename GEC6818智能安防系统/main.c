/*
 * @Description:结课工程主函数
 * @Version: 2.0
 * @Autor: 李慧宁-Huining Li777
 * @Date: 2023-12-22 15:02:49
 * @LastEditors: 李慧宁-Huining Li777
 * @LastEditTime: 2024-06-27 15:00:48
 */
#include <stdio.h>
#include "picture.h"
#include "lcd.h"
#include "touch.h"
#include "music.h"
#include "login.h"
#include "paint.h"
#include "camera.h"
#include "play_video.h"
#include "intelligent_voice.h"
#include "sensor.h"
#include "gobang.h"

#include "picture.h"
//gcc *.c 

void App_Run()
{   
    int App_ID=0;
    while (1)
    {
        App_ID = desktop_touch();
        
        switch (App_ID)
        {
            case PhotoAlbum:/* 电子音乐相册 */
                eMusic_albums();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                break;
            case Monitor:/* 监控 */
                camera_Run();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                break;
            case DrawingBoard:/* 画板 */
                InitCanve();
                PaintRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                break;
            case Vdieo:/* 视频 */
                InitVideo();
                VideoRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                break;
            case intelligent_voice:/* 语音通讯 */
                Server_ClientRun("192.168.100.100","2000");
                show_bmp("Desktop_bmp.bmp",0,0,1);
                break;
            case Sensor :     /*传感器*/
                InitSensor();
                SensorRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                break;
            case Gobang: /*五子棋*/
                GobangRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                break;
            break;
            default:
                
                break;
        }
    }
}

void login_run()
{
    while(1)
    {
        TouchPoint touch_pos=Get_TouchPosition();//获取触摸位置
        int result = show_hua(touch_pos.x*800/1024);
        if (result == 1)
        {
            printf("解锁完成,请你在显示屏处输入正确的密码\n");
            break;
        }
    }
    
}

void password_run()
{
    while (1)
    {
        if(unlock_passward())  //密码输入界面****
        {
            printf("密码正确,可以开始使用APP了\n");
            break;
        }
    }
}

int main()
{

    
    //TcpInit("192.168.100.100",atoi("2000"));
    //sendFile("1024.bmp");
    
    Lcd_Init();//lcd 初始化
    touch_evntInit();//触摸事件初始化
    TouchPoint point;
    show_bmp("lockx.bmp",0,0,1);//背景图片居中显示解锁前的界面
    login_run();
    password_run();
    while(1)
    {
        show_bmp("Desktop_bmp.bmp",0,0,1);
        App_Run();
    }
    Lcd_close();
    Close_Touchfd();
    return 0;
}