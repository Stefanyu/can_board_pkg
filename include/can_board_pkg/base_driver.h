#ifndef __BASE_DRIVER_H
#define __BASE_DRIVER_H

#include <ros/ros.h>

#include <boost/shared_ptr.hpp>
#include "base_driver_config.h"


#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/Imu.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Char.h>
#include <std_msgs/UInt32.h>
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/String.h>

#include "dataframe.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "kinco.h"
// #include "m_odom.h"

//#define MYSMOOTH 1
#define SPEED_REV_OUT_TIME (200 / CALCSPEEDTIME)

class BaseDriver
{
private:
  BaseDriver();

public:
  static BaseDriver* instance;

  static BaseDriver* Instance()
  {
    if (instance == NULL)
      instance = new BaseDriver();

    return instance;
  }
  ~BaseDriver();
  void Work_Loop();
// private:
  void Init_Ros_Topic();

  // void Agv_Speed_Cb(const mymsgs_pkg::motordata& motordata);
  void Agv_Speed_Cb(const geometry_msgs::Twist& speed);
  void Agv_Status_Cb(const std_msgs::UInt32& status);
  void RemoteCtr_Speed_Cb(const geometry_msgs::Twist& speed);
  void Obstacle_Cb(const std_msgs::UInt8MultiArray& obstacle);
  void State_Cb(const std_msgs::String& state);

  int Send_Instance_Speed();
  int Send_Smooth_Speed();
  int Read_Speed();
  int Read_Encoder();
  int Send_Read_Motor_IO_Cmd();
  int Read_Motor_IO_Status();
  int Send_Motor_IO();
  bool Enable_Motor();
  bool Disable_Motor();
  int Send_Motor_Mode();
  void Ros_Publish_Data();
  int Send_Heart_Beat();

public:

  BaseDriverConfig& getBaseDriverConfig(){
    return bdg;
  }

  ros::NodeHandle* getNodeHandle(){
    return &nh;
  }

  ros::NodeHandle* getPrivateNodeHandle(){
    return &pn;
  }

  BaseDriverConfig bdg;
  boost::shared_ptr<Dataframe> frame;

private:
  ros::Publisher motor_status_pub;  //电机状态发布
  ros::Publisher motor_dec_pub;  //电机转速发布
  ros::Publisher motor_encoder_pub;  //电机编码器值发布
  ros::Publisher odom_pub;


  ros::Subscriber agv_speed_sub;  //agv速度订阅
  ros::Subscriber agv_status_sub;  //agv状态字订阅
  ros::Subscriber remotectr_data_sub;  //遥控器数据订阅
  ros::Subscriber obstacle_sub;  
  ros::Subscriber ubt_state_sub;
  ros::NodeHandle nh;
  ros::NodeHandle pn;

	float target_speed_x;
	float target_speed_w;
	uint8_t out_time;
  uint8_t ObstacleStatus;
  std_msgs::String State;
  // m_Odom mOdom;
  // nav_msgs::Odometry Odom;

};

#endif
