#ifndef __CANERRA_H__
#define __CANERRA_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <netinet/in.h>        //  TCP/IP协议所需头文件
#include <sys/socket.h>
#include "touch.h"
#include "lcd.h"

typedef struct {
    uint16_t bfType;        // 文件类型，固定为 "BM"
    uint32_t bfSize;        // 文件大小，包括文件头和像素数据
    uint16_t bfReserved1;   // 保留字段，必须为 0
    uint16_t bfReserved2;   // 保留字段，必须为 0
    uint32_t bfOffBits;     // 像素数据的偏移量
} __attribute__((packed))BMPFileHeader;

typedef struct {
    uint32_t biSize;        // 信息头大小，固定为 40 字节
    int32_t biWidth;        // 图像宽度
    int32_t biHeight;       // 图像高度，负数表示顶部到底部
    uint16_t biPlanes;      // 必须为 1
    uint16_t biBitCount;    // 每个像素的位数，这里为 24 位真彩色
    uint32_t biCompression; // 压缩类型，0 表示不压缩
    uint32_t biSizeImage;   // 像素数据的大小，字节为单位
    int32_t biXPelsPerMeter;// 水平分辨率，像素每米
    int32_t biYPelsPerMeter;// 垂直分辨率，像素每米
    uint32_t biClrUsed;     // 使用的颜色索引数，0 表示使用所有颜色
    uint32_t biClrImportant;// 对图像显示有重要影响的颜色索引数，0 表示所有颜色都重要
} __attribute__((packed))BMPInfoHeader;

//monitor 主函数
void camera_Run();
int read_point(int x,int y);
int save_bmp(int x0, int y0, int w, int h);
void saveBMP(int width, int height);
void saveBMP1(int width, int height) ;

#endif