#include "camera.h"
#include "picture.h"
typedef struct
{
    char *start;
    size_t length;
} buffer_t;

buffer_t buffer[4]; // 映射所需的素材缓存在数组中
buffer_t current;   // 保存当前输出的一帧

//int *plcd;  // 用于存储屏幕缓冲区的首地址,以便后续对屏幕进行读写操作
int *lcd_p; // 指向屏幕缓冲区的特定位置
int lcd_fd; // 用于存储屏幕设备文件的文件描述符

int save_index=0;//保存的图片的下标


// RGB缓冲区
char rgb[640 * 480 * 3]={0};

void *lcd_init()
{
    lcd_fd = open("/dev/fb0", O_RDWR);
    if (lcd_fd == -1)
    {
        perror("open lcd_file error\n");
        return MAP_FAILED;
    }
    plcd = (int *)mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    return plcd;
}
///-----------------------------------------------------
/*
    读点函数
    (x,y):读点的坐标
    返回值：读点的颜色
*/
int read_point(int x,int y)
{
    if( x>=0 && x<800 && y>=0 && y<480 ) //x，y存在范围
    {
        return *(plcd + y*800 + x);

        //3 * (y*640 + x)] << 16 | rgb[3 * (y*640 + x) + 1] << 8 | rgb[3 * (y*640 + x) + 2
    }
}
/*
 * 保存bmp文件的函数
 * 参数:
 *     x0, y0: 图片显示的坐标
 *     w, h: 图片的宽和高
 * 返回值:
 *     成功返回0，失败返回-1
 */
int save_bmp(int x0, int y0, int w, int h) {
    int fd;
    int ret;
    int i;
    int fill = 0;
    int size = w * h * 3 + 54; // 计算文件大小
    unsigned char buf[size];

//--------------------------------------------------



    // 初始化buf
    memset(buf, 0, size);

    // 写入文件头
    buf[0] = 'B';
    buf[1] = 'M';
    *(int *)(buf + 2) = size;
    *(int *)(buf + 10) = 54; // 文件头大小
    *(int *)(buf + 14) = 40; // 信息头大小
    *(int *)(buf + 18) = w;
    *(int *)(buf + 22) = h;
    *(short *)(buf + 26) = 1; // 颜色数
    *(int *)(buf + 28) = 0; // 压缩方式
    *(int *)(buf + 32) = w * h * 3; // 像素数据大小
    *(int *)(buf + 36) = 0; // 水平分辨率
    *(int *)(buf + 40) = 0; // 垂直分辨率

    // 写入像素数据
    for (i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {

            buf[54 + i * w * 3 + j * 3] = rgb[(i * w + j) * 3 + 2];
            buf[54 + i * w * 3 + j * 3 + 1] = rgb[(i * w + j) * 3 + 1];
            buf[54 + i * w * 3 + j * 3 + 2] = rgb[(i * w + j) * 3 + 0];
        }
    }
    char buf1[30]={0};
     if(save_index>4)
    {
        save_index=0;
    }
    sprintf(buf1,"photo_%d.bmp",++save_index);
    printf("%s\n",buf1);
   
    // 写入文件
    fd = open(buf1, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        ret = -1;
        perror("open buf1 error");
        close(fd);
        return ret;
    }
    write(fd, buf, size);
    close(fd);
    memset(buf1, 0, 30);
    return 0;
}

void saveBMP(int width, int height) {
    // BMPFileHeader fileHeader;
    // BMPInfoHeader infoHeader;
    
    char buf[30]={0};
    sprintf(buf,"photo_%d",save_index);
    printf("%s\n",buf);
    int fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        int ret = -1;
        perror("open buf1 error");
        close(fd);
        return ;
    }

    // // BMP 文件头
    // fileHeader.bfType = 0x4D42; // "BM"
    // fileHeader.bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + width * height * 3; // 3 字节每像素
    // fileHeader.bfReserved1 = 0;
    // fileHeader.bfReserved2 = 0;
    // fileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    
    // // BMP 信息头
    // infoHeader.biSize = sizeof(BMPInfoHeader);
    // infoHeader.biWidth = width;
    // infoHeader.biHeight = height;
    // printf("w:%d\n",infoHeader.biWidth);
    // printf("h:%d\n",infoHeader.biHeight);
    // infoHeader.biPlanes = 1;
    // infoHeader.biBitCount = 24; // 24 位真彩色
    // infoHeader.biCompression = 0;
    // infoHeader.biSizeImage = width * height * 3;
    // infoHeader.biXPelsPerMeter = 0;
    // infoHeader.biYPelsPerMeter = 0;
    // infoHeader.biClrUsed = 0;
    // infoHeader.biClrImportant = 0;

    // // 写入文件头和信息头
    // write(fd,&fileHeader, sizeof(BMPFileHeader));
    // write(fd,&infoHeader, sizeof(BMPInfoHeader));

    unsigned char pixels[3*width*height];

    // 写入像素数据
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            pixels[i * width * 3 + j * 3] = rgb[(i * width + j) * 3 + 2];
            pixels[i * width * 3 + j * 3 + 1] = rgb[(i * width + j) * 3 + 1];
            pixels[i * width * 3 + j * 3 + 2] = rgb[(i * width + j) * 3 + 0];
        }
    }

    // 写入像素数据
    write(fd,pixels, sizeof(pixels));

    close(fd);
}

//保存 BMP 文件的函数
void saveBMP1(int width, int height) {
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    char buf[30];
    sprintf(buf, "photo_%d.bmp", ++save_index);
    printf("%s\n", buf);
    printf("测试截图......\n");
    // 打开文件
    int fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open error");
        return;
    }

    // BMP 文件头
    fileHeader.bfType = 0x4D42; // "BM"
    fileHeader.bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + width * height * 3; // 文件大小
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    // BMP 信息头
    infoHeader.biSize = sizeof(BMPInfoHeader);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height; // 正常BMP文件，高度是正值表示从下到上
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24; // 24 位真彩色
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = width * height * 3;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    // 写入文件头和信息头
    write(fd, &fileHeader, sizeof(BMPFileHeader));
    write(fd, &infoHeader, sizeof(BMPInfoHeader));

    // BMP文件每行像素需要4字节对齐
    int row_padded = (width * 3 + 3) & (~3);
    unsigned char* padded_rgb = malloc(row_padded * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            padded_rgb[(height - 1 - y) * row_padded + x * 3 + 0] = rgb[(y * width + x) * 3 + 2]; // B
            padded_rgb[(height - 1 - y) * row_padded + x * 3 + 1] = rgb[(y * width + x) * 3 + 1]; // G
            padded_rgb[(height - 1 - y) * row_padded + x * 3 + 2] = rgb[(y * width + x) * 3 + 0]; // R
        }
    }

    // 写入像素数据
    write(fd, padded_rgb, row_padded * height);

    // 关闭文件
    close(fd);

    free(padded_rgb);
}
//----------------------------------------------------
int uninit_lcd()
{
    close(lcd_fd);
    if (munmap(lcd_p, 800 * 480 * 4) == -1)
    {
        return -1;
    }
    return 0;
}

unsigned int sign3 = 0;
//YUYV转rgb
int yuyv2rgb(int y, int u, int v)
{
    unsigned int pixel24 = 0;
    unsigned char *pixel = (unsigned char *)&pixel24;
    int r, g, b;
    static int ruv, guv, buv;
    if (sign3)
    {
        sign3 = 0;
        ruv = 1159 * (v - 128);
        guv = 380 * (u - 128) + 813 * (v - 128);
        buv = 2018 * (u - 128);
    }
    r = (1164 * (y - 16) + ruv) / 1000;
    g = (1164 * (y - 16) - guv) / 1000;
    b = (1164 * (y - 16) + buv) / 1000;
    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;
    if (r < 0)
        r = 0;
    if (g < 0)
        g = 0;
    if (b < 0)
        b = 0;
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
    return pixel24;
}

//YUYV转rgb（调用yuyv2rgb函数）
int yuyv2rgb0(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out;
    int y0, u, y1, v;
    unsigned int pixel24;
    unsigned char *pixel = (unsigned char *)&pixel24;
    unsigned int size = width * height * 2;
    for (in = 0, out = 0; in < size; in += 4, out += 6)
    {
        y0 = yuv[in + 0];
        u = yuv[in + 1];
        y1 = yuv[in + 2];
        v = yuv[in + 3];
        sign3 = 1;
        pixel24 = yuyv2rgb(y0, u, v);
        rgb[out + 0] = pixel[0];
        rgb[out + 1] = pixel[1];
        rgb[out + 2] = pixel[2];
        pixel24 = yuyv2rgb(y1, u, v);
        rgb[out + 3] = pixel[0];
        rgb[out + 4] = pixel[1];
        rgb[out + 5] = pixel[2];
    }
    return 0;
}


int camera_flag=0;

//**********camera主函数***************//
void camera_Run()
{
    lcd_init();
    display_bgm(0xFFFFFFFF);
    // 打开摄像头
    int fd_v4l2 = open("/dev/video7", O_RDWR);
    if (fd_v4l2 == -1)
    {
        perror("open error");
        exit(-1);
    }
    // 获取功能参数
    struct v4l2_capability cap = {};
    int res = ioctl(fd_v4l2, VIDIOC_QUERYCAP, &cap);
    if (res == -1)
    {
        perror("ioctl cap");
        exit(-1);
    }
    // 先确定摄像头功能可以使用
    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("is a capture device!\n");
    }
    else
    {
        printf("is not a capture device!\n");
        exit(-1);
    }
    // 获取摄像头支持的格式
    struct v4l2_fmtdesc fmt = {};
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 获取摄像头格式
    while ((res = ioctl(fd_v4l2, VIDIOC_ENUM_FMT, &fmt)) == 0)
    {
        printf("pixformat = %c %c %c %c,description = %s\n",
                fmt.pixelformat & 0xff,
                (fmt.pixelformat >> 8) & 0xff,
                (fmt.pixelformat >> 16) & 0xff,
                (fmt.pixelformat >> 24) & 0xff,
                fmt.description);
        fmt.index++;
    }
    // 设置采集通道
    int index = 0; // 使用通道0
    res = ioctl(fd_v4l2, VIDIOC_S_INPUT, &index);
    if (res == -1)
    {
        perror("ioctl_s_input");
        exit(-1);
    }
    // 设置摄像头采集格式
    struct v4l2_format format = {};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width =640;//640--800
    format.fmt.pix.height = 480;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // YUYV
    format.fmt.pix.field = V4L2_FIELD_NONE;
    res = ioctl(fd_v4l2, VIDIOC_S_FMT, &format);
    if (res == -1)
    {
        perror("ioctl s_fmt");
        exit(-1);
    }
    // 申请缓存空间
    struct v4l2_requestbuffers req = {};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    res = ioctl(fd_v4l2, VIDIOC_REQBUFS, &req);
    if (res == -1)
    {
        perror("ioctl reqbufs");
        exit(-1);
    }
    // 分配映射入队
    size_t i, max_len = 0;
    for (i = 0; i < 4; i++)
    {
        struct v4l2_buffer buf = {};
        buf.index = i; // 0~3展现4帧图片
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        res = ioctl(fd_v4l2, VIDIOC_QUERYBUF, &buf);
        if (res == -1)
        {
            perror("ioctl querybuf");
            exit(-1);
        }
        // 判读并记录最大长度，以适配各个图帧
        if (buf.length > max_len)
            max_len = buf.length;
        // 映射
        buffer[i].length = buf.length;
        buffer[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_v4l2, buf.m.offset);
        if (buffer[i].start == MAP_FAILED)
        {
            perror("mmap");
            exit(-1);
        }
        // 入队
        res = ioctl(fd_v4l2, VIDIOC_QBUF, &buf);
        if (res == -1)
        {
            perror("ioctl qbuf");
            exit(-1);
        }
    }
    // 申请临时缓冲区
    current.start = malloc(max_len);
    if (current.start == NULL)
    {
        perror("malloc");
        exit(-1);
    }
    // 启动摄像头
    enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    res = ioctl(fd_v4l2, VIDIOC_STREAMON, &buf_type);
    if (res == -1)
    {
        perror("ioctl streamon");
        exit(-1);
    }
    // 延时
    sleep(1);
    // RGB缓冲区
    // char rgb[640 * 480 * 3];
    
    pthread_t thread_camera_touch;
    if (pthread_create(&thread_camera_touch,NULL,camera_touch,NULL) != 0)
	{
		perror("camera_touch_task create fail");
		return ;
	}
    // 采集数据
    while (1)
    {   
        printf("%d\n",camera_flag);

        if(camera_flag==0)
        {
        struct v4l2_buffer buf = {};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        // 出队
        res = ioctl(fd_v4l2, VIDIOC_DQBUF, &buf);
        if (res == -1)
        {
            perror("ioctl dqbuf");
        }
        // 拷贝数据
        memcpy(current.start, buffer[buf.index].start, buf.bytesused);
        current.length = buf.bytesused;
        // 入队
        res = ioctl(fd_v4l2, VIDIOC_QBUF, &buf);
        if (res == -1)
        {
            perror("ioctl qbuf");
        }
        // YUYV转RGB
        yuyv2rgb0(current.start, rgb, 640, 480);
        // 显示LCD
        int x, y;
        for (y = 0; y < 480; y++)
        {
            for (x = 0; x < 640; x++)
            {
                point(x,y,rgb[3 * (y*640 + x)] << 16 | rgb[3 * (y*640 + x) + 1] << 8 | rgb[3 * (y*640 + x) + 2]);
                //*(plcd + y*800 + x) = rgb[3 * (y*640 + x)] << 16 | rgb[3 * (y*640 + x) + 1] << 8 | rgb[3 * (y*640 + x) + 2];
            }
        }
        }
        else
        {
            camera_flag=0;
            pthread_cancel(thread_camera_touch);
            //printf("123\n");
            break;
        }
    }
    
    // 关闭摄像头采集
    buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    res = ioctl(fd_v4l2, VIDIOC_STREAMOFF, &buf_type);
    if (res == -1)
    {
        perror("ioctl streamoff");
        exit(-1);
    }
    // 解除映射
    for (i = 0; i < 4; i++)
    {
        munmap(buffer[i].start, buffer[i].length);
    }
    free(current.start);
    sleep(1); // 延时一下
    close(fd_v4l2);
    //uninit_lcd();
}

