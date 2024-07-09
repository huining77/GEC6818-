#include "intelligent_voice.h"
extern int save_index;
//全局的套接字
int sockfd = -1;

//将套接字创建好  并且绑定 监听
//将ip地址和端口号传进来 端口号释放需要时间(轮询机制)
void TcpInit(const char * ipaddr,unsigned short port)
{
    //1 创建套接字 ---- 神马都是文件，因此你的网络通信也是一个文件
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == sockfd)
    {
        perror("server socket error");
        exit(1);//没有必要运行了
    }
    //2 我们需要将服务器的IP地址绑定到套接字
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;//协议族
    sa.sin_port = htons(port);//端口号  内存是小端的 我们要转大端
    sa.sin_addr.s_addr = inet_addr(ipaddr);//将我们点分式的ip地址转换为一个大端整数
    

    //连接服务器
    int r = connect(sockfd,(struct sockaddr *)&sa,sizeof(sa));
    if(-1 == r)
    {
        perror("connect error");
        exit(2);
    }

}

// 发送文件
void sendFile(const char *filename) {
    unsigned char buf[1024] = {0};  // 定义一个缓冲区，每次读取和发送 1024 字节
    int fd = open(filename, O_RDWR);  // 打开文件
    if (fd == -1) {
        perror("open file error");  // 如果文件打开失败，打印错误信息并返回
        return;
    }

    int filesize = lseek(fd, 0, SEEK_END);  // 获取文件大小
    lseek(fd, 0, SEEK_SET);  // 将文件指针重置到文件开头
    send(sockfd, &filesize, sizeof(filesize), 0);  // 将文件大小发送给服务器

    while (1) {
        int r = read(fd, buf, sizeof(buf));  // 读取文件内容到缓冲区，每次最多读取 1024 字节
        if (r == -1) {
            perror("read file error");  // 如果读取文件失败，打印错误信息并退出循环
            break;
        } else if (r == 0) {
            printf("File transfer complete\n");  // 如果读取到文件末尾，打印文件传输完成并退出循环
            break;
        } else {
            send(sockfd, buf, r, 0);  // 将读取到的数据发送给服务器
        }
    }
    close(fd);  // 关闭文件
}

//开始正常的收发
int function(void)
{
	unsigned char buf[1024] = {0};
    while(1)
    {
        //首先发送文件大小
		//阻塞你按回车
		printf("按回车继续\n");
		getchar();
		//弄你的文件
		printf("请录音4秒........\n");
		//获取音频文件 
		system("arecord -d4 -c1 -r16000 -traw -fS16_LE hehe.pcm");
		int fd = open("hehe.pcm",O_RDWR);
		if(-1 == fd)
		{
			perror("open pcm error");
            return 0;
        }
		int filesize = lseek(fd,0x00,SEEK_END);
		lseek(fd,0x00,SEEK_SET);//偏移到开头
		send(sockfd,&filesize,4,0);
		//接收信息 "error!!!" or "next!!!!"
		
		recv(sockfd,buf,9,0);
		printf("----> %s\n",buf);
		if(strcmp(buf,"next!!!!"))
		{
			printf("服务器错误了\n");
			continue;
		}
        //如果是"next!!!!"发送文件
		while(1)
		{
			int r = read(fd,buf,1024);
			if(-1 == r)
			{
				perror("read pcm error");
                break;
            }
			else if(0 == r)
			{
				printf("over\n");
				break;			
			}
			else
			{
				send(sockfd,buf,r,0);
			}
		}
		
		close(fd);
		//等待接收id
		int id;
		recv(sockfd,&id,4,0);
		printf("id ===== %d\n",id);
        return id;
    }
}

int recive_fromS()
{
    char buf[1024] = {0};
    int r = recv(sockfd, buf, sizeof(buf), 0);
    if (r > 0) {
        printf("Received message: %s\n", buf);
        int id = atoi(buf); // 将字符串转换为整数
        return id;
    } else {
        perror("recv error");
        return -1;
    }
}


int Voice_ClientRun(char* ip,char* port)
{
    int id,openflag;
    TcpInit(ip,atoi(port));
    while (1)
    {
        id = function();
        switch (id)
        {
        case 1000:
            openflag = 1;
            show_bmp("hutao.bmp",20,245,0);
            system("madplay -Q hu_tao_start.mp3");//开始
            /* 函数 */
            
            printf("1000\n");
            break;
        case 1001:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_paint.mp3");//画板
                
                InitCanve();
                PaintRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                show_bmp("hutao.bmp",20,245,0);
                /* 函数 */
                printf("1001\n");
            }
            break;
        case 1002:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_photo.mp3");//相册
                /* 函数 */
                eMusic_albums();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                show_bmp("hutao.bmp",20,245,0);
                printf("1002\n");
            }
            break;
        case 1003:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_camera.mp3");//监控
                camera_Run();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                show_bmp("hutao.bmp",20,245,0);
                /* 函数 */
                printf("1003\n");
            }
            break;
        case 1004:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_game.mp3");//2048
                
                show_bmp("hutao.bmp",20,245,0);
                /* 函数 */
                printf("1004\n");
            }
            break;
        case 1005:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_no.mp3");//我帅不帅
                /* 函数 */

                show_bmp("hutao.bmp",20,245,0);
                printf("1005\n");
            }
            break;
        case 1006:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_game.mp3");//五子棋
                /* 函数 */
                GobangRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                show_bmp("hutao.bmp",20,245,0);
                printf("1006\n");
            }
            break;
        case 1007:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_vidio.mp3");//视频播放器
                /* 函数 */
                InitVideo();
                VideoRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                show_bmp("hutao.bmp",20,245,0);
                printf("1007\n");
            }
            break;
        case 1008:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_end.mp3");//结束
                openflag = 0;
                printf("1008\n");
                show_bmp("Desktop_bmp.bmp",0,0,1);
                //show_bmp("hutao.bmp",20,245,0);
                return 0;
            }
            break;
            //打开传感器!id(1009);
        case 1009:
            if(openflag == 1)
            {
                system("madplay -Q hu_tao_sensor.mp3");//结束
                InitSensor();
                SensorRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                show_bmp("hutao.bmp",20,245,0);
                printf("1009\n");
            }
            break;
        default:
            if(openflag == 1)
            {
                system("mplayer hu_tao_no.mp3");
            }
            break;
        }   
    }
    close(sockfd);
    return 0;
}
int Server_ClientRun(char* ip,char* port)
{
    int id,openflag;
    TcpInit(ip,atoi(port));
    while (1)
    {
        id = recive_fromS();
        printf("------------------%d---------------------\n",id);
        switch (id)
        {
        case 1234://打开摄像头
            openflag = 1;
            show_bmp("hutao.bmp",20,245,0);
            camera_Run();
            //system("madplay -Q hu_tao_start.mp3");//开始
            /* 函数 */
            printf("1234---打开监控程序\n");
            break;
        case 1235://关闭摄像头
            if(openflag == 1)
            {
                //system("madplay -Q hu_tao_paint.mp3");//画板
                
                // InitCanve();
                // PaintRun();
                show_bmp("Desktop_bmp.bmp",0,0,1);
                show_bmp("hutao.bmp",20,245,0);
                /* 函数 */
                printf("1235\n");
            }
            break;
        case 1236://截图
            if(openflag == 1)
            {
                save_bmp(0,0,640,480);
                saveBMP(640,480);

                //saveBMP("123.bmp",640,480);
                printf("截图中........\n");
                printf("1236\n");
            }
            break;
        case 1237://发送
            if(openflag == 1)
            {
                char buf1[30]={0};
                sprintf(buf1,"photo_%d",save_index);
                printf("%s\n",buf1);
                //camera_flag = 1;
                sendFile(buf1);
                printf("发送中........\n");
                /* 函数 */
                printf("1003\n");
            }
            break;
        default:
            break;
        }   
    }
    close(sockfd);
    return 0;
}