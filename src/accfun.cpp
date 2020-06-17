#include "accfun.h"
#include "stdio.h"
#include "math.h"
#define ACCINTERVALX (ACCSPEEDX * CALCSPEEDTIME / 1000)   
#define ACCINTERVALW (ACCSPEEDW * (3.14 / 180) * CALCSPEEDTIME / 1000)  


void Acc_Fun(float* curv,float* tarv,int cmd)
{
	static float tmpvx = 0;
	static float tmpw = 0;
	float targetvx,targetw;
	if(cmd == 1)  //stop
	{
		tmpvx = 0;
		targetw = 0;
		targetvx = 0;
		targetw = 0;
	}
	else if(cmd == 2)  //slow
	{
		targetvx = (tarv[0] > SLOWSPEED ? SLOWSPEED : tarv[0]);
		targetw  = (tarv[1] > MAXSPEEDW ? MAXSPEEDW : tarv[1]);
	}
	else
	{
		targetvx = (tarv[0] > MAXSPEEDX ? MAXSPEEDX : tarv[0]);
		targetw  = (tarv[1] > MAXSPEEDW ? MAXSPEEDW : tarv[1]);
	}

	if(targetvx != tmpvx)
	{
		 if(targetvx > tmpvx)
		 {
				tmpvx += ACCINTERVALX;
		 }
		 else if(targetvx < tmpvx)
		 {
				tmpvx -= ACCINTERVALX;
		 }
		 if(fabs(targetvx - tmpvx) < (ACCINTERVALX-0.001))
		 {
				tmpvx = targetvx;
		 }
	}
		 
	if(targetw != tmpw)
	{
		 if(targetw > tmpw)
		 {
				tmpw += ACCINTERVALW;
		 }
		 else if(targetw < tmpw)
		 {
				tmpw -= ACCINTERVALW;
		 }
		 if(fabs(targetw - tmpw) < (ACCINTERVALW - 0.001))
		 {
				tmpw = targetw;
		 }
	}
	
	curv[0] =  tmpvx;
	curv[1] =  tmpw;
}


