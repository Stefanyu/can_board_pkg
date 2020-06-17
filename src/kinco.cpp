#include "kinco.h"
#include "string.h"
#include "stdlib.h"
#include <math.h>
#include "data_holder.h"
#include "stdio.h"
#include "base_driver.h"

can_board_pkg::motordata SMotoData;  //电机发送数据
can_board_pkg::motordata RMotoData;  //电机接收数据

FlagType Flag;

/*设置电机can数据帧格式*/
void Set_Sdo_Data(KincoType* sdo,uint16_t obj1,uint8_t cmd,int8_t* value)
{
      int len = 0;
      memset(sdo,0,sizeof(KincoType));
      switch(cmd)
      {
            case SDO_WRITE_1BYTE:
                  len = 1;
            break;
            case SDO_WRITE_2BYTE:
                  len = 2;
            break;
            case SDO_WRITE_4BYTE:

            case SDO_READ_CMD:
                  len = 4;
            break;
            default:
            break;

      }
      sdo->cmd = cmd;  
	sdo->obj1 = obj1;
	sdo->obj2 = 0;
      memcpy(sdo->sdata,value,len);
      sdo->len = 4 + len;
}

/*设置io数据格式*/
void Set_IO_Data(KincoIOType* io,uint8_t cmd,int8_t* value)
{
      memset(io,0,sizeof(KincoIOType));
      switch(cmd)
      {
            case 0xAA:  ///发送io控制命令
                  io->len = 1 + MOTORAMOUNT;  //5个数据长度
            break;
            case 0xBB:  //发送io读取命令
                  io->len = 1;  //1个数据长度
            break;
            default:
            break;

      }
      io->cmd = cmd;
      memcpy(io->sdata,value,MOTORAMOUNT); 
}

/*实际速度转为轮子的转速*/
void Speed_To_DEC(can_board_pkg::motordata* motordata)
{
   //vr = (wl + 2v) / 2   vl = (2v - wl) / 2
   motordata->leftspeed = (-motordata->xspeed * 2 + motordata->thspeed * WHEELINTERVAL) / 2; //���ֵ������ת�����ʵ�ʷ����෴������x��w�ٶȶ���Ҫȡ��
   motordata->rightspeed = (motordata->xspeed * 2 + motordata->thspeed * WHEELINTERVAL) / 2;
   
   //N = v*60 / (pi*d) * ���ٱ�
   motordata->leftrpm  = motordata->leftspeed * 60 / (M_PI * WHEELDIAMETER) * REDUCTIONRATIO;
   motordata->rightrpm = motordata->rightspeed * 60 / (M_PI * WHEELDIAMETER) * REDUCTIONRATIO;
   
   //��ת��ת��Ϊ��������֧�ֵĵ�λ��ת����ʽ�����ṩDEC=[(RPM*512*�������ֱ���)/1875];   
   motordata->leftdec  = motordata->leftrpm  * 512 * ENCODERRESOLUTION / 1875;  
   motordata->rightdec = motordata->rightrpm * 512 * ENCODERRESOLUTION / 1875; 
}

/*轮子转速转为实际速度*/
void DEC_To_Speed(can_board_pkg::motordata* motordata)
{

   //����������֧�ֵĵ�λת��Ϊת�٣�ת����ʽ�����ṩDEC=[(RPM*512*�������ֱ���)/1875]; 
   motordata->leftrpm = (float)motordata->leftdec / (512 * ENCODERRESOLUTION) * 1875.0 ;
   motordata->rightrpm = (float)motordata->rightdec  / (512 * ENCODERRESOLUTION) * 1875.0;
   
   //N = (v*60) / (pi*d) * ���ٱ�
   motordata->leftspeed = motordata->leftrpm * M_PI * WHEELDIAMETER / 60 / REDUCTIONRATIO;
   motordata->rightspeed = motordata->rightrpm * M_PI * WHEELDIAMETER / 60 / REDUCTIONRATIO;

   //vr = (wl + 2v) / 2  vl = (2v - wl) / 2
   motordata->xspeed = (-motordata->leftspeed + motordata->rightspeed) / 2;
   motordata->thspeed = (motordata->leftspeed - (-motordata->rightspeed)) / WHEELINTERVAL;
//   printf("l = %f r = %f x = %f th = %f\r\n",motordata->leftspeed,motordata->rightspeed,motordata->xspeed,motordata->thspeed);
}

/*设置pdo数据*/
void Set_Pdo_Data(int8_t* data,int8_t* data1,int8_t* data2)
{
	// sprintf((char*)data,"%s%s",(char*)data1,(char*)data2);
      memcpy(&data[0],data1,2);
      memcpy(&data[2],data2,4);

      // printf("pdo data = ");
      // for(int i = 0;i < 6;i ++)
      //       printf(" %02x ",data[i]);
      // printf("\n");
}

