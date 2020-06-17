#include "base_driver.h"
#include "data_holder.h"
#include <std_msgs/Float32MultiArray.h>
// #include "serial_transport.h"
#include "simple_dataframe_master.h"
#include "timer_ros.h"
#include "tcptalk.h"
#include "can_board_pkg/motordata.h"
#include <geometry_msgs/Twist.h>
#include "accfun.h"
#include "std_msgs/Int32MultiArray.h"

BaseDriver* BaseDriver::instance = NULL;

BaseDriver::BaseDriver() : pn("~"), bdg(pn)
{
    frame = boost::make_shared<Simple_dataframe>();
    Init_Ros_Topic();
}

BaseDriver::~BaseDriver()
{
    if (instance != NULL)
        delete instance;
}

/*初始化ros话题*/
void BaseDriver::Init_Ros_Topic()
{
    motor_status_pub = nh.advertise<std_msgs::UInt32>("MotorStatus",1);
    motor_dec_pub = nh.advertise<can_board_pkg::motordata>("MotorDec",1);
    motor_encoder_pub = nh.advertise<std_msgs::Int32MultiArray>("MotorEncoder",1);
    odom_pub = nh.advertise<nav_msgs::Odometry>("odom",1);

    agv_status_sub = nh.subscribe("AgvStatus",1,&BaseDriver::Agv_Status_Cb,this);
    // agv_speed_sub = nh.subscribe("smooth_cmd_vel",1,&BaseDriver::Agv_Speed_Cb,this); //change by LQ 2019-12-18:from cmd_vel to State_Machine_cmd_vel
    // remotectr_data_sub = nh.subscribe("State_Machine_cmd_vel_handle",1,&BaseDriver::RemoteCtr_Speed_Cb,this); //change by LQ 2019-12-18:from cmd_vel to State_Machine_cmd_vel
    remotectr_data_sub = nh.subscribe("State_Machine_cmd_vel",1,&BaseDriver::RemoteCtr_Speed_Cb,this); //change by LQ 2019-12-18:from cmd_vel to State_Machine_cmd_vel
    obstacle_sub = nh.subscribe("nav_obstacle",1,&BaseDriver::Obstacle_Cb,this);
    ubt_state_sub = nh.subscribe("state",1,&BaseDriver::State_Cb,this);
}

void BaseDriver::State_Cb(const std_msgs::String& state)
{
    State = state;
    // printf("current state = %s\n",State.data.c_str());
}

void BaseDriver::Obstacle_Cb(const std_msgs::UInt8MultiArray& obstacle)
{
    bool flag = false;
    for(int i = 1;i <= 3;i ++)
    {
        for(int j = 0;j < 4;j ++)
        {
            if(obstacle.data[j] == i)
            {
                ObstacleStatus = i;
                flag = true;
                break;
            }
            else
                ObstacleStatus = 0;
        }
        if(flag == true)
            break;
    }
    // printf("ObstacleStatus = %d\n",ObstacleStatus);
}

/*agv状态订阅回调*/
void BaseDriver::Agv_Status_Cb(const std_msgs::UInt32& status)
{
    //前急停、后急停、前防撞、后防撞触发
    if(Flag._sendmotoriosuc == false && 
        ((status.data & (1<<7)) || (status.data & (1<<8)) || (status.data & (1<<9)) || (status.data & (1<<10))))
    {
        Flag._iotriggersuc = true;  //io触发
        Flag._sendmotoriosuc = true;
        // printf("io pull up\n");
    }
    //停止触发且停止已经全部解除
    else if(Flag._sendmotoriosuc == false && 
        !((status.data & (1<<7)) || (status.data & (1<<8)) || (status.data & (1<<9)) || (status.data & (1<<10))))
    {
        Flag._iotriggersuc = false;  //io触发解除
        Flag._sendmotoriosuc = true;
        // printf("io pull down status = %d\n",status.data);
    }
    
    if(status.data & (1 << 11))
        Flag._manualctrl = true;  //手动模式
    else
        Flag._manualctrl = false;  //自动模式

}

/*agv速度订阅回调*/
void BaseDriver::Agv_Speed_Cb(const geometry_msgs::Twist& speed)
{
    can_board_pkg::motordata agv;
    int8_t data1[2] = {0x0F,0x00};
    int xtmp = (int)(speed.linear.x * 1000);
    int thtmp = (int)(speed.angular.z * 1000);
    // agv.xspeed = speed.linear.x;
    // agv.thspeed = speed.angular.z;
    agv.xspeed = (float)xtmp / 1000;
    agv.thspeed = (float)thtmp / 1000;
    Speed_To_DEC(&agv);

    //将速度值压入到can帧
    #if MYCANBOARD
        Set_Pdo_Data(Data_holder::get()->S_leftdecpdo,data1,(int8_t*)&agv.leftdec);
        Set_Pdo_Data(Data_holder::get()->S_rightdecpdo,data1,(int8_t*)&agv.rightdec);
    #else
        Set_Sdo_Data(&Data_holder::get()->S_leftdecsdo,SENDSPEEDOBJ1,SDO_WRITE_4BYTE,(int8_t*)&agv.leftdec);
        Set_Sdo_Data(&Data_holder::get()->S_rightdecsdo,SENDSPEEDOBJ1,SDO_WRITE_4BYTE,(int8_t*)&agv.rightdec);
    #endif
    Flag._speedsendsuc = true;  //允许速度发送

}

void BaseDriver::RemoteCtr_Speed_Cb(const geometry_msgs::Twist& speed)
{
	out_time = 0;  //接收到数据清超时计时器
    int xtmp = (int)(speed.linear.x * 1000);
    int thtmp = (int)(speed.angular.z * 1000);
    
    
    target_speed_x = (float)xtmp / 1000;
    target_speed_w = (float)thtmp / 1000;
    // target_speed_x = speed.linear.x;
    // target_speed_w = speed.angular.z;
}






/*发送立即速度速度*/
int BaseDriver::Send_Instance_Speed()
{
    if (Flag._speedsendsuc == true) //需要发送速度
    {
        frame->interact(ID_SEND_LEFT_SPEED);
        frame->interact(ID_SEND_RIGHT_SPEED);
        Flag._speedsendsuc = false;  //发送速度成功
        return 0;
    }
}
/*发送平滑速度*/
int BaseDriver::Send_Smooth_Speed()
{
	static bool sflag = false;
	float tarv[2];
	float curv[2];
	int8_t data1[2] = {0x0F,0x00};
	can_board_pkg::motordata agvspeed;
	
	if(out_time <= SPEED_REV_OUT_TIME)	
		out_time ++;
	else //接收速度超时
	{
		target_speed_x = 0;
		target_speed_w = 0;
	}
	
    int tartmpx = (int)(target_speed_x * 100);
    int tartmpw = (int)(target_speed_w * 100);
	tarv[0] = (float)tartmpx / 100;
	tarv[1] = (float)tartmpw / 100;

    if(State.data == "UBT_WAIT")
    {
        Acc_Fun(curv,tarv,0);
    }
    else
    {
        if(ObstacleStatus == 1)
        {
            Acc_Fun(curv,tarv,1);
        }
        else if(ObstacleStatus == 2)
        {
            Acc_Fun(curv,tarv,2);
        }
        // else
        // {
        //     Acc_Fun(curv,tarv,0);
        // }
    }
	// Acc_Fun(curv,tarv);

	if(((curv[0] != 0 || curv[1] != 0) && sflag == false))
	{
		sflag = true;
	}
	if(sflag == true)
	{
		agvspeed.xspeed  = curv[0];
		agvspeed.thspeed = curv[1];
		
		Speed_To_DEC(&agvspeed);
		// printf("agvspeed.leftdec = %d\n",agvspeed.leftdec);
        // printf("agvspeed.rightdec = %d\n",agvspeed.rightdec);
		Set_Pdo_Data(Data_holder::get()->S_leftdecpdo,data1,(int8_t*)&agvspeed.leftdec);
        Set_Pdo_Data(Data_holder::get()->S_rightdecpdo,data1,(int8_t*)&agvspeed.rightdec);
		
		frame->interact(ID_SEND_LEFT_SPEED);
		frame->interact(ID_SEND_RIGHT_SPEED);
		if(curv[0] == 0 && curv[1] == 0) //避免重复发送0
		{
			sflag = false;
		}
	}
  return 0;
}


/*读取电机速度*/
int BaseDriver::Read_Speed()
{
	if(Flag._lspeedreadsuc == true && Flag._rspeedreadsuc == true)
	{		
        DEC_To_Speed(&RMotoData);
        motor_dec_pub.publish(RMotoData);
		Flag._lspeedreadsuc = false;  //速度读取成功
        Flag._rspeedreadsuc = false;  
	}

  return 0;
}

/*读取电机编码器值*/
int BaseDriver::Read_Encoder()
{
    // static bool initflag = false;  //the first data get flag 
    // static double lasttime = 0;
    // float interval; 
    std_msgs::Int32MultiArray encoder;
	if(Flag._lencoderreadsuc == true && Flag._rencoderreadsuc == true)
	{
        // if(initflag == true)
        // {
        //     interval = Get_Curtime() - lasttime;
        //     lasttime = Get_Curtime();

        //     printf("Odom curtime = %lf\n",Get_Curtime());
        //     mOdom.m_Calc_Odom(&Odom,RMotoData.leftencoder,RMotoData.rightencoder,interval);
        //     odom_pub.publish(Odom);
            encoder.data.push_back(RMotoData.leftencoder);
            encoder.data.push_back(RMotoData.rightencoder);
            motor_encoder_pub.publish(encoder);
            Flag._lencoderreadsuc = false;  //编码值读取成功
            Flag._rencoderreadsuc = false;
        // }
        // else
        // {
        //     mOdom.Get_Increment_Encoder(LEFTSTDID,RMotoData.leftencoder);  //give up first data
        //     mOdom.Get_Increment_Encoder(RIGHTSTDID,RMotoData.rightencoder);
        //     lasttime = Get_Curtime();
        //     initflag = true;
        // }
        // int32_t leftencoder = RMotoData.leftencoder;
        // int32_t rightencoder = RMotoData.rightencoder;
    
	}

  return 0;
}

/*读取电机io状态*/
int BaseDriver::Send_Read_Motor_IO_Cmd()
{
    frame->interact(ID_READ_MOTOR_IO);  //读取电机i0
    return 0;
}

int BaseDriver::Read_Motor_IO_Status()
{
    Data_holder* dh = Data_holder::get();
    if(Flag._readmotoriosuc == true)  //成功读取电机状态
    {
        std_msgs::UInt32 rdata;
        memcpy(&rdata.data,dh->R_IOStatus.sdata,MOTORAMOUNT);
        motor_status_pub.publish(rdata);
        Flag._readmotoriosuc = false;
    }
    return 0;
}

/*发送电机io状态*/
int BaseDriver::Send_Motor_IO()
{
    Data_holder* dh = Data_holder::get();
    static bool lasttrigger = false;
    uint8_t sdata[MOTORAMOUNT] = {0};
    // int i;
    bool ret;
    int8_t zero = 0;
    if(Flag._iotriggersuc != lasttrigger)  //只有在触发状态发生改变的时候才发送控制字，避免重复发送
    {
        if(Flag._sendmotoriosuc == true)  //可以发送电机io状态
        {
            if(Flag._iotriggersuc == true)  //io触发
            {
                // for(i = 0;i < MOTORAMOUNT;i ++)
                // {
                //     sdata[i] &= !(1 << 1);  //状态0电机失能
                // }
                // ret = Disable_Motor();
                // printf("Disable_Motor\n");
                Set_Sdo_Data(&Data_holder::get()->S_leftdecsdo,SENDSPEEDOBJ1,SDO_WRITE_4BYTE,(int8_t*)&zero);
                Set_Sdo_Data(&Data_holder::get()->S_rightdecsdo,SENDSPEEDOBJ1,SDO_WRITE_4BYTE,(int8_t*)&zero);
                Flag._speedsendsuc = true;  //允许速度发送
            }
            else  //io触发解除
            {
                // for(i = 0;i < MOTORAMOUNT;i ++)
                // {
                //     sdata[i] |= (1 << 1); //状态1电机使能
                // }
                // ret = Disable_Motor();
                // ret = Enable_Motor();
                // printf("Enable_Motor\n");
            }
            // Set_IO_Data(&dh->S_IOStatus,SENDIOSTATUS,(int8_t *)sdata);
            // int ret = frame->interact(ID_SEND_MOTOR_IO);  //发送电机控制io
            if(ret == true)  //发送成功
            {
                lasttrigger = Flag._iotriggersuc;  //记录当前状态
                Flag._sendmotoriosuc = false;  //发送电机io状态结束
                return 0;
            }
            else  //发送失败
                return -1;
        }
    }
    else  //状态没变
    {
        Flag._sendmotoriosuc = false;  //发送电机io状态结束
        return 0;
    }
}

/*使能电机*/
bool BaseDriver::Enable_Motor()
{
      int i;
      Data_holder* dh = Data_holder::get();
      uint8_t sdata[MOTORAMOUNT] = {0};
      for(i = 0;i < MOTORAMOUNT;i ++)
      {
           sdata[i] |= (1 << 1); //状态1电机使能
      }
      Set_IO_Data(&dh->S_IOStatus,SENDIOSTATUS,(int8_t *)sdata);
      bool ret = frame->interact(ID_SEND_MOTOR_IO);  //发送电机控制io
      return ret;
}

/*失能电机*/
bool BaseDriver::Disable_Motor()
{
      int i;
      Data_holder* dh = Data_holder::get();
      uint8_t sdata[MOTORAMOUNT] = {0};
      for(i = 0;i < MOTORAMOUNT;i ++)
      {
            sdata[i] &= !(1 << 1);  //状态0电机失能
      }
      Set_IO_Data(&dh->S_IOStatus,SENDIOSTATUS,(int8_t *)sdata);
      bool ret = frame->interact(ID_SEND_MOTOR_IO);  //发送电机控制io
      return ret;
}

/*发送电机模式*/
int BaseDriver::Send_Motor_Mode()
{
    bool lret,rret;
    int8_t mode = INSTANTMODE;
    Set_Sdo_Data(&Data_holder::get()->S_leftmodesdo,WORKMODEOBJ1,SDO_WRITE_1BYTE,(int8_t*)&mode);
    Set_Sdo_Data(&Data_holder::get()->S_rightmodesdo,WORKMODEOBJ1,SDO_WRITE_1BYTE,(int8_t*)&mode);

    printf("send motor succeed!\n");
    lret = frame->interact(ID_SEND_LEFT_MODE);
    // if(lret == true && Flag._lmodesendsuc == true)  //左轮发送速度成功
    // {
    //     Flag._lmodesendsuc = false;
    // }
    // else
    // {
    //     // printf("send left mode error!lret = %d Flag._lmodesendsuc = %d\n",lret,Flag._lmodesendsuc);
    //     return -1;
    // }

    rret = frame->interact(ID_SEND_RIGHT_MODE);
    // if(rret == true && Flag._rmodesendsuc == true)  //左轮发送速度成功
    // {
    //     Flag._rmodesendsuc = false;
    // }
    // else
    //     return -1;
    return 0;
}

void BaseDriver::Work_Loop()
{
    Read_Speed();
    Read_Encoder();
    Read_Motor_IO_Status();
}

/*ros发布数据*/
//void BaseDriver::Ros_Publish_Data()
//{
//    Data_holder* dh = Data_holder::get();
//    if(Flag._lspeedreadsuc == true && Flag._rspeedreadsuc == true)  //左右轮速度读取成功
//    {
//        DEC_To_Speed(&RMotoData);
//        motor_dec_pub.publish(RMotoData);
//        Flag._lspeedreadsuc = false;  //速度读取成功
//        Flag._rspeedreadsuc = false;
//    }
//    if(Flag._lencoderreadsuc == true && Flag._rencoderreadsuc == true)  //左右轮编码值读取成功
//    {
//        motor_encoder_pub.publish(RMotoData);
//        Flag._lencoderreadsuc = false;  //编码值读取成功
//        Flag._rencoderreadsuc = false;
//    }
//    if(Flag._readmotoriosuc == true)  //成功读取电机状态
//    {
//        std_msgs::UInt32 rdata;
//        memcpy(&rdata.data,dh->R_IOStatus.sdata,MOTORAMOUNT);
//        // printf("rdata.data = %x\n",rdata.data);
//        motor_status_pub.publish(rdata);
//        Flag._readmotoriosuc = false;
//    }
//}

int BaseDriver::Send_Heart_Beat()
{
    frame->interact(ID_SEND_HEART_BEAT);
    return 0;
}


