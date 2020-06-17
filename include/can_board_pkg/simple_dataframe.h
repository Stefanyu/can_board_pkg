#ifndef PIBOT_SIMPLE_DATAFRAME_H_
#define PIBOT_SIMPLE_DATAFRAME_H_

#include "dataframe.h"
#include <string.h>
#include "kinco.h"

static const unsigned short MESSAGE_BUFFER_SIZE = 255;

#define FIX_HEAD 0x5A

#pragma pack(1)
#if MYCANBOARD
struct Head{
    unsigned char flag;// 头部标记,固定值:0X5A
    unsigned short int msg_id;// 消息ID,表示消息具体作用,决定消息体具体格式
    unsigned char length;// 消息体长度
};
#else
struct Head{
    unsigned char flag;// 头部标记,固定值:0X5A
    unsigned char msg_id;// 消息ID,表示消息具体作用,决定消息体具体格式
    unsigned char length;// 消息体长度
};
#endif
#pragma pack()




struct Message{
    struct Head head;
    unsigned char data[MESSAGE_BUFFER_SIZE];
    unsigned char check;
    unsigned char recv_count;//已经接收的字节数

    Message(){}
    #if MYCANBOARD
    Message(unsigned short int msg_id, unsigned char* data=0,unsigned char len=0){
        head.flag = FIX_HEAD;
        head.msg_id = msg_id;
        head.length = recv_count = len;
        check = 0;

        if (data != 0 && len !=0)
            memcpy(this->data, data, len);
        
        unsigned char* _send_buffer = (unsigned char*)this;

        unsigned int i = 0;
        for (i = 0; i < sizeof(head)+head.length; i++)
            check += _send_buffer[i];
        
        _send_buffer[sizeof(head)+head.length] = check;
    }
    #else
    Message(unsigned char msg_id, unsigned char* data=0,unsigned char len=0){
        head.flag = FIX_HEAD;
        head.msg_id = msg_id;
        head.length = recv_count = len;
        check = 0;

        if (data != 0 && len !=0)
            memcpy(this->data, data, len);
        
        unsigned char* _send_buffer = (unsigned char*)this;

        unsigned int i = 0;
        for (i = 0; i < sizeof(head)+head.length; i++)
            check += _send_buffer[i];
        
        _send_buffer[sizeof(head)+head.length] = check;
    }
    #endif
};

enum RECEIVE_STATE{
    STATE_RECV_FIX=0,
    STATE_RECV_ID,
    STATE_RECV_LEN,
    STATE_RECV_DATA,
    STATE_RECV_CHECK
};

#endif