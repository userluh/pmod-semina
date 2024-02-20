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
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
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

UART_HandleTypeDef huart7;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

int num=0;//fnd에 나타낼 숫자
int num_10=0;//10의자리
int num_1=0;//1의자리
int flag0=0;//0번 플래그
int flag1=0;//1번 플래그
int flag2=0;//2번 플래그
int flag2_2=0;//2-2번 플래그
int flag3=0;//3번 플래그
int delay_time=15;//디바운스 방지 딜레이

//디바운싱을 위해 사용되는 변수들
int current_tick_2=0;
int current_tick_3=0;
int old_tick_2=0;
int old_tick_3=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_UART7_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

extern UART_HandleTypeDef huart7;//HAL 라이브러리를 사용하기 위함

//extern ADC_HandleTypeDef hadc1;

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

	if(GPIO_Pin==0x0040) current_tick_2=HAL_GetTick();//2
	if(GPIO_Pin==0x0080) current_tick_3=HAL_GetTick();//3


	if(GPIO_Pin==0x0040 && (current_tick_2-old_tick_2>15)){//2번버튼, 15ms동안 디바운싱을 해줌
		flag0=0;
		flag1=0;
		flag2_2=1;
		flag3=0;
		old_tick_2=current_tick_2;

	}
	if(GPIO_Pin==0x0080 && (current_tick_3-old_tick_3>15) ){//3번버튼, 15ms동안 디바운싱을 해줌
		flag0=0;
		flag1=0;
		flag2=0;
		flag3=1;

		//초기화 소스코드
		LED_off();//led 끄기
		FND_off();//fnd 끄기
		num=0;//숫자 초기화

		old_tick_3=current_tick_3;
	}
}

void Dynamic_FND(void){//FND를 동적구동 시키는 함수, 아주빠른 주기로 트랜지스터를 토글시켜 동시에 켜진것 처럼 보이게함
	num_10=num/10;
	num_1=num%10;

	GPIOA->ODR|=NPN[0];//트랜지스터의 스위칭 작용으로 10의 자리 fnd만 킴
	GPIOA->ODR&=~NPN[1];//트랜지스터의 스위칭 작용으로 1의 자리 fnd 끔
	display_num(num_10);
	HAL_Delay(10);

	GPIOA->ODR|=NPN[1];//트랜지스터의 스위칭 작용으로 1의 자리 fnd만 킴
	GPIOA->ODR&=~NPN[0];//트랜지스터의 스위칭 작용으로 10의 자리 fnd끔
	display_num(num_1);
	HAL_Delay(10);
}

void SW0(void){
	if(!(GPIOG->IDR & SW[0])){
			  HAL_Delay(delay_time);//디바운스
			  while(!(GPIOG->IDR & SW[0]))//디바운스
			  HAL_Delay(delay_time);//디바운스
			  flag0=1;
			  flag1=0;
			  flag2=0;
			  flag3=0;
    }
}

void SW1(void){
	if(!(GPIOG->IDR & SW[1])){
			  HAL_Delay(delay_time);//디바운스
			  while(!(GPIOG->IDR & SW[1]))//디바운스
			  HAL_Delay(delay_time);//디바운스
			  flag0=0;
			  flag1=1;
			  flag2=0;
			  flag3=0;
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
  MX_UART7_Init();
  /* USER CODE BEGIN 2 */

  //*입출력 설정*//

   GPIOA->MODER=0x01400000;//트랜지스터 output 모드로 설정
   GPIOB->MODER=0x51551154;//led,fnd output 모드로 설정
   GPIOC->MODER=0x00051400;//fnd output 모드로 설정

   //인터럽트 설정
   RCC->AHB1ENR|=(1<<6);//GPIOG 클럭공급
   RCC->APB2ENR|=(1<<14);//SYSCFG으로 설정
   SYSCFG->EXTICR[1]|=(6<<8)|(6<<12); //사용하는 외부 EXIT는 PG 6~7이므로 EXTICR2, PG로 설정할때의 값은 0110
   EXTI->IMR|=(1<<6);//외부인터럽트 EXTI4~7을 NOT Mask(Enable) 시킴
   EXTI->IMR|=(1<<7);
   EXTI->FTSR|=(1<<6);//문제 조건에 따라 누르는 조건은 FALLING Edge로 설정
   EXTI->FTSR|=(1<<7);
   EXTI->RTSR &=~(1<<6);//RISING disable
   EXTI->RTSR &=~(1<<7);
   NVIC->ISER[0]|=(1<<23);//NVIC에 EXTI 4~7을 연결

  //ADC설정
  RCC->AHB1ENR|=(1<<2);//GPIOC 클럭공급 <<이거안하면 adc작동안함
  RCC->APB2ENR|=(1<<8);//ADC1 클럭공급 <<이거안하면 adc작동안함
  GPIOC->MODER|=(3<<8);//ADC핀을 아날로그 입력모드로 설정
  ADC1->SQR1 &= ~(16<<20);//변환할 채널수 설정(주로 SCAN모드, discontinue모드 에서 사용)은 SQR1 레지스터 기본값 0x0000 0000에 의해 single-chanal로 설정됨
  ADC1->SQR3|=0x0000000E;//14채널을 설정
  ADC1->CR1 |= (1<<24);//10비트 분해능 설정
  ADC1->CR2 &= ~(1<<11);//데이터 좌우측 정렬 설정은 CR2 레지스터 기본값 0x0000 0000으로 인해  자동으로 오른쪽정렬이 선택되므로 생략가능
  ADC1->CR2 |=(1<<0);//ADC ON

  //UART설정
  RCC->AHB1ENR|=(1<<4);//GPIOE 클럭공급
  RCC->APB1ENR|=(1<<30);//UART7 클럭공급
  GPIOE->MODER|=(10<<14);//E7,E8을 RX,TX로 사용하기위해 AFIO모드로 설정
  GPIOE->AFR[0]|=(8<<28);//E7을 RX로 사용하기위해 설정
  GPIOE->AFR[1]|=(8<<0);//E8을 TX로 사용하기위해 설정
  UART7->BRR=0x681;//공식에의해 9600 보레이트 설정
  //UART7->BRR=0x85;//115200 보레이트설정
  UART7->CR2 &= ~(1<<10);//패리티비트 disable
  UART7->CR2 &= ~(3<<12);//1 stop bit 설정
  UART7->CR1 &= ~(1<<28);//WORD length 를 1start bit 8 data bit 로 설정
  UART7->CR1 &= ~(1<<12);//WORD length 를 1start bit 8 data bit 로 설정
  UART7->CR1 |= (1<<15);//오버샘플링을 8로 설정
  UART7->CR1 |= (1<<3);//TE비트를 enable
  UART7->CR1 |= (1<<2);//UE비트를 enable
  UART7->CR1 |= (1<<0);//USART Enable

  //초기설정
  LED_off();//초기 led 끄기
  FND_off();//초기 fnd 끄기

  //FND의 초기 세팅값을 00으로 설정
  GPIOA->ODR|=NPN[0];
  GPIOA->ODR|=NPN[1];
  display_num(num);


  char buffer[100]={};//입력한 숫자가 저장될 버퍼
  char data;//입력받는 문자데이터
  int index=0;//인덱스
  int enter_flag=0;//엔터 눌렀는지 확인

  int adcval=0;//센서의 변환된 ADC값

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	//ADC동작문
	ADC1->CR2|=(1<<30);//ADC 활성화비트 ON
	while(!(ADC1->SR && (1<<1)));//ADC 변환이 끝날때 까지 대기
	adcval=ADC1->DR;//ADC값 adcval에 저장

    SW0();//폴링 방식의 1번문제 버튼을 입력받는 함수
    SW1();//폴링 방식의 2번문제 버튼을 입력받는 함수

	if(flag0){//1번 수행문
		for(int i=0; i<=adcval/128; i++){//ADC 값에따라 LED ON
		GPIOB->ODR&=~LED[i];
		}
		LED_off();
	}

	if(flag1){//2번 수행문
		//데이터를 한문자(1byte)씩 보낼수 있기때문에 adc값의 각자리수를 문자형으로 변환
		char ADC_1000=(adcval/1000) + '0';
		char ADC_100=((adcval%1000)/100) + '0';
		char ADC_10=((adcval%100)/10) + '0';
		char ADC_1=(adcval%10) + '0';

		//1000자리 데이터 putty에 출력
		UART7->TDR=ADC_1000;
		while(!(UART7->ISR && (1<<6)));
		HAL_Delay(10);

		//100자리 데이터 putty에 출력
		UART7->TDR=ADC_100;
		while(!(UART7->ISR && (1<<6)));
		HAL_Delay(10);

		//10자리 데이터 putty에 출력
		UART7->TDR=ADC_10;
		while(!(UART7->ISR && (1<<6)));
		HAL_Delay(10);

		//1자리 데이터 putty에 출력
		UART7->TDR=ADC_1;
		while(!(UART7->ISR && (1<<6)));

		//줄바꿈문자 송신
		UART7->TDR='\n';
		while(!(UART7->ISR && (1<<6)));
		HAL_Delay(10);
		UART7->TDR='\r';
		while(!(UART7->ISR && (1<<6)));

		HAL_Delay(100);
	}

    if(flag2|flag3)Dynamic_FND();//2번 혹은 3번 플래그에서 fnd값 표시

    if(flag2_2){//2번 인터럽트가 걸릴때만 활성화
    while(1){
    	if(HAL_UART_Receive(&huart7,&data,1,10)==HAL_OK){//데이터가 들어왔는지 확인

    	   UART7->TDR=data;//입력한 데이터를 putty창에 출력
    	   while(!(UART7->ISR && (1<<6)));//전송이 완료될때 까지 대기

    	   if(data=='\r'){//putty에서 엔터를 쳤을시
    	       buffer[index]='\0';//현재 입력을 문자열의 끝을 알리는 NULL문자로 설정
    	       enter_flag=1;//플래그비트를 1로 만듬
    	       UART7->TDR='\n';
    	       while(!(UART7->ISR && (1<<6)));
    	   }

    	   else{//입력받은 문자가 엔터가 아니라면 버퍼에 계속 저장
    	       buffer[index]=data;
    	       index++;
    	   }

    	   if(enter_flag==1){//입력이 완료됬으면
    	       num=atoi(buffer);//입력했던 문자열 정수형으로 변환
    	       //변수값 초기화
    	       enter_flag=0;
    	       index=0;
    	       memset(buffer, 0, sizeof(buffer));//저장했던 모든 문자열을 삭제한다
    	       flag2_2=0;

    	       flag2=1;//fnd숫자표시
    	       break;
    	   }
    	}
      }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 9600;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_8;
  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */

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
  huart3.Init.OverSampling = UART_OVERSAMPLING_8;
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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_MDC_Pin RMII_RXD1_Pin */
  GPIO_InitStruct.Pin = RMII_MDC_Pin|RMII_RXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_REF_CLK_Pin RMII_MDIO_Pin RMII_CRS_DV_Pin */
  GPIO_InitStruct.Pin = RMII_REF_CLK_Pin|RMII_MDIO_Pin|RMII_CRS_DV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
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

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
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

  /*Configure GPIO pins : RMII_TX_EN_Pin RMII_TXD0_Pin */
  GPIO_InitStruct.Pin = RMII_TX_EN_Pin|RMII_TXD0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

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
