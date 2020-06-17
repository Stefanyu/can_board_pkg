#ifndef DATA_HOLDER_H_
#define DATA_HOLDER_H_

#include <string.h>
#include <stdint.h>
#include "kinco.h"

#pragma pack(1)


typedef int int32;
typedef short int16;
typedef unsigned short uint16;


struct Robot_parameter{
    union{
        char buff[64];
        struct{
            unsigned short wheel_diameter;   //轮子直径  mm
            unsigned short wheel_track;      //差分：轮距， 三全向轮：直径，四全向：前后轮距+左右轮距 mm
            unsigned int encoder_resolution;  //编码器分辨率
            unsigned char do_pid_interval;         //pid间隔 (ms)
            unsigned short kp;
            unsigned short ki;
            unsigned short kd;
            unsigned short ko;                  //pid参数比例
            unsigned short cmd_last_time;       //命令持久时间ms 超过该时间会自动停止运动
            unsigned short max_v_liner_x;
            unsigned short max_v_liner_y;
            unsigned short max_v_angular_z;
        };
    };
};


struct Robot_Can_Board
{
    uint8_t motorctrlio;  //电机控制io
    uint8_t motorretio;  //电机反馈io
};

#pragma pack(0)

class Data_holder{
    public:
        static Data_holder* get(){
            static Data_holder dh;
            return &dh;
        }

        void load_parameter();

        void save_parameter();

    private:
        Data_holder()
		{
            memset(&parameter, 0, sizeof(struct Robot_parameter));

			memset(&can_board, 0, sizeof(struct Robot_Can_Board));
            memset(&S_leftdecsdo,0,sizeof(KincoType));
            memset(&S_rightdecsdo,0,sizeof(KincoType));
            memset(&R_leftdecsdo,0,sizeof(KincoType));
            memset(&R_rightdecsdo,0,sizeof(KincoType));
            memset(&R_leftencodersdo,0,sizeof(KincoType));
            memset(&R_rightencodersdo,0,sizeof(KincoType));
            memset(&R_IOStatus,0,sizeof(KincoIOType));
            memset(&S_IOStatus,0,sizeof(KincoIOType));
        }
    public:
        struct Robot_parameter  parameter;

			struct Robot_Can_Board can_board;
        KincoType S_leftdecsdo;
        KincoType S_rightdecsdo;
        KincoType R_leftdecsdo;
        KincoType R_rightdecsdo;
        KincoType R_leftencodersdo;
        KincoType R_rightencodersdo;
        KincoType S_leftmodesdo;
        KincoType S_rightmodesdo;
        KincoIOType R_IOStatus;
        KincoIOType S_IOStatus;
        int8_t S_leftdecpdo[6];
        int8_t S_rightdecpdo[6];
};
#endif
