#ifndef __FUNCTION_H
#define __FUNCTION_H	 

#include "stm32h7xx_hal.h"
#include "hrtim.h"
#include "adc.h"
#include "key.h"
#include "oled.h"

#define ADC3_BUFFER_SIZE 2
extern uint16_t adc3_data[ADC3_BUFFER_SIZE];
extern struct  _ADI SADC;

extern struct  _FLAG	DF;

extern uint16_t OLEDShowCnt;

extern uint16_t DAC1_OUT1_Value;

#define MAX_DAC_OUT 4000
#define MIN_DAC_OUT 100

//函数声明
void ADCSample(void);
void StateM(void);
void StateMInit(void);
void StateMWait(void);
void StateMRise(void);
void StateMRun(void);
void StateMErr(void);

void ValInit(void);

void ShortOff(void);
void SwOCP(void);
void VinSwUVP(void);

void Signal_Init(void);
void Output_Update();

void LEDShow(void);
void OLEDShow(void);

/*****************************故障类型*****************/
#define     F_NOERR      			0x0000//无故障
#define     F_SW_SHORT    		0x0001//输出短路
#define     F_SW_VIN_UVP  		0x0002//输入欠压
#define     F_SW_IOUT_OCP    	0x0004//输出过流

//采样变量结构体
struct _ADI
{
	int32_t   Iout;//输出电流
	int32_t   IoutAvg;//输出电流平均值
	int32_t		IoutOffset;//输出电流采样偏置
	int32_t   Vout;//输出电电压
	int32_t   VoutAvg;//输出电电压平均值
};

#define CAL_VOUT_K	4101//Q12输入电压矫正K值
#define CAL_VOUT_B	49//Q12输入电压矫正B值
#define CAL_IOUT_K	4096//Q12输出电流矫正K值
#define CAL_IOUT_B	0//Q12输入电流矫正B值

//标志位定义
struct  _FLAG
{
	uint16_t	SMFlag;//状态机标志位
	uint16_t	CtrFlag;//控制标志位
	uint16_t  ErrFlag;//故障标志位
	uint8_t		BBFlag;//运行模式标志位，BUCK模式，BOOST模式，MIX混合模式	
	uint8_t 	PWMENFlag;//启动标志位	
	uint8_t 	KeyFlag1;//按键标志位
	uint8_t 	KeyFlag2;//按键标志位	
	uint8_t 	SyncSuccessFlag;//同步成功标志位
};

//状态机枚举量
typedef enum
{
    Init,//初始化
    Wait,//空闲等待
    Rise,//软启
    Run,//正常运行
    Err//故障
}STATE_M;

#define clock 400000

#define initFreq 1000
#define initSitaDeg 0
#define initDTCmp 12
#define initDelayCmp 30

#define initPeriod clock/initFreq

#define fullDeg 3600
#define baseCmp 300
 
#define MINFREQVALUE 800
#define MAXFREQVALUE 2000
#define MINSitaValue 0
#define MAXSitaValue 1750
#define MINPhiValue 50
#define MAXPhiValue 1850

#define MINDTValue 5
#define MAXDTValue 30

typedef enum
{
	FREQ=0,
	SITA =1,
	DT1=2,
	DT2=3,
	DelayMode=4,
	Dac_Out=5	
} angleTypeDef;

extern angleTypeDef angMode[6];
extern angleTypeDef currentMode;
extern int modeCounter;
extern unsigned int freqUpdateFlag,phaseUpdateFlag,deadtimeUpdateFlag,delayUpdateFlag;

extern int freq,sitaDeg,dt1,dt2,delayComp;

#define setRegBits(reg, mask)   (reg |= (unsigned int)(mask))
#define clrRegBits(reg, mask)  	(reg &= (unsigned int)(~(unsigned int)(mask)))
#define getRegBits(reg, mask)   (reg & (unsigned int)(mask))
#define getReg(reg)           	(reg)

//#define CCMRAM  __attribute__((section("ccmram")))

#endif
