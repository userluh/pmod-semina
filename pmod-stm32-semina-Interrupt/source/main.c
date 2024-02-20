/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
int num=0;
int num_10=0;
int num_1=0;
int time_count=0;
int flag0=0;
int flag1=0;
int flag2=0;
int flag3=0;
int flag_sw2=0;
int delay_time=15;

//디바운싱을 위해 사용되는 변수들

int current_tick_0=0;
int current_tick_1=0;
int current_tick_2=0;
int current_tick_3=0;
int old_tick_0=0;
int old_tick_1=0;
int old_tick_2=0;
int old_tick_3=0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int FND[8]={0x0800,0x1000,0x4000,0x8000,0x0020,0x0040,0x0100,0x0200};//11,12,14,15,5,6,8,9번핀
GPIO_TypeDef* GPIO_FND[8]={GPIOB,GPIOB,GPIOB,GPIOB,GPIOC,GPIOC,GPIOC,GPIOC};//각 fnd의 포트 BBBBGGGG, ->연산자를 사용해야하기 때문에 포인터형식을 사용
int NPN[2]={0x0800,0x1000};//11,12핀

//FND의 숫자를 표시하기 위한 배열
int FND_Num_GPIOB[10]={0xD000,0x4000,0xC800,0xC800,0x5800,0x9800,0x9800,0xD000,0xD800,0xD800};//순서대로 0,1,2,3,4,5,6,7,8,9을 나타냄
int FND_Num_GPIOC[10]={0x0160,0x0100,0x0060,0x0140,0x0100,0x0140,0x0160,0x0100,0x0160,0x0100};//순서대로 0,1,2,3,4,5,6,7,8,9을 나타냄

int LED[8]={0x0002,0x0004,0x0008,0x0010,0x0040,0x0100,0x0200,0x0400};//1,2,3,4,6,8,9,10번핀
int SW[4]={0x0010,0x0020,0x0040,0x080};//4,5,6,7번핀

void FND_off(void){//모든 fnd를 끄는 함수
	for(int i=0; i<8; i++){
		GPIO_FND[i]->ODR|=FND[i];
	}
}

void LED_off(void){//모든 led를 끄는 함수
	for(int i=0; i<8; i++){
		GPIOB->ODR |= LED[i];
	}
}
void display_num(int num){//원하는 숫자 표시하는 함수
	FND_off();
	GPIOB->ODR&= ~FND_Num_GPIOB[num];
	GPIOC->ODR&= ~FND_Num_GPIOC[num];
}

//디바운싱 해주어야함
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==0x0010) current_tick_0=HAL_GetTick();//0
	if(GPIO_Pin==0x0020) current_tick_1=HAL_GetTick();//1
	if(GPIO_Pin==0x0040) current_tick_2=HAL_GetTick();//2
	if(GPIO_Pin==0x0080) current_tick_3=HAL_GetTick();//3

	if(GPIO_Pin==0x0010&&(current_tick_0-old_tick_0>15)){//0번버튼, 15ms동안 디바운싱을 해줌
		flag0=1;
		flag1=0;
		flag2=0;
		flag3=0;
		old_tick_0=current_tick_0;
	}
	if(GPIO_Pin==0x0020&&(current_tick_1-old_tick_1>15)){//1번버튼, 15ms동안 디바운싱을 해줌
		flag1=1;
		flag0=0;
		old_tick_1=current_tick_1;
		delay_time=15;
		}
	if(GPIO_Pin==0x0040&&(current_tick_2-old_tick_2>15)){//2번버튼, 15ms동안 디바운싱을 해줌
		if(flag_sw2){
		num+=5;
		}
		old_tick_2=current_tick_2;
	}
	if(GPIO_Pin==0x0080&&(current_tick_3-old_tick_3>15)){//3번버튼, 15ms동안 디바운싱을 해줌
		flag0=0;
		flag1=0;
		flag3=1;
		num=0;
		flag_sw2=1;
		old_tick_3=current_tick_3;
	}
}

void Dynamic_FND(void){//FND를 동적구동 시키는 함수, 아주빠른 주기로 트랜지스터를 토글시켜 동시에 켜진것 처럼 보이게함

	GPIOA->ODR|=NPN[0];//트랜지스터의 스위칭 작용으로 10의 자리 fnd만 킴
	GPIOA->ODR&=~NPN[1];//트랜지스터의 스위칭 작용으로 1의 자리 fnd 끔
	display_num(num_10);
	HAL_Delay(10);

	GPIOA->ODR|=NPN[1];//트랜지스터의 스위칭 작용으로 1의 자리 fnd만 킴
	GPIOA->ODR&=~NPN[0];//트랜지스터의 스위칭 작용으로 10의 자리 fnd끔
	display_num(num_1);
	HAL_Delay(10);
}

void SW1_Input(void){//SW의 입력을 받는함수
	if(!(GPIOG->IDR & SW[1])){
		HAL_Delay(delay_time);//디바운싱 방지 딜레이 15ms
		delay_time=0;//버튼을 "처음 눌렀을때만" 디바운싱 할수 있도록 해줌
		if(num_10>0){
			for(int i=0; i<num_10; i++){//FND의 10자리수 만큼 LED를 킴
				GPIOB->ODR &= ~LED[i];
			}
		}
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  //*입출력 설정*//
  GPIOA->MODER=0x01400000;//트랜지스터 output 모드로 설정
  GPIOB->MODER=0x51551154;//led,fnd output 모드로 설정
  GPIOC->MODER=0x00051400;//fnd output 모드로 설정

  //*인터럽트 레지스터설정 코드*//

  //외부인터럽트 EXTI 설정
  SYSCFG->EXTICR[2]=0x00005555; //사용하는 외부 EXIT는 PG 4~7이므로 EXTICR2, PG로 설정할때의 값은 0100

  //인터럽트 마스크 설정 코드
  EXTI->IMR=(1<<4)|(1<<5)|(1<<6)|(1<<7);//외부인터럽트 EXTI4~7을 NOT Mask(Enable) 시킴



  EXTI->RTSR=(1<<5)|(1<<6);//문제 조건에 따라 눌렀다 떼는 조건은 RISING Edge로 설정
  EXTI->FTSR=(1<<4)|(1<<7);//문제 조건에 따라 누르는 조건은 FALLING Edge로 설정
  NVIC->ISER[0]|=(1<<23)|(1<<10);//NVIC에 EXTI 4~7을 연결

  LED_off();//초기 led 끄기
  FND_off();//초기 fnd 끄기

  //FND의 초기 세팅값을 00으로 설정
  GPIOA->ODR|=NPN[0];
  GPIOA->ODR|=NPN[1];
  display_num(num);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(flag0){
		  if(time_count==10){//FND의 수가 올라가는 속도조절
		      num++;
		      time_count=0;
	      }
		  if(num==100) num=0;//99가 넘어가면 0으로 초기화
		  num_10=num/10;
		  num_1=num%10;
		  SW1_Input();
		  Dynamic_FND();//FND 동적구동
		  time_count++;
	  }


	  if(flag1){//뗀순간 LED,FND상태 그대로 정지
		  Dynamic_FND();
	  }

	  if(flag3){//초기화
		  num_10=num/10;
		  num_1=num%10;
		  LED_off();//모든 led꺼줌
		  Dynamic_FND();
	  }



  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, FND_C_Pin|FND_B_Pin|FND_A_Pin|FND_F_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LED1_Pin|LED2_Pin|LED8_Pin
                          |FND_D_Pin|FND_E_Pin|FND_G_Pin|FND_DP_Pin
                          |LED3_Pin|LED4_Pin|LED5_Pin|LD2_Pin
                          |LED6_Pin|LED7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, npn1_Pin|npn2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RMII_MDC_Pin */
  GPIO_InitStruct.Pin = RMII_MDC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(RMII_MDC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_REF_CLK_Pin RMII_MDIO_Pin RMII_CRS_DV_Pin */
  GPIO_InitStruct.Pin = RMII_REF_CLK_Pin|RMII_MDIO_Pin|RMII_CRS_DV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : FND_C_Pin FND_B_Pin FND_A_Pin FND_F_Pin */
  GPIO_InitStruct.Pin = FND_C_Pin|FND_B_Pin|FND_A_Pin|FND_F_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LED1_Pin LED2_Pin LED8_Pin
                           FND_D_Pin FND_E_Pin FND_G_Pin FND_DP_Pin
                           LED3_Pin LED4_Pin LED5_Pin LD2_Pin
                           LED6_Pin LED7_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LED1_Pin|LED2_Pin|LED8_Pin
                          |FND_D_Pin|FND_E_Pin|FND_G_Pin|FND_DP_Pin
                          |LED3_Pin|LED4_Pin|LED5_Pin|LD2_Pin
                          |LED6_Pin|LED7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RMII_TXD1_Pin */
  GPIO_InitStruct.Pin = RMII_TXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INTERRUPT1_Pin INTERRUPT4_Pin */
  GPIO_InitStruct.Pin = INTERRUPT1_Pin|INTERRUPT4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : INTERRUPT2_Pin INTERRUPT3_Pin */
  GPIO_InitStruct.Pin = INTERRUPT2_Pin|INTERRUPT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : npn1_Pin npn2_Pin */
  GPIO_InitStruct.Pin = npn1_Pin|npn2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_TX_EN_Pin RMII_TXD0_Pin */
  GPIO_InitStruct.Pin = RMII_TX_EN_Pin|RMII_TXD0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
