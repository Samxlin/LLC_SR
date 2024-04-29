#include "function.h"

ALIGN_32BYTES (uint16_t adc3_data[ADC3_BUFFER_SIZE]) __attribute__((section(".ARM.__at_0x24000000")));
struct  _ADI SADC={0,0,32768,0,0};//输入输出参数采样值和平均值

struct  _FLAG	DF={0,0,0,0,0,0,0,0};//控制标志位

unsigned int fullperiod,halfperiod;
int freq,sitaDeg,dt1,dt2,delayComp;

angleTypeDef angMode[6]={FREQ, SITA, DT1, DT2, DelayMode, Dac_Out };
angleTypeDef currentMode=FREQ;
int modeCounter=0;
unsigned int freqUpdateFlag=0,phaseUpdateFlag=0,deadtimeUpdateFlag=0,delayUpdateFlag=0;

//OLED刷新计数 1mS计数一次，在1mS中断里累加
uint16_t OLEDShowCnt=0;

//DAC输出值
uint16_t DAC1_OUT1_Value=300;

/*
** ===================================================================
**     Funtion Name :   void ADCSample(void)
**     Description :    采样输出电压、输出电流、输入电压、输入电流 带宽为采样频率 fH/2pi/a
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void ADCSample()
{
	static uint32_t VoutAvgSum=0,IoutAvgSum=0;
	
	//从DMA缓冲器中获取数据 Q15,并对其进行线性矫正，反相情况下寄存器对应的值有变化
	SADC.Iout  = adc3_data[0] - SADC.IoutOffset;
	SADC.Vout  = adc3_data[1];

	if(SADC.Iout <0 )
		SADC.Iout = 0;
	if(SADC.Vout <100 )
		SADC.Vout = 0;
	
	//计算各个采样值的平均值-滑动平均方式
	VoutAvgSum = VoutAvgSum + SADC.Vout -(VoutAvgSum>>2); //带宽1kHz/2pi/4=40Hz
	SADC.VoutAvg = VoutAvgSum>>2;
	IoutAvgSum = IoutAvgSum + SADC.Iout -(IoutAvgSum>>2);
	SADC.IoutAvg = IoutAvgSum>>2;	
}

/** ===================================================================
**     Funtion Name :void StateM(void)
**     Description :   状态机函数
**     初始化状态
**     等待启动状态
**     启动状态
**     运行状态
**     故障状态
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateM(void)
{
	//判断状态类型
	switch(DF.SMFlag)
	{
		//初始化状态
		case  Init :StateMInit();
		break;
		//等待状态
		case  Wait :StateMWait();
		break;
		//软启动状态
		case  Rise :StateMRise();
		break;
		//运行状态
		case  Run :StateMRun();
		break;
		//故障状态
		case  Err :StateMErr();
		break;
	}
}
/** ===================================================================
**     Funtion Name :void StateMInit(void)
**     Description :   初始化状态函数，参数初始化
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMInit(void)
{
	//相关参数初始化
	ValInit();
	//状态机跳转至等待软启状态
	DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void StateMWait(void)
**     Description :   等待状态机，等待1S后无故障则软启
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMWait(void)
{
	//计数器定义
	static uint16_t CntS = 0;
	static uint32_t	IoutSum=0;
	
	//关PWM
	DF.PWMENFlag=0;
	//计数器累加
	CntS ++;
	//等待*S，采样输入和输出电流偏置好后， 且无故障情况,切按键按下，启动，则进入启动状态
	if(CntS>256)
	{
		CntS=256;
		HAL_HRTIM_WaveformOutputStart(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //开启PWM输出和PWM计时器
		HAL_HRTIM_WaveformOutputStart(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //开启PWM输出和PWM计时器
//		if((DF.ErrFlag==F_NOERR)&&(DF.KeyFlag1==1))
		if(DF.ErrFlag==F_NOERR)
		{
			//计数器清0
			CntS=0;
			IoutSum=0;
			//状态标志位跳转至等待状态
			DF.SMFlag  = Rise;
		}
	}
	else//进行输入和输出电流1.65V偏置求平均
	{
	  //输入输出电流偏置求和
		IoutSum += adc3_data[0];
    //256次数
    if(CntS==256)
    {
        //求平均
        SADC.IoutOffset = IoutSum >>8;
    }
	}
}
/*
** ===================================================================
**     Funtion Name : void StateMRise(void)
**     Description :软启阶段
**     软启初始化
**     软启等待
**     开始软启
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SSCNT       100//等待100ms
void StateMRise(void)
{
	if(DF.SyncSuccessFlag)
	{
		//状态机跳转至运行状态
		DF.SMFlag  = Run;
	}
}
/*
** ===================================================================
**     Funtion Name :void StateMRun(void)
**     Description :正常运行，主处理函数在中断中运行
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
**     Description :故障状态
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
void StateMErr(void)
{
	//关闭PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //关闭
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭		
	//若故障消除跳转至等待重新软启
	if(DF.ErrFlag==F_NOERR)
			DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void ValInit(void)
**     Description :   相关参数初始化函数
**     Parameters  :
**     Returns     :
** ===================================================================*/
void ValInit(void)
{
	//关闭PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //关闭
	HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭		
	//清除故障标志位
	DF.ErrFlag=0;
}

/*
** ===================================================================
**     Funtion Name :void ShortOff(void)
**     Description :短路保护，可以重启10次
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SHORT_I     1396//短路电流判据
#define MIN_SHORT_V     289//短路电压判据
void ShortOff(void)
{
	static int32_t RSCnt = 0;
	static uint8_t RSNum =0 ;

	//当输出电流大于 *A，且电压小于*V时，可判定为发生短路保护
	if((SADC.Iout> MAX_SHORT_I)&&(SADC.Vout <MIN_SHORT_V))
	{
		//关闭PWM
		DF.PWMENFlag=0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //关闭
		HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭	
		//故障标志位
		setRegBits(DF.ErrFlag,F_SW_SHORT);
		//跳转至故障状态
		DF.SMFlag  =Err;
	}
	//输出短路保护恢复
	//当发生输出短路保护，关机后等待4S后清楚故障信息，进入等待状态等待重启
	if(getRegBits(DF.ErrFlag,F_SW_SHORT))
	{
		//等待故障清楚计数器累加
		RSCnt++;
		//等待2S
		if(RSCnt >400)
		{
			//计数器清零
			RSCnt=0;
			//短路重启只重启10次，10次后不重启
			if(RSNum > 10)
			{
				//确保不清除故障，不重启
				RSNum =11;
				//关闭PWM
				DF.PWMENFlag=0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //关闭
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭	
			}
			else
			{
				//短路重启计数器累加
				RSNum++;
				//清除过流保护故障标志位
				clrRegBits(DF.ErrFlag,F_SW_SHORT);
			}
		}
	}
}
/*
** ===================================================================
**     Funtion Name :void SwOCP(void)
**     Description :软件过流保护，可重启
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_OCP_VAL     1117//*A过流保护点 
void SwOCP(void)
{
	//过流保护判据保持计数器定义
	static  uint16_t  OCPCnt=0;
	//故障清楚保持计数器定义
	static  uint16_t  RSCnt=0;
	//保留保护重启计数器
	static  uint16_t  RSNum=0;

	//当输出电流大于*A，且保持500ms
	if((SADC.Iout > MAX_OCP_VAL)&&(DF.SMFlag  ==Run))
	{
		//条件保持计时
		OCPCnt++;
		//条件保持50ms，则认为过流发生
		if(OCPCnt > 10)
		{
			//计数器清0
			OCPCnt  = 0;
			//关闭PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //关闭
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭	
			//故障标志位
			setRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			//跳转至故障状态
			DF.SMFlag  =Err;
		}
	}
	else
		//计数器清0
		OCPCnt  = 0;

	//输出过流后恢复
	//当发生输出软件过流保护，关机后等待4S后清楚故障信息，进入等待状态等待重启
	if(getRegBits(DF.ErrFlag,F_SW_IOUT_OCP))
	{
		//等待故障清楚计数器累加
		RSCnt++;
		//等待2S
		if(RSCnt > 400)
		{
			//计数器清零
			RSCnt=0;
			//过流重启计数器累加
			RSNum++;
			//过流重启只重启10次，10次后不重启（严重故障）
			if(RSNum > 10 )
			{
				//确保不清除故障，不重启
				RSNum =11;
				//关闭PWM
				DF.PWMENFlag=0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //关闭
				HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭		
			}
			else
			{
			 //清除过流保护故障标志位
				clrRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			}
		}
	}
}

/*
** ===================================================================
**     Funtion Name :void VinSwUVP(void)
**     Description :输入软件欠压保护，低压输入保护,可恢复
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MIN_UVP_VAL    4000//3V欠压保护 
#define MIN_UVP_VAL_RE  4500//3.3V欠压保护恢复 
void VinSwUVP(void)
{
	//过压保护判据保持计数器定义
	static  uint16_t  UVPCnt=0;
	static  uint16_t	RSCnt=0;

	//当输出电流小于于11.4V，且保持200ms
	if ((SADC.Vout < MIN_UVP_VAL) && (DF.SMFlag != Init ))
	{
		//条件保持计时
		UVPCnt++;
		//条件保持10ms
		if(UVPCnt > 20)
		{
			//计时器清零
			UVPCnt=0;
			RSCnt=0;
			//关闭PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //关闭
			HAL_HRTIM_WaveformOutputStop(&hhrtim, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭		
			//故障标志位
			setRegBits(DF.ErrFlag,F_SW_VIN_UVP);
			//跳转至故障状态
			DF.SMFlag  =Err;
		}
	}
	else
		UVPCnt = 0;
	
	//输入欠压保护恢复
	//当发生输入欠压保护，等待输入电压恢复至正常水平后清楚故障标志位，重启
	if(getRegBits(DF.ErrFlag,F_SW_VIN_UVP))
	{
		if(SADC.Vout > MIN_UVP_VAL_RE) 
		{
			//等待故障清楚计数器累加
			RSCnt++;
			//等待1S
			if(RSCnt > 200)
			{
				RSCnt=0;
				UVPCnt=0;
				//清楚故障标志位
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
**     Description :  LED显示函数
**     初始化状态，全亮
**     等待启动状态，亮1
**     启动状态，亮 2
**     运行状态，亮 12
**     故障状态，亮 3
**     Parameters  :
**     Returns     :
** ===================================================================*/
//输出状态灯宏定义
 #define SET_LED1()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_SET)//灯1亮
 #define SET_LED2()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1,GPIO_PIN_SET)//灯2亮
 #define SET_LED3()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2,GPIO_PIN_SET)//灯3亮
 #define CLR_LED1()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_RESET)//灯1灭
 #define CLR_LED2()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1,GPIO_PIN_RESET)//灯2灭
 #define CLR_LED3()	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2,GPIO_PIN_RESET)//灯3灭
void LEDShow(void)
{
	switch(DF.SMFlag)
	{
		//初始化状态，红黄绿全亮
		case  Init :
		{
			SET_LED1();
			SET_LED2();
			SET_LED3();
			break;
		}
		//等待状态，亮1
		case  Wait :
		{
			SET_LED1();
			CLR_LED2();
			CLR_LED3();
			break;
		}
		//软启动状态，亮2
		case  Rise :
		{
			CLR_LED1();
			SET_LED2();
			CLR_LED3();
			break;
		}
		//运行状态，亮12
		case  Run :
		{
			SET_LED1();
			SET_LED2();
			CLR_LED3();
			break;
		}
		//故障状态，红灯亮
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
**     Description : OLED显示函数		 
**     显示运行模式-BUCK MODE,BOOST MODE,MIX MODE
**     显示状态：IDEL,RISISING,RUNNING,ERROR
**     显示输出电压：两位小数
**     显示输出电流：两位小数
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