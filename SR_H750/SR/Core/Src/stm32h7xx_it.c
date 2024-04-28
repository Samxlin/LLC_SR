/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "function.h"
#include "key.h"
#include "dac.h"
#include "hrtim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc3;
extern HRTIM_HandleTypeDef hhrtim;
extern TIM_HandleTypeDef htim2;
/* USER CODE BEGIN EV */

int captured_valueA=0;
int captured_valueB=0;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

  /* USER CODE END DMA1_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc3);
  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
	
	//输入输出采样
	ADCSample();
	//输入欠压保护
	VinSwUVP();
	//状态机函数
	StateM();
	//状态灯显示函数
	LEDShow();
	//输出更新函数
	Output_Update();
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles HRTIM master timer global interrupt.
  */
void HRTIM1_Master_IRQHandler(void)
{
  /* USER CODE BEGIN HRTIM1_Master_IRQn 0 */

  /* USER CODE END HRTIM1_Master_IRQn 0 */
  //HAL_HRTIM_IRQHandler(&hhrtim,HRTIM_TIMERINDEX_MASTER);
  /* USER CODE BEGIN HRTIM1_Master_IRQn 1 */
	__HAL_HRTIM_MASTER_CLEAR_IT(&hhrtim, HRTIM_MASTER_IT_MREP);
	
	static int a=20;
	int cA=0,cB=0,b=0,cnt=0;
	
	cA=dt1-captured_valueA;
	if(captured_valueA==1){
			cA=0;
	}
	
	cB=dt1-captured_valueA;;
	if(captured_valueB==1){
			cB=0;
	}

	b=abs(cA-cB);

	if((cA==0)||(cB==0))
	{
			a=a+cA-cB;
	}
	else if((b>2)&&(b<=(dt1+1)))
	{
		{
			a=a+cA-cB/abs(cA-cB);
		}
	}

	a=a%400;
	
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_1, a);	
	__HAL_HRTIM_SetCompare(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2, a+200);
			
	if(b<2)
	{
		cnt++;
		if(cnt>200)
		{		
			cnt=0;
			DF.SyncSuccessFlag=1;
		}
	}
  /* USER CODE END HRTIM1_Master_IRQn 1 */
}

/**
  * @brief This function handles HRTIM timer A global interrupt.
  */
void HRTIM1_TIMA_IRQHandler(void)
{
  /* USER CODE BEGIN HRTIM1_TIMA_IRQn 0 */
	uint32_t tisrflagsA = READ_REG(hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].TIMxISR);
  /* USER CODE END HRTIM1_TIMA_IRQn 0 */
  //HAL_HRTIM_IRQHandler(&hhrtim,HRTIM_TIMERINDEX_TIMER_A);
  /* USER CODE BEGIN HRTIM1_TIMA_IRQn 1 */

	/* Timer capture 1 event */
  if((uint32_t)(tisrflagsA & HRTIM_TIM_FLAG_CPT1) != (uint32_t)RESET)
  {
		__HAL_HRTIM_TIMER_CLEAR_IT(&hhrtim, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_IT_CPT1);
		captured_valueA = hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CPT1xR;

  }
	
  /* USER CODE END HRTIM1_TIMA_IRQn 1 */
}

/**
  * @brief This function handles HRTIM timer B global interrupt.
  */
void HRTIM1_TIMB_IRQHandler(void)
{
  /* USER CODE BEGIN HRTIM1_TIMB_IRQn 0 */
	uint32_t tisrflagsB = READ_REG(hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].TIMxISR);
  /* USER CODE END HRTIM1_TIMB_IRQn 0 */
  //HAL_HRTIM_IRQHandler(&hhrtim,HRTIM_TIMERINDEX_TIMER_B);
  /* USER CODE BEGIN HRTIM1_TIMB_IRQn 1 */
	/* Timer capture 1 event */
  if((uint32_t)(tisrflagsB & HRTIM_TIM_FLAG_CPT1) != (uint32_t)RESET)
  {
		__HAL_HRTIM_TIMER_CLEAR_IT(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_TIM_IT_CPT1);
		captured_valueB = hhrtim.Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CPT1xR;
		
  }
  /* USER CODE END HRTIM1_TIMB_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
