#include "signalGen.h"
#include "stdio.h"

unsigned int fullperiod,halfperiod;

int freq;
int sita1Deg,sita2Deg,phiDeg;
int sita1Cmp,sita2Cmp,phiCmp;
int dt1,dt2,dt3,dt4;

void signalGen(int a, int timer)
{
	int cmp1,cmp2;
	if((a+halfperiod)>fullperiod)
		cmp2=a-halfperiod;
	else
		cmp2=a+halfperiod;
	
	cmp1=a;
	if(cmp1<300) cmp1=300;
	if(cmp2<300) cmp2=300;
	__HAL_HRTIM_SetPeriod(&hhrtim1, timer, fullperiod);
	__HAL_HRTIM_SetCompare(&hhrtim1, timer, HRTIM_COMPAREUNIT_1, cmp1);
	__HAL_HRTIM_SetCompare(&hhrtim1, timer, HRTIM_COMPAREUNIT_2, cmp2);
}

void signalCal(int freq, int sita1, int sita2, int phi, int dt1,int dt2, int dt3,int dt4)
{
	
  HRTIM_DeadTimeCfgTypeDef pDeadTimeCfg = {0};
	
	fullperiod=clock/freq;
	halfperiod=fullperiod/2;
	sita1Cmp=sita1Deg*fullperiod/3600;
	sita2Cmp=sita2Deg*fullperiod/3600;
	phiCmp=phiDeg*fullperiod/3600;
	
	pDeadTimeCfg.Prescaler = HRTIM_TIMDEADTIME_PRESCALERRATIO_MUL8;
  pDeadTimeCfg.RisingValue = dt1;
  pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
  pDeadTimeCfg.RisingLock = HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
  pDeadTimeCfg.RisingSignLock = HRTIM_TIMDEADTIME_RISINGSIGNLOCK_WRITE;
  pDeadTimeCfg.FallingValue = dt2;
  pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
  pDeadTimeCfg.FallingLock = HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;
  pDeadTimeCfg.FallingSignLock = HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_WRITE;
  if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, &pDeadTimeCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, &pDeadTimeCfg) != HAL_OK)
  {
    Error_Handler();
  }
	
	pDeadTimeCfg.RisingValue = dt3;
	pDeadTimeCfg.FallingValue = dt4;
  if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, &pDeadTimeCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, &pDeadTimeCfg) != HAL_OK)
  {
    Error_Handler();
  }
	
	__HAL_HRTIM_SetPeriod(&hhrtim1,HRTIM_TIMERINDEX_MASTER,fullperiod);
	
	signalGen(300,HRTIM_TIMERINDEX_TIMER_A);
	signalGen(300+sita1Cmp,HRTIM_TIMERINDEX_TIMER_B);
	signalGen(300+phiCmp+(sita1Cmp-sita2Cmp)/2,HRTIM_TIMERINDEX_TIMER_E);
	signalGen(300+phiCmp+(sita1Cmp+sita2Cmp)/2,HRTIM_TIMERINDEX_TIMER_F);
}

void signal_init()
{
	freq=initFreq;
	sita1Deg=initSita1Deg;
	sita2Deg=initSita2Deg;
	phiDeg=initPhiDeg;
	dt1=initDTDeg;
	dt2=initDTDeg;
	dt3=initDTDeg;
	dt4=initDTDeg;
	signalCal(freq,sita1Deg,sita2Deg,phiDeg,dt1,dt2,dt3,dt4);
}
