#ifndef PIBOT_DATA_FRAME_H_
#define PIBOT_DATA_FRAME_H_

#include "kinco.h"

enum MESSAGE_ID{
    ID_LEFT_MOTOR = 1,
    ID_RIGHT_MOTOR = 2,
    ID_CAN_BOARD = 8,
    ID_READ_MOTOR_IO = 9,
    ID_SEND_MOTOR_IO = 10,
    ID_SEND_LEFT_SPEED = 11,
    ID_SEND_RIGHT_SPEED = 12,
    ID_READ_LEFT_SPEED = 13,
    ID_READ_RIGHT_SPEED = 14,
    ID_READ_LEFT_ENCODER = 15,
    ID_READ_RIGHT_ENCODER = 16,
    ID_SEND_LEFT_MODE = 17,
    ID_SEND_RIGHT_MODE = 18,
    ID_SEND_HEART_BEAT = 19,
    ID_MESSGAE_MAX
};

class Notify{
    public:
        virtual void update(const MESSAGE_ID id, void* data) = 0;
};


class Dataframe{
    public:
        virtual bool init()=0;
        virtual void register_notify(const MESSAGE_ID id, Notify* _nf)=0;
        virtual bool data_recv(unsigned char c)=0;
        virtual bool data_parse()=0;
        virtual bool interact(const MESSAGE_ID id)=0;
        virtual bool recv_proc()=0;
};

#endif
