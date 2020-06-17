#include "timer_ros.h"
#include <sys/time.h>

Task_Last_Time_Type Tasklasttime;

/*用于线程的延时函数*/
int mySleep_ms(uint32_t ms)
{
    struct timeval t_timeval;
    t_timeval.tv_sec = 0;
    t_timeval.tv_usec = 1000 * ms;
    select(0,NULL,NULL,NULL,&t_timeval);
    return 0;
}

// uint64_t Timetick_ms = 0; 

/*��ʱ���ص�����*/
// void Timer_CallBack(const ros::TimerEvent& event)
// {
// //    Timetick_ms ++;
//    ros::Time::now().toSec();
// //    ROS_INFO("Timetick_ms = %ld\n",Timetick_ms);

//    ROS_INFO("curtime = %lf\n",ros::Time::now().toSec());
// }

/*��ȡ��ǰʱ��*/
double Get_Curtime(void)
{
	// return Timetick_ms;
	return ros::Time::now().toSec();
}

/*�ж��Ƿ�ʱ*/
bool is_Timeout(double* lasttime,uint32_t time)
{
	double curtime;
	curtime = Get_Curtime();
	if((curtime - *lasttime) * 1000 >= time)  //������ʱ��ʱ��
	{
		*lasttime = curtime;  //����ǰ��ʱ�丳ֵ����һ��ʱ��
		return true;  //��ʱ����true
	}
	else
		return false;  //δ��ʱ����false
}








