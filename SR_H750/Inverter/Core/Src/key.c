#include "key.h"
#include "function.h"

MYKEY_TYPE  MYKEY1 = {
	{KEY_1_GPIO_Port,KEY_1_Pin},
	{0,0,0,0,0,0,0,0, },
};
 
MYKEY_TYPE  MYKEY2 = {
	{KEY_2_GPIO_Port,KEY_2_Pin},
	{0,0,0,0,0,0,0,0, },
};
	
MYKEY_TYPE  MYKEY3 = {
 	{KEY_3_GPIO_Port,KEY_3_Pin},
	{0,0,0,0,0,0,0,0, },
};
 
MYKEY_TYPE  MYKEY4 = {
 	{KEY_4_GPIO_Port,KEY_4_Pin},
	{0,0,0,0,0,0,0,0, },
};

void  act0 ( KEY_StateMachine *   pStateMachine);
void  act1 ( KEY_StateMachine *   pStateMachine);
void  act2 ( KEY_StateMachine *   pStateMachine);
void  act3 ( KEY_StateMachine *   pStateMachine);

ActionFunType transition_table[KEY_STATES][KEY_STATES]={
{&act3,&act0,},
{&act2,&act1,},
};

void KEY_GPIO_Init( MYKEY_TYPE* mykey )
{
  mykey->Machine.State_last=0;
  mykey->Machine.usKeyDtcNum=0;
  mykey->Machine.usTime=0;
  mykey->Machine.usTS=0;
}
int addCount=0;

//�ͷ�״̬������
void  act0 ( KEY_StateMachine *  pStateMachine)
{
   pStateMachine->usTime=0;
}
//����״̬
void  act1 ( KEY_StateMachine *   pStateMachine)
{
   pStateMachine->usTime++;
	 if( pStateMachine->usTime >=  KEY_PUSH_1000MS  )
	 {
		pStateMachine->usKeyDtcNum=0;
		 addCount++;
	  (*pStateMachine->LONG_Deal)(); //����������
	 }		 
}
//����״̬���ͷ�
void  act2 ( KEY_StateMachine *   pStateMachine)
{
   pStateMachine->usTS=0;
	 if ( ( pStateMachine->usTime >KEY_PUSH_0020MS) && ( pStateMachine->usTime <KEY_PUSH_0400MS) )
	 {
	   pStateMachine->usKeyDtcNum++;
	 }
}
//�ͷ�״̬
void  act3 ( KEY_StateMachine *   pStateMachine)
{
	 if( pStateMachine->usTS < KEY_PUSH_0400MS )
	 {
	     pStateMachine->usTS++;
	 }else
	 {
	   if(pStateMachine->usKeyDtcNum == 1)
		 {
		   (*pStateMachine->SHOURT_Deal)(); //�̰�������
		 } 
		 if(pStateMachine->usKeyDtcNum == 2)
		 {
		    (*pStateMachine->DOUBLE_Deal)(); //˫����������
		 }
		 if(pStateMachine->usKeyDtcNum > 2)
		 {
			 for(int i=0;i<pStateMachine->usKeyDtcNum;i++){
		    (*pStateMachine->SHOURT_Deal)(); //��ε�����������
			 }
		 }
     pStateMachine->usKeyDtcNum=0;
		 pStateMachine->usTS=0;		 		 
	 }
}



void KEY_GPIO_DEAL(MYKEY_TYPE* mykey)
{
	 KEY_State  State_now= KEY_STATES_RELEASE ;
	 if(HAL_GPIO_ReadPin(mykey->keyset.GPIOx,mykey->keyset.Pin)==KEY_PUSH)
		 State_now= KEY_STATES_PUSH;     //��ȡ��ǰ����״̬
	
   (*transition_table[mykey->Machine.State_last][State_now] )( (KEY_StateMachine *) (&(mykey->Machine))  );  //����״̬ *FUNC��last����now�� ִ�ж�Ӧ�Ĵ�����
	
	  mykey->Machine.State_last=State_now;  //���浱ǰ�İ���״̬
}

//����1�ص�����
void callback10()
{
	;
}


void callback11()
{
	if(modeCounter!=5)
		modeCounter++;
	currentMode=angMode[modeCounter];
}
void callback12()
{
	;
}
void callback13()
{
	;	
}


//����2�ص�����
void callback20()
{
	;
}
void callback21()
{
	if(modeCounter!=0)
		modeCounter--;
	currentMode=angMode[modeCounter];
}
void callback22()
{
	;
}
void callback23()
{
	;
}



//����3�ص�����
void callback30()
{
	;
}
void callback31()
{
	switch(modeCounter)
	{
		case 0:
		{
			freq+=1;
			if(freq>=MAXFREQVALUE)
				freq=MAXFREQVALUE;
			freqUpdateFlag=1;
		} break;
		case 1:
		{
			sitaDeg+=1;
			if(sitaDeg>MAXSitaValue)
				sitaDeg=MAXSitaValue;
			phaseUpdateFlag=1;
		} break;
		case 2:
		{
			dt1+=1;
			if(dt1>MAXDTValue)
				dt1=MAXDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 3:
		{
			dt2+=1;
			if(dt2>MAXDTValue)
				dt2=MAXDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 4:
		{
			delayComp++;
			delayUpdateFlag=1;
		} break;
		case 5:
		{
			DAC1_OUT1_Value++;
			if(DAC1_OUT1_Value > MAX_DAC_OUT)	DAC1_OUT1_Value=MAX_DAC_OUT;
		} break;
	}
}
void callback32()
{
	;
}
void callback33()
{
	switch(modeCounter)
	{
		case 0:
		{
			freq+=1;
			if(freq>=MAXFREQVALUE)
				freq=MAXFREQVALUE;
			freqUpdateFlag=1;
		} break;
		case 1:
		{
			sitaDeg+=addCount*varRate;
			if(sitaDeg>MAXSitaValue)
				sitaDeg=MAXSitaValue;
			phaseUpdateFlag=1;
		} break;
		case 2:
		{
			dt1+=addCount*varRate;
			if(dt1>MAXDTValue)
				dt1=MAXDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 3:
		{
			dt2+=addCount*varRate;
			if(dt2>MAXDTValue)
				dt2=MAXDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 4:
		{
			delayComp++;
			delayUpdateFlag=1;
		} break;
		case 5:
		{
			DAC1_OUT1_Value++;
			if(DAC1_OUT1_Value > MAX_DAC_OUT)	DAC1_OUT1_Value=MAX_DAC_OUT;
		} break;

	}
	addCount=0;
}


//����4�ص�����
void callback40()
{
	;
}
void callback41()
{
	switch(modeCounter)
	{
		case 0:
		{
			freq-=1;
			if(freq<=MINFREQVALUE)
				freq=MINFREQVALUE;
			freqUpdateFlag=1;
		} break;
		case 1:
		{
			sitaDeg-=1;
			if(sitaDeg<=MINSitaValue)
				sitaDeg=MINSitaValue;
			phaseUpdateFlag=1;
		} break;
		case 2:
		{
			dt1-=1;
			if(dt1<=MINDTValue)
				dt1=MINDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 3:
		{
			dt2-=1;
			if(dt2<=MINDTValue)
				dt2=MINDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 4:
		{
			delayComp--;
			delayUpdateFlag=1;
		} break;
		case 5:
		{
			DAC1_OUT1_Value--;
			if(DAC1_OUT1_Value > MIN_DAC_OUT)	DAC1_OUT1_Value=MIN_DAC_OUT;
		} break;
	}	
}
void callback42()
{
	;
}
void callback43()
{
	switch(modeCounter)
	{
		case 0:
		{
			freq-=1;
			if(freq<=MINFREQVALUE)
				freq=MINFREQVALUE;			
			freqUpdateFlag=1;
		} break;
		case 1:
		{
			sitaDeg-=addCount*varRate;
			if(sitaDeg<=MINSitaValue)
				sitaDeg=MINSitaValue;
			phaseUpdateFlag=1;
		} break;
		case 2:
		{
			dt1-=addCount*varRate;
			if(dt1<=MINDTValue)
				dt1=MINDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 3:
		{
			dt2-=addCount*varRate;
			if(dt2<=MINDTValue)
				dt2=MINDTValue;
			deadtimeUpdateFlag=1;
		} break;
		case 4:
		{
			delayComp--;
			delayUpdateFlag=1;
		} break;
		case 5:
		{
			;
		} break;
	}
	addCount=0;
}

void KEY_Init()
{
	MYKEY1.Machine.NO_PRO_Deal=callback10;
	MYKEY1.Machine.SHOURT_Deal=callback11;
	MYKEY1.Machine.DOUBLE_Deal=callback12;
	MYKEY1.Machine.LONG_Deal  =callback13;
	KEY_GPIO_Init(&MYKEY1);	
	MYKEY2.Machine.NO_PRO_Deal=callback20;
	MYKEY2.Machine.SHOURT_Deal=callback21;
	MYKEY2.Machine.DOUBLE_Deal=callback22;
	MYKEY2.Machine.LONG_Deal  =callback23;
	KEY_GPIO_Init(&MYKEY2);	
	MYKEY3.Machine.NO_PRO_Deal=callback30;
	MYKEY3.Machine.SHOURT_Deal=callback31;
	MYKEY3.Machine.DOUBLE_Deal=callback32;
	MYKEY3.Machine.LONG_Deal  =callback33;
	KEY_GPIO_Init(&MYKEY3);	
	MYKEY4.Machine.NO_PRO_Deal=callback40;
	MYKEY4.Machine.SHOURT_Deal=callback41;
	MYKEY4.Machine.DOUBLE_Deal=callback42;
	MYKEY4.Machine.LONG_Deal  =callback43;
	KEY_GPIO_Init(&MYKEY4);	
}

void Botton_Process()
{	
	KEY_GPIO_DEAL(&MYKEY1);
	KEY_GPIO_DEAL(&MYKEY2);
	KEY_GPIO_DEAL(&MYKEY3);
	KEY_GPIO_DEAL(&MYKEY4);
}

