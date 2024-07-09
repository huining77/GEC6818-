#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <strings.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;

// 文件头结构体
typedef struct {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

// 信息头结构体
typedef struct {
    DWORD biSize; /*info header size in bytes*/
    DWORD biWidth; /*widht of image*/
    DWORD biHeight;/*height of image*/
    WORD biPlanes;/*number of colour planes*/
    WORD biBitCount;/*bits per pixel*/
    DWORD biCompression;/*compression type*/
    DWORD biSizeImage;/*image size meter*/
    DWORD biXPelsPerMeter;/*pixels per meter*/
    DWORD biYPelsPerMeter;/*pexels per meter*/
    DWORD biClrUsed;/*number of colour*/
    DWORD biClrImportant;/*important colour*/
} __attribute__((packed)) BITMAPINFOHEADER;

// 生成BMP文件
int imageBmp(unsigned char* rgbBytes, int width, int height, const char *filePath)
{
    int _bmppitch = ((width * 24 + 31) >> 5) << 2;
    unsigned char *_bmpBuffer = (unsigned char *) malloc(_bmppitch);
    memset(_bmpBuffer, 0, _bmppitch);

    BITMAPFILEHEADER    bmfh;
    BITMAPINFOHEADER    bmih;

    ((char *) &bmfh.bfType)[0] = 'B';
    ((char *) &bmfh.bfType)[1] = 'M';
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + _bmppitch * height;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = width;
    bmih.biHeight = height;
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;
    bmih.biCompression = 0;
    bmih.biSizeImage = 0; /*说明图像的大小，以字节为单位。当用BI_RGB格式时，总设置为0*/
    bmih.biXPelsPerMeter = 0; /*缺省值*/
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed = 0; /*说明位图实际使用的调色板索引数，0：使用所有的调色板索引*/
    bmih.biClrImportant = 0; /*说明对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要*/

    FILE* image_file = fopen(filePath, "wb");
    if (NULL == image_file)
    {
        //LOGE("imageBmp(): file INVALID, FILE_NAME= %s", filePath);
        free(_bmpBuffer);
        _bmpBuffer = NULL;
        return -1;
    }

    fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, image_file);
    fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, image_file);

    unsigned char _r, _g, _b;
    for (int y = height - 1; y >= 0; y--)
    {
        memset(_bmpBuffer, 0, _bmppitch);
        for (int x = 0; x < width; x++) {
            _bmpBuffer[x * 3 + 0] = rgbBytes[(y * width + x) * 3 + 0];
            _bmpBuffer[x * 3 + 1] = rgbBytes[(y * width + x) * 3 + 1];
            _bmpBuffer[x * 3 + 2] = rgbBytes[(y * width + x) * 3 + 2];
        }
        fwrite(_bmpBuffer, _bmppitch, 1, image_file);
    }

    fclose(image_file);

    free(_bmpBuffer);
    _bmpBuffer = NULL;

    return 0;
}


