#include "mythread.h"
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "tcptalk.h"
#include "base_driver.h"
#include "timer_ros.h"
#include "accfun.h"

int8_t g_SendTcpDataCTX = 0;
int8_t g_ReadTcpDataCTX = 0;
int8_t g_RosPublishDataCTX = 0;


pthread_t g_SendTcpDataHandle;
pthread_t g_ReadTcpDataHandle;
pthread_t g_RosPublishDataHandle;



/*发送tcp数据线程*/
void* Send_Tcp_Data_Thread(void *param)
{
    int* rtx = (int*)param;
    Tasklasttime._task_send_smooth_speed_time   = Get_Curtime();
    Tasklasttime._task_read_motor_io_time       = Get_Curtime();
    Tasklasttime._task_send_motor_io_time       = Get_Curtime();
    Tasklasttime._task_send_heart_beat_time     = Get_Curtime();
    Tasklasttime._task_send_instance_speed_time = Get_Curtime();
    ros::Rate loop_rate(1000);
    printf("create send tcp data thread succeed!\n");
    while(!(*rtx))
    {
        if(is_Timeout(&Tasklasttime._task_send_instance_speed_time,2) == true)  //立即速度发送
        {
            BaseDriver::Instance()->Send_Instance_Speed();
        }
        if(is_Timeout(&Tasklasttime._task_send_heart_beat_time,249) == true)  //心跳发送
        {
            BaseDriver::Instance()->Send_Heart_Beat();  
        }
        if(is_Timeout(&Tasklasttime._task_send_smooth_speed_time,CALCSPEEDTIME) == true)  //发送平滑速度
        {
            BaseDriver::Instance()->Send_Smooth_Speed();
        }
        // if(is_Timeout(&Tasklasttime._task_read_speed_time,20) == true)  //20ms读取一次速度
        // {
        //     BaseDriver::Instance()->Read_Speed();
        // }
        // if(is_Timeout(&Tasklasttime._task_read_encoder_time,20) == true)  //20ms读取一次编码器值
        // {
        //     BaseDriver::Instance()->Read_Encoder();
        // }
        // if(is_Timeout(&Tasklasttime._task_read_motor_io_time,1000) == true)  //100ms读取电机io
        // {
        //     BaseDriver::Instance()->Read_Motor_IO();  
        // }
        // if(is_Timeout(&Tasklasttime._task_send_motor_io_time,8) == true)  //8ms检测一下io控制
        // {
        //     BaseDriver::Instance()->Send_Motor_IO();  
        // }
        ros::spinOnce();

        loop_rate.sleep();
    }
    pthread_exit(0);
}

/*读取tcp数据线程*/
void* Read_Tcp_Data_Thread(void *param)
{
    int* rtx = (int*)param;
    ros::Rate loop_rate(1000);

    printf("create read tcp data thread succeed!\n");
    while(!(*rtx))
    {
        BaseDriver::Instance()->frame->recv_proc();
        ros::spinOnce();
      
        loop_rate.sleep();
    }
    pthread_exit(0);

}


/*停止线程*/
void Stop_Thread(int signo) 
{
    g_SendTcpDataCTX = 1;
    g_ReadTcpDataCTX = 1;
    Tcp_Client_Close();
    pthread_join(g_SendTcpDataHandle,NULL);
    pthread_join(g_ReadTcpDataHandle,NULL);


	printf("\nStop procedure!!!\n");
	exit(-1);
}

/*创建线程*/
void Init_Thread()
{
    pthread_create(&g_SendTcpDataHandle, NULL, Send_Tcp_Data_Thread, &g_SendTcpDataCTX);
    pthread_create(&g_ReadTcpDataHandle, NULL, Read_Tcp_Data_Thread, &g_ReadTcpDataCTX);

    printf("create thread succeed!\n");
}

/*退出信号初始化*/
void Exit_Signal_Init()
{
	signal(SIGINT,Stop_Thread);
}


