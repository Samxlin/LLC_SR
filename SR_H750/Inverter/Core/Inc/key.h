#ifndef __KEY_H
#define __KEY_H	 

#include "main.h"

#define KEY_1_Pin GPIO_PIN_3
#define KEY_1_GPIO_Port GPIOE
#define KEY_2_Pin GPIO_PIN_4
#define KEY_2_GPIO_Port GPIOE
#define KEY_3_Pin GPIO_PIN_5
#define KEY_3_GPIO_Port GPIOE
#define KEY_4_Pin GPIO_PIN_6
#define KEY_4_GPIO_Port GPIOE

#define varRate 1

typedef enum
{
  KEY_UP   = 0,
  KEY_DOWN = 1,
}KEYState_TypeDef;

#define KEY_PUSH               0
#define KEY_PUSH_0020MS        20-1     //���尴���ж�ʱ��
#define KEY_PUSH_0400MS        400-1    //���尴���ж�ʱ��
#define KEY_PUSH_1000MS        1000-1   //���尴���ж�ʱ��


typedef int KEY_State;
typedef int KEY_Condition;

//״̬
#define KEY_STATES            2
#define KEY_STATES_RELEASE    0
#define KEY_STATES_PUSH       1



typedef void  (*KeyDeal)( void);

typedef struct
{
  KEY_State         State_last;       //��¼��ǰ��״̬
	uint32_t          usTime;        //��¼���µ�ʱ��
  uint32_t          usTS;          //��¼�ͷŵ�ʱ��
  uint32_t          usKeyDtcNum;	 //��¼������
	KeyDeal           NO_PRO_Deal;
	KeyDeal           SHOURT_Deal;
	KeyDeal           LONG_Deal;
	KeyDeal           DOUBLE_Deal;
	
}KEY_StateMachine,*KEY_pStateMachine;
 
typedef void  (*ActionFunType)( KEY_pStateMachine  pStateMachine);
typedef struct
{
	GPIO_TypeDef*   GPIOx;
	uint16_t        Pin;
}MYKEY_SET_TYPE;
typedef struct
{
	MYKEY_SET_TYPE    keyset;
	KEY_StateMachine  Machine;
}MYKEY_TYPE;
	
void KEY_Init(void);
void Botton_Process(void);

#endif
