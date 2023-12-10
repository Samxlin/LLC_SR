#ifndef __SIGNALGEN_H
#define __SIGNALGEN_H	 

#include "hrtim.h"

#define clock 54400000

#define initFreq 850
#define initSita1Deg 0
#define initSita2Deg 0
#define initPhiDeg 1800

#define initDTDeg 136

#define initPeriod clock/initFreq

#define fullDeg 3600


#define MINFREQVALUE 840
#define MAXFREQVALUE 2000
#define MINSitaValue 0
#define MAXSitaValue 1750
#define MINPhiValue 50
#define MAXPhiValue 1850


#define MINDTValue 2
#define MAXDTValue 200

extern int freq,sita1Deg,sita2Deg,phiDeg;
extern int dt1,dt2,dt3,dt4;

void signal_init(void);
void signalCal(int freq, int sita1, int sita2, int phi, int dt1, int dt2, int dt3, int dt4);

typedef enum
{
	FREQ=0,
	SITA1 =1,
	SITA2 =2,
	PHI=3,
	DT1=4,
	DT2=5,
	DT3=6,
	DT4=7
} angleTypeDef;

#endif
