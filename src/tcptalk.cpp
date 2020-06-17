#include "tcptalk.h"
#include "timer_ros.h"

int CSocketfd;  //client socket fd
int SSocketfd;  //server socket fd
int Clientfd;  //�������Ҫ���ӵĿͻ���




unsigned char buf[MAXSIZE];
int size;
bool RecvFlag = false;

/*ctrl+c退出关闭回调函数*/
// void Stop_Thread(int signo) 
// {
// 	Tcp_Server_Close();
// 	Tcp_Client_Close();
// 	printf("\nStop procedure!!!\n");
// 	exit(-1);
// }


/*tcp服务器初始化*/
void Tcp_Server_Init()
{
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int ret,on = 1;
	int flags;
	SSocketfd = socket(AF_INET,SOCK_STREAM,0);
    if (SSocketfd == -1)
    {
        std::cout << "SSocketfd create error!\n"<< std::endl;
        exit(-1);
    }
	ret = setsockopt(SSocketfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORTNUM);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int res = bind(SSocketfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if (res == -1)
    {
        std::cout << "SSocketfd bind error!\n" << std::endl;
        exit(-1);
    }
	listen(SSocketfd,100);  //�ȴ�����
	printf("listen socket\n");
	socklen_t len = sizeof(clientaddr);
    Clientfd = accept(SSocketfd,(struct sockaddr*)&clientaddr,&len);
	if (Clientfd == -1)
	{
		std::cout << "Clientfd accept error!\n" << std::endl;
		exit(-1);
	}
	flags = fcntl(Clientfd,F_GETFL,0);
	fcntl(Clientfd,F_SETFL,flags | O_NONBLOCK);  //设置tcp接收处于非阻塞状态

}

/*tcp客户端初始化*/
void Tcp_Client_Init()
{
	struct sockaddr_in serveraddr;
	int enable = 1;
	CSocketfd = socket(AF_INET, SOCK_STREAM,0);
	if(CSocketfd == -1)
	{
		std::cout<<"CSocketfd create error!\n"<<std::endl;
		exit(-1);
	}
	int ret = setsockopt(CSocketfd,IPPROTO_TCP,TCP_NODELAY,&enable,sizeof(int));  //设置不使用nagle算法，实时发送
	if(ret < 0)
	{
		printf("setsock nodelay error!\n");
		exit(-1);
	}
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORTNUM);
	serveraddr.sin_addr.s_addr = inet_addr(TARGETIPADDR);
	printf("tcp wait to connect...\n");
	int res = connect(CSocketfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res == -1)
    {
        std::cout<<"CSocketfd connect error!\n"<<std::endl;
        exit(-1);
    }
	printf("connect succeed!\n");
	// Tcp_Client_Close();
	// printf("close succeed!\n");
	// exit(-1);
}

/*tcp客户端关闭*/
void Tcp_Client_Close()
{
	close(CSocketfd);
}

/*tcp服务端关闭*/
void Tcp_Server_Close()
{
	close(SSocketfd);
	close(Clientfd);
}

/*tcp客户端发送*/
int Tcp_Client_Write(unsigned char* buf,int len)
{
	// mySleep_ms(2);
	int ret = write(CSocketfd,buf,len);
	if(ret < 0)
	{
		// perror("recv");
		printf("write error str = %s error= %d\n",strerror(errno),errno);
		if(errno == 11)
		{
			printf("can board tcp wirte error num 11!222222222222222222222222222222");
			printf("can board tcp wirte error num 11!222222222222222222222222222222");
			printf("can board tcp wirte error num 11!222222222222222222222222222222");
			printf("can board tcp wirte error num 11!222222222222222222222222222222");
			printf("can board tcp wirte error num 11!222222222222222222222222222222");
			// mySleep_ms(100);
		}
		if(errno == 32)  //服务器断开重新链接
		{
		
			// printf("11111111111111111111111111111111111111111reconnect tcp!\n");
			Tcp_Client_Close();
			Tcp_Client_Init();

		}
		// exit(-1);
	}
	return ret;
}

/*tcp服务端读取*/
int Tcp_Server_Read(uint8_t* buf)
{
	int ret = recv(Clientfd,buf,MAXSIZE,MSG_DONTWAIT);
	
	return ret;
}

/*tcp客户端读取*/
int Tcp_Client_Read(uint8_t* buf)
{
	int enable = 1;
	int ret = recv(CSocketfd,buf,MAXSIZE,MSG_DONTWAIT);
	// int ret = recv(CSocketfd,buf,MAXSIZE,0);

	setsockopt(CSocketfd,IPPROTO_TCP,TCP_QUICKACK,&enable,sizeof(enable));

	// int ret = recv(CSocketfd,buf,MAXSIZE,0);
	// if(ret < 0)
	// {
	// 	// perror("recv");
	// 	printf("ret = %d read error num = %d\n",ret,errno);
	// }
	// else if(ret == 0)
	// {
	// 	printf("ret = %d read error num = %d\n",ret,errno);
	// }
	return ret;
}

/*退出信号初始化*/
// void Exit_Signal_Init()
// {
// 	signal(SIGINT,Stop_Thread);
// }

