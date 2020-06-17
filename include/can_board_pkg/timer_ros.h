#ifndef TIMER_ROS_H_
#define TIMER_ROS_H_

#include <ros/ros.h>
#include <stdlib.h>

/*������һ�ε�����ʱ��*/
typedef struct 
{
    double _task_send_instance_speed_time;
    double _task_send_smooth_speed_time;
	double _task_send_motor_io_time;
	double _task_read_motor_io_time;
	double _board_recv_data_time;  //用于接收超时计时
	double _task_send_heart_beat_time;
	
}Task_Last_Time_Type;

extern Task_Last_Time_Type Tasklasttime;

/*��ʱ���ص�����*/
void Timer_CallBack(const ros::TimerEvent& event);
/*��ȡ��ǰʱ��*/
double Get_Curtime(void);
/*�ж��Ƿ�ʱ*/
bool is_Timeout(double* lasttime,uint32_t time);
/*用于线程的延时函数*/
int mySleep_ms(uint32_t ms);
#endif



