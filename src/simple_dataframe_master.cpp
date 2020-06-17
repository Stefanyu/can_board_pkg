#include "ros/ros.h"
#include "simple_dataframe_master.h"
#include "data_holder.h"
#include <stdio.h>
#include "tcptalk.h"
#include "timer_ros.h"
#include "kinco.h"
#include "motecmotor.h"
// #include "transport.h"
// Simple_dataframe::Simple_dataframe(Transport* _trans): trans(_trans){
//     recv_state = STATE_RECV_FIX;
// }
Simple_dataframe::Simple_dataframe(){
    recv_state = STATE_RECV_FIX;
}
Simple_dataframe::~Simple_dataframe(){
}

bool Simple_dataframe::send_message(const MESSAGE_ID id){
    Message msg(id);

    send_message(&msg);

    return true;
}

bool Simple_dataframe::send_message(const MESSAGE_ID id, unsigned char* data, unsigned char len){
    Message msg(id, data, len);
    // printf("id = %04x\n",id);
    send_message(&msg);

    return true;
}

bool Simple_dataframe::send_message(Message* msg){
    unsigned char* buf = (unsigned char*)msg;   
    int len = sizeof(msg->head) + msg->head.length + 1; 
    // ROS_INFO("msglen = %d\n",len);
    int ret = Tcp_Client_Write(buf,len);
    // printf("ret = %d\n",ret);
    // if(ret > 0)
    // {
    //     for(int i = 0;i < ret;i ++)
    //     {
    //         printf("%02x ",buf[i]);
    //     }
    // }
    // printf("\n");


    // if(ret < 0)  //第一次发送失败
    // {
    //     Tcp_Client_Close();
    //     Tcp_Client_Init();
    //     ret = Tcp_Client_Write(buf,len);
    //     if(ret < 0)  //第二次次发送失败
    //     {
    //         printf("tcp write error!\n");
    //         exit(-1);  //退出程序
    //     }
    //     else  //重新连接成功
    //     {
    //         printf("tcp reconnect succeed!111111111111111111111111\n");
    //         // exit(-1);
    //     }

    // }
    return true;
}

bool Simple_dataframe::init(){
    return true;
}

#if MYCANBOARD //采用自己的can模块进行通讯
bool Simple_dataframe::interact(const MESSAGE_ID id){
    int32_t zero = 0;
    unsigned char heartbeat = 0xCC;

    Data_holder* dh = Data_holder::get();
    // mymsgs_pkg::motordata ps2;
    switch (id){

    case ID_SEND_LEFT_SPEED:  //发送左轮速度
    	// send_message((MESSAGE_ID)PDO_SEND_SPEED_STD, (unsigned char*)dh->S_leftdecpdo,sizeof(dh->S_leftdecpdo));
		send_message((MESSAGE_ID)(PDO_SEND_SPEED_STD | LEFTSTDID), (unsigned char*)dh->S_leftdecpdo,sizeof(dh->S_leftdecpdo));
        break;
    case ID_SEND_RIGHT_SPEED:  //发送右轮速度
        // Send_Can_Frame((MESSAGE_ID)PDO_SEND_SPEED_STD,(unsigned char*)dh->S_rightdecpdo,sizeof(dh->S_rightdecpdo));

        send_message((MESSAGE_ID)(PDO_SEND_SPEED_STD | RIGHTSTDID),(unsigned char*)dh->S_rightdecpdo,sizeof(dh->S_rightdecpdo));
        break;
 

    case ID_SEND_MOTOR_IO:
        send_message((MESSAGE_ID)ID_CAN_BOARD, (unsigned char*)&dh->S_IOStatus,dh->S_IOStatus.len);
        break;
    case ID_SEND_LEFT_MODE:
        Send_Can_Frame((MESSAGE_ID)SDO_SEND_STD,(unsigned char*)&dh->S_leftmodesdo,dh->S_leftmodesdo.len);

        // Send_Can_Frame((SDO_SEND_STD | LEFTSTDID),(unsigned char*)&dh->S_leftmodesdo,dh->S_leftmodesdo.len);

        // send_message(ID_LEFT_MOTOR, (unsigned char*)&dh->S_leftmodesdo,dh->S_leftmodesdo.len);
        // printf("send left mode succeed!\n");
        break;
    case ID_SEND_RIGHT_MODE:
        Send_Can_Frame((MESSAGE_ID)SDO_SEND_STD,(unsigned char*)&dh->S_rightmodesdo,dh->S_rightmodesdo.len);

        // Send_Can_Frame((SDO_SEND_STD | RIGHTSTDID),(unsigned char*)&dh->S_rightmodesdo,dh->S_rightmodesdo.len);
        // send_message(ID_RIGHT_MOTOR, (unsigned char*)&dh->S_rightmodesdo,dh->S_rightmodesdo.len);
        // printf("send right mode succeed!\n");
        break;
    case ID_SEND_HEART_BEAT:
        send_message(ID_CAN_BOARD, (unsigned char*)&heartbeat,sizeof(heartbeat));
        break;
    default:
        break;
    }
    // mySleep_ms(10);
    // if (!recv_proc())
    //     return false;

    return true;
}
#else  //采用广成can模块进行通讯
bool Simple_dataframe::interact(const MESSAGE_ID id){
    int32_t zero = 0;
    Data_holder* dh = Data_holder::get();
    // mymsgs_pkg::motordata ps2;
    switch (id){

    case ID_SEND_LEFT_SPEED:  //发送左轮速度
        Send_Can_Frame(SDO_SEND_STD | LEFTSTDID,(unsigned char*)&dh->S_leftdecsdo,dh->S_leftdecsdo.len);
        // send_message(ID_LEFT_MOTOR, (unsigned char*)&dh->S_leftdecsdo,dh->S_leftdecsdo.len);
        // memcpy(&ps2.leftdec,dh->S_leftdecsdo.sdata,4);
        // DEC_To_Speed(&ps2);
        // printf("ps2.leftspeed = %f\n",ps2.leftspeed);
        // printf("send curtime = %lf\n",Get_Curtime());
        break;
    case ID_SEND_RIGHT_SPEED:  //发送右轮速度
        Send_Can_Frame(SDO_SEND_STD | RIGHTSTDID,(unsigned char*)&dh->S_rightdecsdo,dh->S_rightdecsdo.len);
        // send_message(ID_RIGHT_MOTOR, (unsigned char*)&dh->S_rightdecsdo,dh->S_rightdecsdo.len);
        break;
    // case ID_READ_LEFT_SPEED:  //读取左轮转速
    //     Set_Sdo_Data(&dh->R_leftdecsdo,READSPEEDOBJ1,SDO_READ_CMD,(int8_t*)&zero);
    //     Send_Can_Frame(SDO_SEND_STD | LEFTSTDID,(unsigned char*)&dh->R_leftdecsdo,dh->R_leftdecsdo.len);
    //     // send_message(ID_LEFT_MOTOR, (unsigned char*)&dh->R_leftdecsdo,dh->R_leftdecsdo.len);
    //     break;
    // case ID_READ_RIGHT_SPEED:  //读取右轮转速
    //     Set_Sdo_Data(&dh->R_rightdecsdo,READSPEEDOBJ1,SDO_READ_CMD,(int8_t*)&zero);
    //     Send_Can_Frame(SDO_SEND_STD | RIGHTSTDID,(unsigned char*)&dh->R_rightdecsdo,dh->R_rightdecsdo.len);

    //     // send_message(ID_RIGHT_MOTOR, (unsigned char*)&dh->R_rightdecsdo,dh->R_rightdecsdo.len);
    //     break;
    // case ID_READ_LEFT_ENCODER:
    //     Set_Sdo_Data(&dh->R_leftencodersdo,READENCODEROBJ1,SDO_READ_CMD,(int8_t*)&zero);
    //     send_message(ID_LEFT_MOTOR, (unsigned char*)&dh->R_leftencodersdo,dh->R_leftencodersdo.len);
    //     break;
    // case ID_READ_RIGHT_ENCODER:
    //     Set_Sdo_Data(&dh->R_rightencodersdo,READENCODEROBJ1,SDO_READ_CMD,(int8_t*)&zero);
    //     send_message(ID_RIGHT_MOTOR, (unsigned char*)&dh->R_rightencodersdo,dh->R_rightencodersdo.len);
    //     break;
    // case ID_READ_MOTOR_IO:
    //     Set_IO_Data(&dh->R_IOStatus,READIOSTATUS,(int8_t*)&zero);
    //     send_message(ID_CAN_BOARD,(unsigned char*)&dh->R_IOStatus,dh->R_IOStatus.len);
    //     break;
    case ID_SEND_MOTOR_IO:
        send_message(ID_CAN_BOARD, (unsigned char*)&dh->S_IOStatus,dh->S_IOStatus.len);
        break;
    case ID_SEND_LEFT_MODE:
        Send_Can_Frame(SDO_SEND_STD | LEFTSTDID,(unsigned char*)&dh->S_leftmodesdo,dh->S_leftmodesdo.len);

        // send_message(ID_LEFT_MOTOR, (unsigned char*)&dh->S_leftmodesdo,dh->S_leftmodesdo.len);
        // printf("send left mode succeed!\n");
        break;
    default:

        break;
    }
    // mySleep_ms(10);
    // if (!recv_proc())
    //     return false;

    return true;
}
#endif

// bool Simple_dataframe::recv_proc(){
//     int i=0;
//     unsigned char buf[MAXSIZE];
//     int len;
//     // trans->set_timeout(150);
//     bool got=false;
//     int ret;
//     // ros::Rate loop(10000);
//     Tasklasttime._board_recv_data_time = Get_Curtime();
//     memset(buf,0,MAXSIZE);
//     while(true){
//         len = Tcp_Client_Read(buf);
//         if(len > 0)
//         {
//             // printf("buf = %02x %02x %02x len =%d\n",buf[0],buf[1],buf[2],len);
//             for (int i=0;i<len;i++){
//                 if (data_recv(buf[i])){
//                     got = true;
//                     break;
//                 }
//             }
//             if (got)
//                 break;
          
//         }
//         if (is_Timeout(&Tasklasttime._board_recv_data_time,100))
//         {
//             std::cout << "timeout:" << std::endl;
//             return false;
//         }
       
//         // ros::spinOnce();
//         // loop.sleep();
//     }

//     ret = data_parse();
//     return ret;
// }
// bool Simple_dataframe::recv_proc(){
//     int i=0;
//     unsigned char buf[MAXSIZE];
//     int len;
//     // trans->set_timeout(150);
//     bool got=false;
//     int ret;
//     // ros::Rate loop(10000);
//     Tasklasttime._board_recv_data_time = Get_Curtime();
//     memset(buf,0,MAXSIZE);
//     while(true){
//         len = Tcp_Client_Read(buf);
//         if(len > 0)
//         {
//             printf("recv data! len = %d\n",len);
//             // printf("buf = %02x %02x %02x len =%d\n",buf[0],buf[1],buf[2],len);
//             for (int i=0;i<len;i++){
//                 if (data_recv(buf[i])){
//                     got = true;
//                     break;
//                 }
//             }
//             if (got)
//                 break;
          
//         }
//         else if(len == 0)
//         {
//             printf("read error num = %d error len = %d\n",errno,len);
//         }
//         // if (is_Timeout(&Tasklasttime._board_recv_data_time,100))
//         // {
//         //     std::cout << "timeout:" << std::endl;
//         //     return false;
//         // }
       
//         // ros::spinOnce();
//         // loop.sleep();
//     }

//     ret = data_parse();
//     return ret;
// }
bool Simple_dataframe::recv_proc(){
    int i=0;
    unsigned char buf[MAXSIZE];
    int len;
    bool got=false;
    int ret;
    Tasklasttime._board_recv_data_time = Get_Curtime();
    memset(buf,0,MAXSIZE);
    len = Tcp_Client_Read(buf);
    if(len > 0)
    {
        // printf("recv data! len = %d\n",len);
        // printf("buf = ");
        // printf("buf = %02x %02x %02x len =%d\n",buf[0],buf[1],buf[2],len);
        for (int i=0;i<len;i++){
            // printf("%02x ",buf[i]);
            if (data_recv(buf[i])){
                got = true;
                break;
            }
        }
        // printf("\n");
    }
    else 
    {
        // printf("read error num = %d error len = %d\n",errno,len);
        return false;
    }
    if(got == true)
        ret = data_parse();
    else
        ret = false;
    return ret;
}

#if MYCANBOARD
bool Simple_dataframe::data_recv(unsigned char c){
    static bool idflag = false;
    // printf("%02x ",c);

    switch (recv_state){
    case STATE_RECV_FIX:
        if (c == FIX_HEAD){
            memset(&active_rx_msg,0, sizeof(active_rx_msg));
            active_rx_msg.head.flag = c;
            active_rx_msg.check += c;
            recv_state = STATE_RECV_ID;
        }
        else
            recv_state = STATE_RECV_FIX;
        break;
    case STATE_RECV_ID:
        if(idflag == false)
        {
            active_rx_msg.head.msg_id = c;
            active_rx_msg.check += c;
            idflag = true;
        }
        else 
        {
            active_rx_msg.head.msg_id |= c << 8;
            active_rx_msg.check += c;
            idflag = false;
            recv_state = STATE_RECV_LEN;
        }
        break;
       
        // if (c < ID_MESSGAE_MAX){
        //     active_rx_msg.head.msg_id = c;
        //     active_rx_msg.check += c;
        //     recv_state = STATE_RECV_LEN;
        // }
        // else
        //     recv_state = STATE_RECV_FIX;
        // break;
    case STATE_RECV_LEN:
        active_rx_msg.head.length =c;
        active_rx_msg.check += c;
        if (active_rx_msg.head.length==0){
            recv_state = STATE_RECV_CHECK;
        }
        else
            recv_state = STATE_RECV_DATA;
        break;
    case STATE_RECV_DATA:
        active_rx_msg.data[active_rx_msg.recv_count++] = c;
        active_rx_msg.check += c;
        if (active_rx_msg.recv_count >=active_rx_msg.head.length)
            recv_state  = STATE_RECV_CHECK;
        break;
    case STATE_RECV_CHECK:
        recv_state = STATE_RECV_FIX;
        if (active_rx_msg.check == c){
            
            return true;
        }
        break;
    default:
        recv_state = STATE_RECV_FIX;
    }

    return false;
}
#else
bool Simple_dataframe::data_recv(unsigned char c){
    switch (recv_state){
    case STATE_RECV_FIX:
        if (c == FIX_HEAD){
            memset(&active_rx_msg,0, sizeof(active_rx_msg));
            active_rx_msg.head.flag = c;
            active_rx_msg.check += c;
            recv_state = STATE_RECV_ID;
        }
        else
            recv_state = STATE_RECV_FIX;
        break;
    case STATE_RECV_ID:
        if (c < ID_MESSGAE_MAX){
            active_rx_msg.head.msg_id = c;
            active_rx_msg.check += c;
            recv_state = STATE_RECV_LEN;
        }
        else
            recv_state = STATE_RECV_FIX;
        break;
    case STATE_RECV_LEN:
        active_rx_msg.head.length =c;
        active_rx_msg.check += c;
        if (active_rx_msg.head.length==0){
            recv_state = STATE_RECV_CHECK;
        }
        else
            recv_state = STATE_RECV_DATA;
        break;
    case STATE_RECV_DATA:
        active_rx_msg.data[active_rx_msg.recv_count++] = c;
        active_rx_msg.check += c;
        if (active_rx_msg.recv_count >=active_rx_msg.head.length)
            recv_state  = STATE_RECV_CHECK;
        break;
    case STATE_RECV_CHECK:
        recv_state = STATE_RECV_FIX;
        if (active_rx_msg.check == c){
            
            //printf("\r\n");
            return true;
        }
        break;
    default:
        recv_state = STATE_RECV_FIX;
    }

    return false;
}
#endif

#if MYCANBOARD
bool Simple_dataframe::data_parse(){
    MESSAGE_ID id = (MESSAGE_ID)active_rx_msg.head.msg_id;

    Data_holder* dh = Data_holder::get();
    // printf("\t\t\t\t\t\tcurtime = %lf = %d d1 = %x d2 = %x\n",Get_Curtime(),id,active_rx_msg.data[1],active_rx_msg.data[2]);
    switch (id){
        case ID_LEFT_MOTOR | PDO_READ_SPEED_ENCODER_STD:
            memcpy(&RMotoData.leftdec,&active_rx_msg.data[4],sizeof(RMotoData.leftdec)); 
            memcpy(&RMotoData.leftencoder,&active_rx_msg.data[0],sizeof(RMotoData.leftencoder)); 
            // DEC_To_Speed(&RMotoData);
            
            Flag._lspeedreadsuc = true;
            Flag._lencoderreadsuc = true;
            break;
        case ID_RIGHT_MOTOR | PDO_READ_SPEED_ENCODER_STD:
            memcpy(&RMotoData.rightdec,&active_rx_msg.data[4],sizeof(RMotoData.rightdec)); 
            memcpy(&RMotoData.rightencoder,&active_rx_msg.data[0],sizeof(RMotoData.rightencoder)); 

            // DEC_To_Speed(&RMotoData);
            
            Flag._rspeedreadsuc = true;
            Flag._rencoderreadsuc = true;

        return true;
    }
}
#else
bool Simple_dataframe::data_parse(){
    MESSAGE_ID id = (MESSAGE_ID)active_rx_msg.head.msg_id;

    Data_holder* dh = Data_holder::get();
    // printf("\t\t\t\t\t\tcurtime = %lf = %d d1 = %x d2 = %x\n",Get_Curtime(),id,active_rx_msg.data[1],active_rx_msg.data[2]);
    switch (id){
        case ID_LEFT_MOTOR:
            if(active_rx_msg.data[1] == 0xFF && active_rx_msg.data[2] == 0x60)  //左轮速度发送反馈
            {
                Flag._lspeedsendsuc = true;
                // printf("left speed send succeed!\n");
            }     
            else if(active_rx_msg.data[1] == 0x6C && active_rx_msg.data[2] == 0x60)  //左轮速度读取
            {
                memcpy(&RMotoData.leftdec,&active_rx_msg.data[4],sizeof(RMotoData.leftdec)); 
                // DEC_To_Speed(&RMotoData);
                // printf("RMotoData.leftdec = %d\n",RMotoData.leftdec);
                Flag._lspeedreadsuc = true;
            }
            else if(active_rx_msg.data[1] == 0x63 && active_rx_msg.data[2] == 0x60)  //获取左轮编码器值
            {
                memcpy(&RMotoData.leftencoder,&active_rx_msg.data[4],sizeof(RMotoData.leftencoder)); 
                Flag._lencoderreadsuc = true;
            }
            else if(active_rx_msg.data[1] == 0x60 && active_rx_msg.data[2] == 0x60)  //左轮状态设置成功
            {
                Flag._lmodesendsuc = true;
                // printf("left mode succeed! Flag._lmodesendsuc = %d\n", Flag._lmodesendsuc);

            }
            break;
        case ID_RIGHT_MOTOR:
            if(active_rx_msg.data[1] == 0xFF && active_rx_msg.data[2] == 0x60)  //左轮速度发送反馈
            {
                Flag._rspeedsendsuc = true;
            }
            else if(active_rx_msg.data[1] == 0x6C && active_rx_msg.data[2] == 0x60)  //左轮速度读取
            {
                memcpy(&RMotoData.rightdec,&active_rx_msg.data[4],sizeof(RMotoData.rightdec)); 
                Flag._rspeedreadsuc = true;
            }
            else if(active_rx_msg.data[1] == 0x63 && active_rx_msg.data[2] == 0x60)  //左轮速度读取
            {
                memcpy(&RMotoData.rightencoder,&active_rx_msg.data[4],sizeof(RMotoData.rightencoder)); 
                Flag._rencoderreadsuc = true;
            }
            else if(active_rx_msg.data[1] == 0x60 && active_rx_msg.data[2] == 0x60)  //左轮状态设置成功
            {
                Flag._rmodesendsuc = true;
                // printf("right mode succeed!\n");
            }
            break;
        case ID_CAN_BOARD:
            if(active_rx_msg.data[0] == READIOSTATUS)  
            {
                // printf("active_rx_msg.data[0] = %x\n",active_rx_msg.data[0]);
                memcpy(dh->R_IOStatus.sdata,&active_rx_msg.data[1],MOTORAMOUNT); 
                // printf("motordata =");
                // for(int i = 0;i < 4;i ++)
                // {
                //     printf("%x ",dh->R_IOStatus.sdata[i]);
                // }
                // printf("\n");
                Flag._readmotoriosuc = true;   //读取电机io成功
            }
            // else if(active_rx_msg.data[0] == ID_SEND_MOTOR_IO)
            // {

            // }

            break;

        return true;
    }
}
#endif



