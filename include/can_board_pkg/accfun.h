#ifndef __ACCFUN_H
#define __ACCFUN_H

#define CALCSPEEDTIME  10 
#define MAXSPEEDX      1.0f  
#define MAXSPEEDW      1.57f   
#define SLOWSPEED      0.2f
#define ACCSPEEDX      0.7f
#define ACCSPEEDW      90  


void Acc_Fun(float* curv,float* tarv,int cmd);

#endif