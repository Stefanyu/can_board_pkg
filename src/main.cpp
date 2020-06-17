#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>
#include "tcptalk.h"
#include "timer_ros.h"
#include "base_driver.h"
#include "mythread.h"
/**
 * 2020-4-17 v4 yyy
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{

  ros::init(argc, argv, "cancommunication");

  ros::NodeHandle n;
  Exit_Signal_Init();
  Tcp_Client_Init();  
  BaseDriver::Instance();
  Init_Thread();
  ros::Rate loop_rate(1000);
  while(ros::ok())
  {
      BaseDriver::Instance()->Work_Loop();  //循环检测需要发布的ros数据
      ros::spinOnce();
      
      loop_rate.sleep();
  }


  return 0;
}
