#include "base_driver_config.h"

#include "data_holder.h"
#define PI 3.1415926f


BaseDriverConfig::BaseDriverConfig(ros::NodeHandle &p) : pn(p)
{
#ifdef USE_DYNAMIC_RECONFIG
  param_update_flag = false;
#endif

  set_flag = true;
}


BaseDriverConfig::~BaseDriverConfig()
{
    
}

void BaseDriverConfig::init(Robot_parameter* r) 
{
  rp = r;

  //comm param
  // pn.param<std::string>("port", port, "/dev/ttyUSB0");
  // pn.param<int32_t>("buadrate", buadrate, 115200);
  
  // ROS_INFO("port:%s buadrate:%d", port.c_str(), buadrate);

  pn.param<std::string>("base_frame", base_frame, "/base_footprint");

  // pn.param<bool>("out_pid_debug_enable", out_pid_debug_enable, false);
  // ROS_INFO("out_pid_debug_enable:%d", out_pid_debug_enable);

  //topic name param
  pn.param<std::string>("cmd_vel_topic", cmd_vel_topic, "/cmd_vel_chassis");
  pn.param<std::string>("odom_topic", odom_topic, "odom");

}

void BaseDriverConfig::SetRobotParameters() 
{
#ifdef USE_DYNAMIC_RECONFIG
  static bool flag=true;
  if (flag)
  {
    flag = false;
    f = boost::bind(&BaseDriverConfig::dynamic_callback, this, _1, _2);
    server.setCallback(f);
  }
#endif
}

#ifdef USE_DYNAMIC_RECONFIG
void BaseDriverConfig::dynamic_callback(tcptest_pkg::moto_parametreConfig &config, uint32_t level) {

  rp->wheel_diameter = config.wheel_diameter;
  rp->wheel_track = config.wheel_track;
  rp->do_pid_interval = config.do_pid_interval;
  rp->encoder_resolution = config.encoder_resolution;
  rp->kp = config.kp;
  rp->ki = config.ki;
  rp->kd = config.kd;
  rp->ko = config.ko;
  rp->cmd_last_time = config.cmd_last_time;
  rp->max_v_liner_x = config.max_v_liner_x;
  rp->max_v_liner_y = config.max_v_liner_y;
  rp->max_v_angular_z = config.max_v_angular_z;

  param_update_flag = true;
}

bool BaseDriverConfig::get_param_update_flag()
{
  bool tmp=param_update_flag;
  param_update_flag = false;
  
  return tmp;
}
#endif
