#include "function.h"

ALIGN_32BYTES (uint16_t adc3_data[ADC3_BUFFER_SIZE]) __attribute__((section(".ARM.__at_0x24000000")));
struct  _ADI SADC={0,0,32768,0,0};//���������������ֵ��ƽ��ֵ

struct  _FLAG	DF={0,0,0,0,0,0,0,0};//���Ʊ�־λ

unsigned int fullperiod,halfperiod;
int freq,sitaDeg,dt1,dt2,delayComp;

angleTypeDef angMode[6]={FREQ, SITA, DT1, DT2, DelayMode, Dac_Out };
angleTypeDef currentMode=FREQ;
int modeCounter=0;
unsigned int freqUpdateFlag=0,phaseUpdateFlag=0,deadtimeUpdateFlag=0,delayUpdateFlag=0;

//OLEDˢ�¼��� 1mS����һ�Σ���1mS�ж����ۼ�
uint16_t OLEDShowCnt=0;

//DAC���ֵ
uint16_t DAC1_OUT1_Value=300;

/*
** ===================================================================
**     Funtion Name :   void ADCSample(void)
**     Description :    ���������ѹ����������������ѹ��������� ����Ϊ����Ƶ�� fH/2pi/a
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void ADCSample()
{
	static uint32_t VoutAvgSum=0,IoutAvgSum=0;
	
	//��DMA�������л�ȡ���� Q15,������������Խ�������������¼Ĵ�����Ӧ��ֵ�б仯
	SADC.Iout  = adc3_data[0] - SADC.IoutOffset;
	SADC.Vout  = adc3_data[1];

	if(SADC.Iout <0 )
		SADC.Iout = 0;
	if(SADC.Vout <100 )
		SADC.Vout = 0;
	
	//�����������ֵ��ƽ��ֵ-����ƽ����ʽ
	VoutAvgSum = VoutAvgSum + SADC.Vout -(VoutAvgSum>>2); //����1kHz/2pi/4=40Hz
	SADC.VoutAvg = VoutAvgSum>>2;
	IoutAvgSum = IoutAvgSum + SADC.Iout -(IoutAvgSum>>2);
	SADC.IoutAvg = IoutAvgSum>>2;	
}

/** ===================================================================
**     Funtion Name :void StateM(void)
**     Description :   ״̬������
**     ��ʼ��״̬
**     �ȴ�����״̬
**     ����״̬
**     ����״̬
**     ����״̬
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateM(void)
{
	//�ж�״̬����
	switch(DF.SMFlag)
	{
		//��ʼ��״̬
		case  Init :StateMInit();
		break;
		//�ȴ�״̬
		case  Wait :StateMWait();
		break;
		//������״̬
		case  Rise :StateMRise();
		break;
		//����״̬
		case  Run :StateMRun();
		break;
		//����״̬
		case  Err :StateMErr();
		break;
	}
}
/** ===================================================================
**     Funtion Name :void StateMInit(void)
**     Description :   ��ʼ��״̬������������ʼ��
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMInit(void)
{
	//��ز�����ʼ��
	ValInit();
	//״̬����ת���ȴ�����״̬
	DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void StateMWait(void)
**     Description :   �ȴ�״̬�����ȴ�1S���޹���������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMWait(void)
{
	//����������
	static uint16_t CntS = 0;
	static uint32_t	IoutSum=0;
	
	//��PWM
	DF.PWMENFlag=0;
	//�������ۼ�
	CntS ++;
	//�ȴ�*S������������������ƫ�úú� ���޹������,�а������£����������������״̬
	if(CntS>256)
	{
		CntS=256;
		HAL_HRTIM_WaveformOutputStart(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //����PWM�����PWM��ʱ��
		HAL_HRTIM_WaveformOutputStart(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //����PWM�����PWM��ʱ��
//		if((DF.ErrFlag==F_NOERR)&&(DF.KeyFlag1==1))
		if(DF.ErrFlag==F_NOERR)
		{
			//��������0
			CntS=0;
			IoutSum=0;
			//״̬��־λ��ת���ȴ�״̬
			DF.SMFlag  = Rise;
		}
	}
	else//����������������1.65Vƫ����ƽ��
	{
	  //�����������ƫ�����
		IoutSum += adc3_data[0];
    //256����
    if(CntS==256)
    {
        //��ƽ��
        SADC.IoutOffset = IoutSum >>8;
    }
	}
}
/*
** ===================================================================
**     Funtion Name : void StateMRise(void)
**     Description :�����׶�
**     ������ʼ��
**     �����ȴ�
**     ��ʼ����
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SSCNT       100//�ȴ�100ms
void StateMRise(void)
{
	if(DF.SyncSuccessFlag)
	{
		//״̬����ת������״̬
		DF.SMFlag  = Run;
	}
}
/*
** ===================================================================
**     Funtion Name :void StateMRun(void)
**     Description :�������У������������ж�������
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
void StateMRun(void)
{
	;
}

/*
** ===================================================================
**     Funtion Name :void StateMErr(void)
**     Description :����״̬
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
void StateMErr(void)
{
	//�ر�PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //�ر�
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�		
	//������������ת���ȴ���������
	if(DF.ErrFlag==F_NOERR)
			DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void ValInit(void)
**     Description :   ��ز�����ʼ������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void ValInit(void)
{
	//�ر�PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //�ر�
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�		
	//������ϱ�־λ
	DF.ErrFlag=0;
}

/*
** ===================================================================
**     Funtion Name :void ShortOff(void)
**     Description :��·��������������10��
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SHORT_I     1396//��·�����о�
#define MIN_SHORT_V     289//��·��ѹ�о�
void ShortOff(void)
{
	static int32_t RSCnt = 0;
	static uint8_t RSNum =0 ;

	//������������� *A���ҵ�ѹС��*Vʱ�����ж�Ϊ������·����
	if((SADC.Iout> MAX_SHORT_I)&&(SADC.Vout <MIN_SHORT_V))
	{
		//�ر�PWM
		DF.PWMENFlag=0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //�ر�
		HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�	
		//���ϱ�־λ
		setRegBits(DF.ErrFlag,F_SW_SHORT);
		//��ת������״̬
		DF.SMFlag  =Err;
	}
	//�����·�����ָ�
	//�����������·�������ػ���ȴ�4S�����������Ϣ������ȴ�״̬�ȴ�����
	if(getRegBits(DF.ErrFlag,F_SW_SHORT))
	{
		//�ȴ���������������ۼ�
		RSCnt++;
		//�ȴ�2S
		if(RSCnt >400)
		{
			//����������
			RSCnt=0;
			//��·����ֻ����10�Σ�10�κ�����
			if(RSNum > 10)
			{
				//ȷ����������ϣ�������
				RSNum =11;
				//�ر�PWM
				DF.PWMENFlag=0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //�ر�
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�	
			}
			else
			{
				//��·�����������ۼ�
				RSNum++;
				//��������������ϱ�־λ
				clrRegBits(DF.ErrFlag,F_SW_SHORT);
			}
		}
	}
}
/*
** ===================================================================
**     Funtion Name :void SwOCP(void)
**     Description :�������������������
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_OCP_VAL     1117//*A���������� 
void SwOCP(void)
{
	//���������оݱ��ּ���������
	static  uint16_t  OCPCnt=0;
	//����������ּ���������
	static  uint16_t  RSCnt=0;
	//������������������
	static  uint16_t  RSNum=0;

	//�������������*A���ұ���500ms
	if((SADC.Iout > MAX_OCP_VAL)&&(DF.SMFlag  ==Run))
	{
		//�������ּ�ʱ
		OCPCnt++;
		//��������50ms������Ϊ��������
		if(OCPCnt > 10)
		{
			//��������0
			OCPCnt  = 0;
			//�ر�PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //�ر�
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�	
			//���ϱ�־λ
			setRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			//��ת������״̬
			DF.SMFlag  =Err;
		}
	}
	else
		//��������0
		OCPCnt  = 0;

	//���������ָ�
	//�����������������������ػ���ȴ�4S�����������Ϣ������ȴ�״̬�ȴ�����
	if(getRegBits(DF.ErrFlag,F_SW_IOUT_OCP))
	{
		//�ȴ���������������ۼ�
		RSCnt++;
		//�ȴ�2S
		if(RSCnt > 400)
		{
			//����������
			RSCnt=0;
			//���������������ۼ�
			RSNum++;
			//��������ֻ����10�Σ�10�κ����������ع��ϣ�
			if(RSNum > 10 )
			{
				//ȷ����������ϣ�������
				RSNum =11;
				//�ر�PWM
				DF.PWMENFlag=0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //�ر�
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�		
			}
			else
			{
			 //��������������ϱ�־λ
				clrRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			}
		}
	}
}

/*
** ===================================================================
**     Funtion Name :void VinSwUVP(void)
**     Description :�������Ƿѹ��������ѹ���뱣��,�ɻָ�
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MIN_UVP_VAL    4000//3VǷѹ���� 
#define MIN_UVP_VAL_RE  4500//3.3VǷѹ�����ָ� 
void VinSwUVP(void)
{
	//��ѹ�����оݱ��ּ���������
	static  uint16_t  UVPCnt=0;
	static  uint16_t	RSCnt=0;

	//���������С����11.4V���ұ���200ms
	if ((SADC.Vout < MIN_UVP_VAL) && (DF.SMFlag != Init ))
	{
		//�������ּ�ʱ
		UVPCnt++;
		//��������10ms
		if(UVPCnt > 20)
		{
			//��ʱ������
			UVPCnt=0;
			RSCnt=0;
			//�ر�PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //�ر�
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�		
			//���ϱ�־λ
			setRegBits(DF.ErrFlag,F_SW_VIN_UVP);
			//��ת������״̬
			DF.SMFlag  =Err;
		}
	}
	else
		UVPCnt = 0;
	
	//����Ƿѹ�����ָ�
	//����������Ƿѹ�������ȴ������ѹ�ָ�������ˮƽ��������ϱ�־λ������
	if(getRegBits(DF.ErrFlag,F_SW_VIN_UVP))
	{
		if(SADC.Vout > MIN_UVP_VAL_RE) 
		{
			//�ȴ���������������ۼ�
			RSCnt++;
			//�ȴ�1S
			if(RSCnt > 200)
			{
				RSCnt=0;
				UVPCnt=0;
				//������ϱ�־λ
				clrRegBits(DF.ErrFlag,F_SW_VIN_UVP);
			}	
		}
		else	
			RSCnt=0;	
	}
	else
		RSCnt=0;
}


void Freq_Update()
{
	fullperiod=clock/freq;
	halfperiod=fullperiod/2;
	
	__HAL_HRTIM_SetPeriod(&hhrtim,HRTIM_TIMERINDEX_MASTER,fullperiod<<2);
	__HAL_HRTIM_SetPeriod(&hhrtim,HRTIM_TIMERINDEX_TIMER_A,fullperiod);
	__HAL_HRTIM_SetPeriod(&hhrtim,HRTIM_TIMERINDEX_TIMER_B,fullperiod);
	
	sitaDeg=0;
	phaseUpdateFlag=1;
	delayComp=initDelayCmp;
	delayUpdateFlag=1;
}

void Phase_Update()
{
	static int sitaCmp;
	sitaCmp=sitaDeg*fullperiod/3600;
	
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_1, baseCmp+sitaCmp);	
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2, baseCmp+halfperiod-sitaCmp);
}

void Deadtime_Update()
{
  /* Update the HRTIM registers */
  MODIFY_REG(hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].DTxR, HRTIM_DTR_DTR, dt1);
  MODIFY_REG(hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].DTxR, HRTIM_DTR_DTF, dt1<< 16U);
  MODIFY_REG(hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].DTxR, HRTIM_DTR_DTR, dt2);
  MODIFY_REG(hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].DTxR, HRTIM_DTR_DTF, dt2<< 16U);
	
}

void Delay_Update()
{
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_1, fullperiod-delayComp);	
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_2, halfperiod-delayComp); 
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_1, fullperiod-delayComp);	
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_2, halfperiod-delayComp); 
}

void Signal_Init()
{
	freq=initFreq;
	sitaDeg=initSitaDeg;
	dt1=initDTCmp;
	dt2=initDTCmp;
	delayComp=initDelayCmp;
	
	freqUpdateFlag=1;
	phaseUpdateFlag=1;
	deadtimeUpdateFlag=1;
	delayUpdateFlag=1;
}

void Output_Update()
{
	if(freqUpdateFlag)
	{
		freqUpdateFlag=0;
		Freq_Update();
			
		hhrtim.Instance->sCommonRegs.CR2 |= HRTIM_CR2_MSWU;
	}
	if(phaseUpdateFlag)
	{
		phaseUpdateFlag=0;
		Phase_Update();
	}
	if(deadtimeUpdateFlag)
	{
		deadtimeUpdateFlag=0;
		Deadtime_Update();
	}
	if(delayUpdateFlag)
	{
		delayUpdateFlag=0;
		Delay_Update();
	}
}

/** ===================================================================
**     Funtion Name :void LEDShow(void)
**     Description :  LED��ʾ����
**     ��ʼ��״̬��ȫ��
**     �ȴ�����״̬����1
**     ����״̬���� 2
**     ����״̬���� 12
**     ����״̬���� 3
**     Parameters  :
**     Returns     :
** ===================================================================*/
//���״̬�ƺ궨��
 #define SET_LED1()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_SET)//��1��
 #define SET_LED2()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1,GPIO_PIN_SET)//��2��
 #define SET_LED3()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2,GPIO_PIN_SET)//��3��
 #define CLR_LED1()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_RESET)//��1��
 #define CLR_LED2()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1,GPIO_PIN_RESET)//��2��
 #define CLR_LED3()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2,GPIO_PIN_RESET)//��3��
void LEDShow(void)
{
	switch(DF.SMFlag)
	{
		//��ʼ��״̬�������ȫ��
		case  Init :
		{
			SET_LED1();
			SET_LED2();
			SET_LED3();
			break;
		}
		//�ȴ�״̬����1
		case  Wait :
		{
			SET_LED1();
			CLR_LED2();
			CLR_LED3();
			break;
		}
		//������״̬����2
		case  Rise :
		{
			CLR_LED1();
			SET_LED2();
			CLR_LED3();
			break;
		}
		//����״̬����12
		case  Run :
		{
			SET_LED1();
			SET_LED2();
			CLR_LED3();
			break;
		}
		//����״̬�������
		case  Err :
		{
			CLR_LED1();
			CLR_LED2();
			SET_LED3();
			break;
		}
	}
}


/** ===================================================================
**     Funtion Name :void OLEDShow(void)
**     Description : OLED��ʾ����		 
**     ��ʾ����ģʽ-BUCK MODE,BOOST MODE,MIX MODE
**     ��ʾ״̬��IDEL,RISISING,RUNNING,ERROR
**     ��ʾ�����ѹ����λС��
**     ��ʾ�����������λС��
**     Parameters  :
**     Returns     :
** ===================================================================*/
void OLED_SHOW()
{
	unsigned int freqDis[4]={0,0,0,0};
	unsigned int sitaDis[4]={0,0,0,0};
	unsigned int dt1Dis[4]={0,0,0,0};
	unsigned int dt2Dis[4]={0,0,0,0};	
	unsigned int delayDis[4]={0,0,0,0};	
	unsigned int dac1Dis[4]={0,0,0,0};	
	
	freqDis[0] = (u8)(freq/1000);
	freqDis[1] = (u8)((freq-(uint8_t)freqDis[0]*1000)/100);
	freqDis[2] = (u8)((freq-(uint16_t)freqDis[0]*1000-(uint16_t)freqDis[1]*100)/10);
	freqDis[3] = (u8)(freq-(uint16_t)freqDis[0]*1000-(uint16_t)freqDis[1]*100-(uint16_t)freqDis[2]*10);	
	
	sitaDis[0] = (u8)(sitaDeg/1000);
	sitaDis[1] = (u8)((sitaDeg-(uint8_t)sitaDis[0]*1000)/100);
	sitaDis[2] = (u8)((sitaDeg-(uint16_t)sitaDis[0]*1000-(uint16_t)sitaDis[1]*100)/10);
	sitaDis[3] = (u8)(sitaDeg-(uint16_t)sitaDis[0]*1000-(uint16_t)sitaDis[1]*100-(uint16_t)sitaDis[2]*10);	
	
//	dt1Dis[0] = (u8)(SADC.Iout/1000);
//	dt1Dis[1] = (u8)((SADC.Iout-(uint8_t)dt1Dis[0]*1000)/100);
//	dt1Dis[2] = (u8)((SADC.Iout-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100)/10);
//	dt1Dis[3] = (u8)(SADC.Iout-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100-(uint16_t)dt1Dis[2]*10);	
//	
//	dt2Dis[0] = (u8)(SADC.Vout/1000);
//	dt2Dis[1] = (u8)((SADC.Vout-(uint8_t)dt2Dis[0]*1000)/100);
//	dt2Dis[2] = (u8)((SADC.Vout-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100)/10);
//	dt2Dis[3] = (u8)(SADC.Vout-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100-(uint16_t)dt2Dis[2]*10);	
	
	dt1Dis[0] = (u8)(dt1/1000);
	dt1Dis[1] = (u8)((dt1-(uint8_t)dt1Dis[0]*1000)/100);
	dt1Dis[2] = (u8)((dt1-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100)/10);
	dt1Dis[3] = (u8)(dt1-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100-(uint16_t)dt1Dis[2]*10);	
	
	dt2Dis[0] = (u8)(dt2/1000);
	dt2Dis[1] = (u8)((dt2-(uint8_t)dt2Dis[0]*1000)/100);
	dt2Dis[2] = (u8)((dt2-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100)/10);
	dt2Dis[3] = (u8)(dt2-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100-(uint16_t)dt2Dis[2]*10);	
	
	delayDis[0] = (u8)(delayComp/1000);
	delayDis[1] = (u8)((delayComp-(uint8_t)delayDis[0]*1000)/100);
	delayDis[2] = (u8)((delayComp-(uint16_t)delayDis[0]*1000-(uint16_t)delayDis[1]*100)/10);
	delayDis[3] = (u8)(delayComp-(uint16_t)delayDis[0]*1000-(uint16_t)delayDis[1]*100-(uint16_t)delayDis[2]*10);	
	
	dac1Dis[0] = (u8)(DAC1_OUT1_Value/1000);
	dac1Dis[1] = (u8)((DAC1_OUT1_Value-(uint8_t)dac1Dis[0]*1000)/100);
	dac1Dis[2] = (u8)((DAC1_OUT1_Value-(uint16_t)dac1Dis[0]*1000-(uint16_t)dac1Dis[1]*100)/10);
	dac1Dis[3] = (u8)(DAC1_OUT1_Value-(uint16_t)dac1Dis[0]*1000-(uint16_t)dac1Dis[1]*100-(uint16_t)dac1Dis[2]*10);	

	OLED_ShowStr(25,0,(uint8_t *)"MODE  :  SR",1);
	OLED_ShowStr(10,2,(uint8_t *)"Freq  :  ",1);
	OLED_ShowStr(100,2,(uint8_t *)"kHz",1);
	OLED_ShowStr(10,3,(uint8_t *)"sita : ",1);
	OLED_ShowStr(10,4,(uint8_t *)"dt1  : ",1);
	OLED_ShowStr(10,5,(uint8_t *)"dt2  : ",1);
	
	OLEDShowData(65,2,freqDis[0]);
	OLEDShowData(72,2,freqDis[1]);
	OLEDShowData(79,2,freqDis[2]);
	OLEDShowData(86,2,freqDis[3]);
	
	OLEDShowData(80,3,sitaDis[0]);
	OLEDShowData(87,3,sitaDis[1]);
	OLEDShowData(94,3,sitaDis[2]);
	OLEDShowData(101,3,sitaDis[3]);
	
//	OLEDShowData(10,6,dt1Dis[0]);
//	OLEDShowData(17,6,dt1Dis[1]);
//	OLEDShowData(24,6,dt1Dis[2]);
//	OLEDShowData(31,6,dt1Dis[3]);
//	
//	OLEDShowData(80,6,dt2Dis[0]);
//	OLEDShowData(87,6,dt2Dis[1]);
//	OLEDShowData(94,6,dt2Dis[2]);
//	OLEDShowData(101,6,dt2Dis[3]);
	
	OLEDShowData(80,4,dt1Dis[0]);
	OLEDShowData(87,4,dt1Dis[1]);
	OLEDShowData(94,4,dt1Dis[2]);
	OLEDShowData(101,4,dt1Dis[3]);
	
	OLEDShowData(80,5,dt2Dis[0]);
	OLEDShowData(87,5,dt2Dis[1]);
	OLEDShowData(94,5,dt2Dis[2]);
	OLEDShowData(101,5,dt2Dis[3]);
	
	OLEDShowData(10,6,delayDis[0]);
	OLEDShowData(17,6,delayDis[1]);
	OLEDShowData(24,6,delayDis[2]);
	OLEDShowData(31,6,delayDis[3]);
	
	OLEDShowData(80,6,dac1Dis[0]);
	OLEDShowData(87,6,dac1Dis[1]);
	OLEDShowData(94,6,dac1Dis[2]);
	OLEDShowData(101,6,dac1Dis[3]);
}