#ifndef __MOTECMOTOR_H
#define __MOTECMOTOR_H

#include "tcptalk.h"
#include <geometry_msgs/Twist.h>
#include "kinco.h"
// #define WHEELINTERVAL 0.15  //轮子直径
// #define WHEELDIAMETER 0.55  //两轮间距
// #define REDUCTIONRATIO 22   //减速比

#define SENDID 0x200   //发送ID
#define RECID 0x180    //接收ID
#define LEFTNODE  0x00  //左轮节点
#define RIGHTNODE 0x01 //右轮节点

#define LEN_2BYTE 2  //2字节长度
#define LEN_4BYTE 4  //4字节长度
#define LEN_6BYTE 6  //6字节长度
#define LEN_8BYTE 8  //8字节长度

/*初始化ros话题*/
void Init_Ros_Topic();
/*初始化电机*/
void Init_Motec_Motor();
/*自己创建的延时函数*/
int mySleep(uint32_t ms);
/*设置can速度*/
void Send_Can_Speed(int16_t speed,uint32_t id);
/*发送can数据帧*/
int Send_Can_Frame(uint32_t id,uint8_t* data,uint8_t datalen);
#endif