#ifndef __KINCO_H
#define __KINCO_H

#include "stdint.h"
#include "can_board_pkg/motordata.h"

#define MYCANBOARD 1

#define MOTORAMOUNT 4  //电机数量

#define MAXMOTOSPEED 1.5  //最大速度
#define ENCODERRESOLUTION 65535  //编码器分辨率
#define REDUCTIONRATIO 20 //减速比
#define WHEELINTERVAL  0.462  //两轮间距 300kg
// #define WHEELINTERVAL  0.56544  //两轮间距 500kg
// #define WHEELINTERVAL  0.387  //两轮间距 150kg

#define WHEELDIAMETER  0.16  //轮子直径

#define SDO_WRITE_1BYTE	0X2F  //д1�ֽ�����
#define SDO_WRITE_2BYTE	0X2B  //д2�ֽ�����
#define SDO_WRITE_4BYTE	0X23  //д4�ֽ�����

#define SDO_WRITE_SUCESS 0X60  //д������ȷ����
#define SDO_WRITE_FAILED 0X80  //д���������

#define SDO_READ_1BYTE	0X4F  //��1�ֽ������
#define SDO_READ_2BYTE	0X4B  //��2�ֽ������
#define SDO_READ_4BYTE	0X43  //��4�ֽ������

#define SDO_READ_CMD    0X40  //读数据命令发送

#define SDO_SEND_STD	0x600  //����ͷ������Ҫ�ټ���������id
#define SDO_READ_STD	0X580  //����ͷ������Ҫ�ټ���������id

#define PDO_SEND_SPEED_STD 0x200  //发送速度的std
#define PDO_READ_SPEED_ENCODER_STD 0x180  //读取速度和编码器值的std

#define LEFTSTDID  1  //���ֵ�stdid
#define RIGHTSTDID 2  //���ֵ�stdid

#define WORKMODEOBJ1  0x6060  //����ģʽ����1
#define SENDSPEEDOBJ1 0x60FF  //�����ٶȶ���1
#define READSPEEDOBJ1 0x606C  //��ȡ�ٶȶ���1
#define READENCODEROBJ1 0x6063  //读取编码器值命令
#define CONTROLWORDOBJ1 0x6040  //�����ֶ���1

#define INSTANTMODE -3  //����ģʽ
#define ACCMODE      3  //�Ӽ���ģʽ

/*���������ڵ������ʹ��*/
#define MOTOOFF      0x06  //������
#define MOTOON       0x0F  // ����ϵ�

#define SENDIOSTATUS 0xAA  //发送io状态
#define READIOSTATUS 0xBB  //读取io状态


//���Ƶ��sdo��������
#pragma pack(1)
typedef struct
{
	uint8_t cmd;  //����
	uint16_t obj1;  //��������1��2�ֽ�
	uint8_t obj2;  //��������2��1�ֽ�
	uint8_t sdata[4];  //����
  uint8_t len;
}KincoType;


typedef struct
{
  uint8_t cmd;
  uint8_t sdata[MOTORAMOUNT]; 
  uint8_t len;
}KincoIOType;
#pragma pack()


typedef struct
{
  bool _agvspeedsend;

  bool _lspeedsendsuc;  //左轮速度发送标志
  bool _rspeedsendsuc;  //右轮速度发送标志
  bool _lspeedreadsuc;  //左轮速度读取标志
  bool _rspeedreadsuc;  //右轮速度读取标志
  bool _lencoderreadsuc;  //左轮编码器值读取标志
  bool _rencoderreadsuc;  //右轮编码器值读取标志
  bool _speedsendsuc;  //速度发送成功标志
  bool _sendmotoriosuc;  //电机io发送成功标志
  bool _readmotoriosuc;  //电机io读取成功标志
  bool _iotriggersuc;  //io触发成功标志
  bool _lmodesendsuc;  //左轮状态发送成功
  bool _rmodesendsuc;  //右轮状态发送成功
  bool _manualctrl;  //手动控制
}FlagType;



extern can_board_pkg::motordata SMotoData;  //������͵�����
extern can_board_pkg::motordata RMotoData;  //������͵�����
extern FlagType Flag;

/*�������ã����Ϳ�������������
* ������uint8_t stdid -> ��������Ӧ��id
*       uint16_t obj1 -> ��������1
*       uint8_t obj2  -> ��������2
*       uint8_t cmd   -> ��Ҫ���͵������ֽ�����Ӧ������
*       uint8_t* value -> ��Ҫ���͵�����
* ����ֵ��0 -> ���ͳɹ�
*         -1 -> ����ʧ��
*/
int8_t SendSdoCmd(uint8_t stdid,uint16_t obj1,uint8_t obj2,uint8_t cmd,int8_t* value);

/*�������ã����Ͷ�ȡ����������
* ������uint8_t stdid -> ��������Ӧ��id
*       uint16_t obj1 -> ��������1
*       uint8_t obj2  -> ��������2
* ����ֵ��0 -> ���ͳɹ�
*         -1 -> ����ʧ��
*/
int8_t SendSdoCmdREAD(uint8_t stdid,uint16_t obj1,uint8_t obj2);

/*�������ã�������decת��Ϊ����ʵ���ٶ�
// */
void DEC_To_Speed(can_board_pkg::motordata* motordata);

// /*�������ã���ʵ���ٶ�ת������dec
// */
void Speed_To_DEC(can_board_pkg::motordata* motordata);

/*�������ã����͵������ģʽ��һ���Ǵ�������ģʽ��-3*/
int8_t Send_Mode(uint8_t stdid,int8_t mode);

/*�������ã���ȡ���ӱ���ֵ*/
int8_t Read_Encoder(uint8_t stdid);

/*�������ã���ȡ�����ٶ�*/
int8_t Read_Speed(uint8_t stdid);

/*�������ã������ٶ�*/
int8_t Send_Speed(int32_t speed,uint8_t stdid);

/*�������ã����������ٶ�*/
void Update_Speed(float xspeed,float thspeed);

/*�ٶ���Ҫ���£�Ȼ���������ж�ʱ����*/
void Speed_Need_Update(float xspeed,float thspeed);

void Set_Sdo_Data(KincoType* sdo,uint16_t obj1,uint8_t cmd,int8_t* value);
/*设置io数据格式*/
void Set_IO_Data(KincoIOType* io,uint8_t cmd,int8_t* value);

/*使能电机*/
bool Enable_Motor();

/*失能电机*/
bool Disable_Motor();
/*设置pdo数据*/
void Set_Pdo_Data(int8_t* data,int8_t* data1,int8_t* data2);
#endif

