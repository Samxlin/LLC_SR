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

//��������
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

/*****************************��������*****************/
#define     F_NOERR      			0x0000//�޹���
#define     F_SW_SHORT    		0x0001//�����·
#define     F_SW_VIN_UVP  		0x0002//����Ƿѹ
#define     F_SW_IOUT_OCP    	0x0004//�������

//���������ṹ��
struct _ADI
{
	int32_t   Iout;//�������
	int32_t   IoutAvg;//�������ƽ��ֵ
	int32_t		IoutOffset;//�����������ƫ��
	int32_t   Vout;//������ѹ
	int32_t   VoutAvg;//������ѹƽ��ֵ
};

#define CAL_VOUT_K	4101//Q12�����ѹ����Kֵ
#define CAL_VOUT_B	49//Q12�����ѹ����Bֵ
#define CAL_IOUT_K	4096//Q12�����������Kֵ
#define CAL_IOUT_B	0//Q12�����������Bֵ

//��־λ����
struct  _FLAG
{
	uint16_t	SMFlag;//״̬����־λ
	uint16_t	CtrFlag;//���Ʊ�־λ
	uint16_t  ErrFlag;//���ϱ�־λ
	uint8_t		BBFlag;//����ģʽ��־λ��BUCKģʽ��BOOSTģʽ��MIX���ģʽ	
	uint8_t 	PWMENFlag;//������־λ	
	uint8_t 	KeyFlag1;//������־λ
	uint8_t 	KeyFlag2;//������־λ	
	uint8_t 	SyncSuccessFlag;//ͬ���ɹ���־λ
};

//״̬��ö����
typedef enum
{
    Init,//��ʼ��
    Wait,//���еȴ�
    Rise,//����
    Run,//��������
    Err//����
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
