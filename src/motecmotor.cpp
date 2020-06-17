#include "motecmotor.h"


ros::Subscriber speed_send_sub;


/*速度结构体*/
typedef struct 
{
    float xspeed;
    float wspeed;
    float leftspeed;
    float rightspeed;
    int16_t leftrpm;
    int16_t rightrpm;
}SpeedTpye;

SpeedTpye t_Speed; 

/*电机can通讯协议帧*/
typedef struct
{
    uint8_t framemeg;
    uint8_t frameid[4];
    uint8_t framedata[8];
}MotecCanType;

/*自己创建的延时函数*/
int mySleep(uint32_t ms)
{
    struct timeval t_timeval;
    t_timeval.tv_sec = 0;
    t_timeval.tv_usec = 1000 * ms;
    select(0,NULL,NULL,NULL,&t_timeval);
}

/*发送can数据帧*/
int Send_Can_Frame(uint32_t id,uint8_t* data,uint8_t datalen)
{
    MotecCanType moteccan;
    int size = 1 + 4 + datalen;

    memset(&moteccan,0,sizeof(moteccan));
    moteccan.framemeg = datalen;
    for(int i = 3;i >= 0;i --)
    {
        moteccan.frameid[i] = id >> (8 * (3 - i));
    }
    // memcpy(moteccan.frameid,&id,4);
    memcpy(moteccan.framedata,data,datalen);
    int ret = Tcp_Client_Write((uint8_t*)&moteccan,sizeof(MotecCanType));
    // int ret = Tcp_Write((uint8_t*)&moteccan,sizeof(MotecCanType));
    
    // if(ret < 0)
    // {
    //     Tcp_Client_Close();
    //     Tcp_Client_Init();
    //     Tcp_Write((uint8_t*)&moteccan,sizeof(MotecCanType));
    //     printf("tcp reconnect !\n");
    // }
    // printf("ret = %d\n",ret);
    printf("moteccan.framemeg = %02x\n",moteccan.framemeg);
    for(int i = 0;i < 4;i ++)
        printf("moteccan.frameid = %02x\n",moteccan.frameid[i]);
    for(int j = 0;j < datalen;j ++)
        printf("moteccan.framedata = %02x\n",moteccan.framedata[j]);
    printf("\n");
    // printf("ret = %d\n",ret);
    return ret;
}

/*设置can速度*/
void Send_Can_Speed(int16_t speed,uint32_t id)
{
    uint8_t data[LEN_4BYTE];
    data[0] = 0x0F;
    data[1] = 0x00;
    data[2] = speed;
    data[3] = speed >> 8;
    Send_Can_Frame(id,data,LEN_4BYTE);
}

/*实际速度转为轮子的转速*/
void Speed_To_RPM(SpeedTpye* motordata)
{
   //vr = (wl + 2v) / 2   vl = (2v - wl) / 2
   motordata->leftspeed = (motordata->xspeed * 2 - motordata->wspeed * WHEELDIAMETER) / 2; //左右轮已经配置为同向，正常情况不配置左右轮是反向的
   motordata->rightspeed = (motordata->xspeed * 2 + motordata->wspeed * WHEELDIAMETER) / 2;
   
   //N = v*60 / (pi*d) * 减速比
   motordata->leftrpm  = motordata->leftspeed * 60 / (M_PI * WHEELINTERVAL) * REDUCTIONRATIO;
   motordata->rightrpm = motordata->rightspeed * 60 / (M_PI * WHEELINTERVAL) * REDUCTIONRATIO;
}

/*速度发送Dev回调函数*/
void Speed_Send_Cb(const geometry_msgs::Twist& vel_cmd)
{
    t_Speed.xspeed = vel_cmd.linear.x;
    t_Speed.wspeed = vel_cmd.angular.z;
    Speed_To_RPM(&t_Speed);  //速度转化
    Send_Can_Speed(t_Speed.leftrpm,SENDID | LEFTNODE);
    Send_Can_Speed(t_Speed.rightrpm,SENDID | RIGHTNODE);
    printf("xspeed = %f\n",t_Speed.xspeed);
    printf("wspeed = %f\n",t_Speed.wspeed);
    printf("leftspeed = %f\n",t_Speed.leftspeed);
    printf("rightspeed = %f\n",t_Speed.rightspeed);
    printf("leftrpm = %d\n",t_Speed.leftrpm);
    printf("rightrpm = %d\n",t_Speed.rightrpm);

}

/*初始化ros话题*/
void Init_Ros_Topic()
{
    ros::NodeHandle nh;
    speed_send_sub = nh.subscribe("cmd_vel",1,Speed_Send_Cb);
}

/*初始化电机*/
void Init_Motec_Motor()
{
    uint8_t opmode[] = {0x01,0x00};
    uint8_t poweron[] = {0x06,0x00,0x00,0x00,0x00,0x00};
    uint8_t enable[] = {0x07,0x00,0x00,0x00,0x00,0x00};
    Send_Can_Frame(0,opmode,sizeof(opmode));
    mySleep(2000);
    Send_Can_Frame(SENDID|LEFTNODE,poweron,sizeof(poweron));
    mySleep(2000);
    Send_Can_Frame(SENDID | RIGHTNODE,poweron,sizeof(poweron));
    mySleep(2000);
    Send_Can_Frame(SENDID | LEFTNODE,enable,sizeof(enable));
    mySleep(2000);
    Send_Can_Frame(SENDID | RIGHTNODE,enable,sizeof(enable));
    mySleep(2000);

    // if(ret > 0)
    // {
    //     printf("tcp send succeed! ret = %d\n",ret);
    // }
    // mySleep(1000);
}

