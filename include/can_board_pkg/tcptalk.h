#ifndef __TCPTALK_H
#define __TCPTALK_H

#include <ros/ros.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#define MAXSIZE 1024
// #define TARGETIPADDR "192.168.0.30"  //150kg
#define TARGETIPADDR "192.168.1.30"  //300kg/500kg

#define PORTNUM 8000
// #define TARGETIPADDR "192.168.1.10"
// #define PORTNUM 4001
extern bool RecvFlag;

/*tcp服务端关闭*/
void Tcp_Server_Close();
/*tcp客户端关闭*/
void Tcp_Client_Close();
/*tcp客户端初始化*/
void Tcp_Client_Init();
/*tcp服务端初始化*/
void Tcp_Server_Init();

/*tcp客户端发送*/
int Tcp_Client_Write(unsigned char* buf,int len);
/*tcp客户端读取*/
int Tcp_Client_Read(uint8_t* buf);
/*tcp服务端读取*/
int Tcp_Server_Read(uint8_t* buf);

/*退出信号初始化*/
// void Exit_Signal_Init();
#endif