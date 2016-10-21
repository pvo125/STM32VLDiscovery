#include <stm32f10x.h>
#include "header.h"


Time_Type 								Time;
Alarm_Type 								Alarm;

volatile BUTTON_TypeDef		button=BUTTON_DISABLE;

Count_Type 								CNT;

GPIO_InitTypeDef 					GPIO_InitStruct;
ADC_InitTypeDef 					ADC_InitStruct;
TIM_TimeBaseInitTypeDef 	TIM_TimeBaseInitStruct;
TIM_OCInitTypeDef 				TIM_OCInitStruct;
DMA_InitTypeDef						DMA_InitStruct;

void InitPerifery(void);
void ADC1_Init (void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main (void){ 
	
	InitPerifery ();
	ADC1_Init();
	LCDInit();
	DateCalc();
	AlarmCalc();	
	EXTI->IMR	= EXTI_IMR_MR17|EXTI_IMR_MR1|EXTI_IMR_MR2|EXTI_IMR_MR3;
	if(Error_LSE==1)
	{	PutText (OSC32_Failed,0x0);
		Delay(1000000);
	ClearLCD();}
	while(1)
	{
		
		NVIC->ICER[0]=NVIC_ICER_CLRENA_7|NVIC_ICER_CLRENA_9|NVIC_ICER_CLRENA_29;
		PutChar (&Time.time[0],0x0,8);
		if((BKP->DR1<<16|BKP->DR2)>=(RTC->CNTH<<16|RTC->CNTL))
			PutSimvol (0xEB,0xF);
		else 
			PutSimvol (0x20,0xF);
		PutChar (&Time.date[0],0x40,10);
		if(TimerONOFF)
		{
			if(PhaseBrez)
				PutSimvol('B',0x4F);
			else
				PutSimvol('P',0x4F);
		}
		NVIC->ISER[0]=NVIC_ISER_SETENA_7|NVIC_ISER_SETENA_9|NVIC_ISER_SETENA_29;		
		if(Enable_Sleep==1) 
			Sleepdeep();
		if(button==BUTTON_SET) 
			_Menu();
	}
}	

void ADC1_Init (void){
		uint8_t i;
//-----------------------------------------------------------------/
//				Включим тактирование АЦП1 и делитель для АЦП1 на 12Мгц /	
//-----------------------------------------------------------------/	
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

// GPIOx config ------------------------------------------------------
/*PA0=ADC_IN0 PC4=ADC_IN14 PC5=ADC_IN15 PB0=ADC_IN8  */
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
// DMA1 Channel1------------------------------------------------------
		DMA_InitStruct.DMA_BufferSize=2;			
		DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;
		DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;
		DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)&ADC1Buff[0];
		DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
		DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
		DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;
		DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR;
		DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
		DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
		DMA_InitStruct.DMA_Priority=DMA_Priority_Low;
		DMA_Init(DMA1_Channel1, &DMA_InitStruct);
/*Enable DMA1*/
		DMA_Cmd(DMA1_Channel1, ENABLE);

//TIM4 config ---------------------------------------------------------
		TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Period=10100;
		TIM_TimeBaseInitStruct.TIM_Prescaler=7199;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
		TIM_OCInitStruct.TIM_Pulse=10000;
		TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
		TIM_OC4Init(TIM4, &TIM_OCInitStruct);
		/*Enable TIM4*/		
		TIM_Cmd(TIM4, ENABLE);
	
//ADC1 config --------------------------------------------------------
		ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;
		ADC_InitStruct.ADC_ScanConvMode=ENABLE;
		ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;
		ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;
		ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T4_CC4;
		ADC_InitStruct.ADC_NbrOfChannel=2;																// Задаем количество каналов для регулярного преобразования
		ADC_Init(ADC1,&ADC_InitStruct);
		
		
		ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
		ADC_DMACmd(ADC1, ENABLE);	
		ADC_TempSensorVrefintCmd(ENABLE);
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_239Cycles5);
		/*ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime_239Cycles5);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 4, ADC_SampleTime_239Cycles5);*/
		/*ADC_DiscModeChannelCountConfig(ADC1, 4);
		ADC_DiscModeCmd(ADC1, ENABLE);*/
		ADC_ExternalTrigConvCmd(ADC1, ENABLE);
		ADC_Cmd(ADC1, ENABLE);
		
		ADC_StartCalibration(ADC1);
		while(ADC_GetCalibrationStatus(ADC1)){};
/*Инициализируем ячейки памяти - разделительные точки вольты миливольты  */		
		for (i=0;i<2;i++)	
		ADC_Ch[i][1]=0x2E;	
}


void InitPerifery(void){
//---------------------------------------------------------------------------------------------------------------------		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																									RTC Initialization	
	
		RCC->APB1ENR |= RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN;						// Включим тактирование BKP и PWR интерфейса  
		PWR_BackupAccessCmd(ENABLE);																					// Разрешим доступ к бэкап домену установкой бита DBP
		// Настроим SysTick сбросим флаг CLKSOURCE выберем источник тактирования AHB/8
	
	SysTick->CTRL &=~SysTick_CTRL_CLKSOURCE;
	SysTick->CTRL |=SysTick_CTRL_ENABLE_Msk;	
	
		if ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY)  							// Проверим если часы запущены то выход из инициализации
		// Если часы не включены проинициализируем их и загрузим значения делителя и начальное значение счетчика
			{	
				RCC->BDCR |= RCC_BDCR_BDRST;																	// Установим сброс на BKP  домен 
				RCC->BDCR &=~ RCC_BDCR_BDRST;																	// Снимим сброс с BKP  домена 
				RCC->BDCR |= RCC_BDCR_LSEON;																	// Включим 	усилитель кварца 32768		
																																			// Дождемся установки бита готовности LSERDY
				SysTick->LOAD=16000000;
				SysTick->VAL=0;
				while(!((SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk)||(RCC->BDCR&RCC_BDCR_LSERDY))){}	/*Ждем установки LSERDY или примерно 1.5 сек*/
				if(RCC->BDCR&RCC_BDCR_LSERDY)																		/*Если флаг установился настраиваем RTC*/	
					{
					RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
					RCC->BDCR|=RCC_BDCR_RTCEN;
					while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}					// Дождемся готовности часов для записи
					RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
					RTC->PRLL = 0x7fff;																						// Загрузим в предделитель число 0x7fff 
					RTC->ALRH =0x1AA6;
					RTC->ALRL =0x8280;	
					RTC->CRL &=~ RTC_CRL_CNF;																		
					while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}					// Дождемся готовности часов для записи
					}
				else																														// Если флаг не установился настроим часы от HSE
					{
					RCC->BDCR |= RCC_BDCR_RTCSEL_HSE;															// Выберем в качестве источника генератор HSE 
					RCC->BDCR |= RCC_BDCR_RTCEN;
					while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}					// Дождемся готовности часов для записи
					RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
					RTC->PRLL = 0xf423;																						// Загрузим в предделитель число 0x7fff 
					RTC->ALRH =0x1AA6;
					RTC->ALRL =0x8280;	
					RTC->CRL &=~ RTC_CRL_CNF;																		
					while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}
						Error_LSE=1;																								// Переменнная-флаг для индикации ошибки не запуска	LSE
					}
				RTC->CRL &=~RTC_CRL_RSF;																			// После запуска часов сбросим флаг RSF	
				while ((RTC->CRL &RTC_CRL_RSF)!=RTC_CRL_RSF){}								// И дождемся его аппаратной установки
					
				}
				RTC->CRH |= RTC_CRH_SECIE;																			// Разрешим прерывание от секундных импульсов в модуле RTC SECIE
//---------------------------------------------------------------------------------------------------------------------		
			
				
			//	BKP_RTCOutputConfig(BKP_RTCOutputSource_Alarm);
			//	PWR_BackupAccessCmd(DISABLE);
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																									GPIO Initialization		

// Включим тактирование GPIOA, B, C  AFIO USART1, TIM2
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN|
									RCC_APB2ENR_IOPAEN|
									RCC_APB2ENR_IOPBEN|
									RCC_APB2ENR_IOPCEN|
									RCC_APB2ENR_IOPDEN
									/*RCC_APB2ENR_USART1EN*/;
										
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

// Освободим линии PA15 PB3 от линий JTDI JTDO 
	 AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_1;
		
// Настраиваем выводы для таймера TIM2: PA0 как Input pull-up  (TIM2_CH1_ETR)
//																			PA1 как Alternate function push-pull  (TIM2_CH2)
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
		
// Для таймера TIM3 будем настраивать порты PA6 PA7 input Pull-up
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
				
	
// Для контроллера EXTI Настраиваем выводы  PC0 PC1 PC2 как Input pull-up с подтяжкой к + 
	/*GPIOC->CRL |= GPIO_CRL_CNF0_1|GPIO_CRL_CNF1_1|GPIO_CRL_CNF2_1;
	GPIOC->CRL &=~(GPIO_CRL_CNF0_0|GPIO_CRL_CNF1_0|GPIO_CRL_CNF2_0);
	GPIOC->ODR|=(uint16_t)0x0007;		*/
  
	//Конфигурируем выводы порта B для управления ЖК индмкатором (GPIOB8....GPIOB11)  Push-pull  10MHz
	//	DB4---GPIOB8
	//	DB5---GPIOB9	
	//	DB6---GPIOB10	
	//	DB7---GPIOB11
	GPIOB->CRH |= GPIO_CRH_MODE8_0|GPIO_CRH_MODE9_0|GPIO_CRH_MODE10_0|GPIO_CRH_MODE11_0;
	GPIOB->CRH &=~ GPIO_CRH_CNF8_0 &~GPIO_CRH_CNF9_0 &~GPIO_CRH_CNF10_0 &~GPIO_CRH_CNF11_0;
	
//Конфигурируем выводы порта C для управления ЖК индмкатором (GPIOC10 GPIOC11) и светодиодами (GPIOC8 GPIO9) Push-pull  10MHz
	GPIOC->CRH |= GPIO_CRH_MODE8_0|GPIO_CRH_MODE9_0|GPIO_CRH_MODE10_0|GPIO_CRH_MODE11_0;
	GPIOC->CRH &=~ GPIO_CRH_CNF8_0 &~GPIO_CRH_CNF9_0 &~GPIO_CRH_CNF10_0 &~GPIO_CRH_CNF11_0;
	
	// Включим транзистор управляющий питанием и подсветкой LCD с ножки порта PC6 
	GPIOC->CRL |=GPIO_CRL_MODE6_1;
	GPIOC->BSRR =GPIO_BSRR_BR6;
//---------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																					EXTI Initialization
	
	EXTI->FTSR = EXTI_FTSR_TR1|EXTI_FTSR_TR2|EXTI_FTSR_TR3;
	EXTI->RTSR|= EXTI_RTSR_TR17;

	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI1_PC|AFIO_EXTICR1_EXTI2_PC|AFIO_EXTICR1_EXTI3_PC;

//----------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																					USART1 Initialization
	// Инициализация модуля USART1
	// Настройка выводов порта А Tx Rx
	//Сделаем ремап на TX/PB6 RX/PB7
	AFIO->MAPR|=AFIO_MAPR_USART1_REMAP;
	// Tx  AF push-pull  output 10MHz
	GPIOB->CRL |= GPIO_CRL_MODE6_0;
	GPIOB->CRL |= GPIO_CRL_CNF6_1;
	GPIOB->CRL &=~ GPIO_CRL_CNF6_0;
	// Rx по умолчанию настроен Input floating
	//Задаем скорость передачи 72000000/2400=7530  
	USART1->BRR = 0x7530;
	USART1->CR1 |= USART_CR1_RE |/*USART_CR1_TE |*/USART_CR1_UE|USART_CR1_RXNEIE; 
	
//---------------------------------------------------------------------------------------------------------------------												
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 															Инициализация таймера TIM2
//Настройка канала захвата
	TIM2->SMCR |=TIM_SMCR_TS_0|				//Источник синхронизации внешнего сигнала Filtered Timer Input 1(TI1FP1)
							 TIM_SMCR_TS_2|				// TS[2:0] 101
					  	 TIM_SMCR_SMS_1|
							 TIM_SMCR_SMS_2;			// SMS[2:0] 110 Slave mode selection выбираем Trigger mode
	TIM2->CCMR1 |=TIM_CCMR1_CC1S_0;		// CC1 Channel is configured as input, IC1 is mapped on TI1

// Настроим входной фильтр триггера для захвата входного сигнала по спадающему фронту
	TIM2->CCER|=TIM_CCER_CC1P;

// Настройка канала сравнения
	// Выбираем режим  PWM mode 2  установкой битов OC1M регистра TIM2->CCMR1 [2:0]  111
	TIM2->CCMR1 |=TIM_CCMR1_OC2M|TIM_CCMR1_OC2PE; 			//Output compare 2 preload enable
	//Зададим коэффициент деления для CK_CNT=CK_PSC/(PSC[15:0]+1)  72000000/(719+1) =100KHz
	TIM2->PSC=719;
	//Включим режим одного импульcа и предварительной загрузки регистра ARR
	TIM2->CR1 |=TIM_CR1_OPM|TIM_CR1_ARPE;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
//																			Инициализация таймера TIM3
//Настройка канала захвата
	TIM3->SMCR |= TIM_SMCR_SMS_0;//|TIM_SMCR_SMS_1;		// SMS[2:0] 011 Encoder mode 1																			
	TIM3->CCMR1|=TIM_CCMR1_CC1S_0|										//
							 TIM_CCMR1_CC2S_0;										//
	TIM3->ARR=193;
	TIM3->CCER |=TIM_CCER_CC1E;												// Enabled канал захвата CC1
	
	TIM3->DIER |=TIM_DIER_CC1IE;					  					// Разрешим Capture/Compare 1 interrupt enable
	TIM3->CR1 |=TIM_CR1_CEN;
	
	
//--------------------------------------------------------------------------------------------------------------	
	//PWR->CR |=PWR_CR_CWUF;
						//PWR_CR_CSBF;
	// Настроим режим STOP. По команде WFE контроллер переходит в STOP Mode. 
	PWR->CR |=PWR_CR_LPDS;						//LPDS=1
	PWR->CR &=~PWR_CR_PDDS;						//PDDS=0
	
	DBGMCU->CR |=DBGMCU_CR_DBG_SLEEP|DBGMCU_CR_DBG_STOP;
	DBGMCU->CR |=DBGMCU_CR_DBG_TIM2_STOP|
				    	 DBGMCU_CR_DBG_TIM4_STOP;
	
/* Проинициализируем ячейки памяти для отображения двоеточия и точки в изображении даты и времени*/
	Time.time[2]=0x3A;
	Time.time[5]=0x3A;
	Time.date[2]=0x2E;
	Time.date[5]=0x2E;
	Time.date[6]=0x32;
	Time.date[7]=0x30;
	
	Alarm.time[2]=0x3A;
	Alarm.time[5]=0x3A;
	Alarm.date[2]=0x2E;
	Alarm.date[5]=0x2E;
	Alarm.date[6]=0x32;
	Alarm.date[7]=0x30;
	CNT.persent=0x25;
	Temperature[2]=0x2E;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																			/*NVIC IRQ Enable*/
	NVIC_EnableIRQ(RTC_IRQn);            
	NVIC_SetPriority(RTC_IRQn,3);					  //Приоритет RTC=3
	
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_SetPriority(EXTI1_IRQn,3);					//Приоритет EXTI0=3
	
	/*NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_SetPriority(EXTI2_IRQn,3);	*/				//Приоритет EXTI2=3
	
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_SetPriority(EXTI3_IRQn,3);					//Приоритет EXTI3=3
	
	
	NVIC_EnableIRQ(ADC1_2_IRQn);
	NVIC_SetPriority(ADC1_2_IRQn,2);					//Приоритет ADC1=2
	
	
	
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn,2);					//Приоритет TIM2=2
	
	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn,3);					//Приоритет TIM3=3
	
	NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	NVIC_SetPriority(DMA1_Channel2_IRQn,1);	
	//NVIC_EnableIRQ(RTCAlarm_IRQn);
	//NVIC_SetPriority(RTCAlarm_IRQn,3);			//Приоритет ALARM=3
	/*NVIC_EnableIRQ(SPI1_IRQn);
	NVIC_SetPriority(SPI1_IRQn,2);	*/		//Приоритет SPI1=2
}

void DateCalc(void){																		// Функция Вычисления даты по значению секунд 
	int day=1, year=0,month=0, i,leap;										// с регистра RTC->CNT
	day+=(RTC->CNTH<<16 |RTC->CNTL)/86400;
	// Сначала вычислим год. Сохраним значение в ячейке year	
		while (day>=366)
			{		
				day-=366;
				year++;
				for (i=0;i<3;i++)
					{	
						if (day<=365)
						break;
						day-=365;
						year++;
					}
			 }
			 // Проверим является ли полученный год високосным	
	 if(year%4!=0)
			leap=0;
		else
			leap=1;
		// Остаток дней от вычисления года переведем в месяцы и дни		
		for (i=0; day>days[leap][i];i++)
				{
				month++;
				day-=days[leap][i];
				}
				// Запишем все в соответ. ячейки памяти предварительно приведя к виду удобному для индикации 		
		Time.date[8]=(year/10) |0x30;
		Time.date[9]=(year%10) |0x30;
		Time.date[3]=(month/10) |0x30;
		Time.date[4]=(month%10) |0x30;
		Time.date[0]=(day/10) |0x30;
		Time.date[1]=(day%10) |0x30;	
}
void AlarmCalc(void){																		// Функция Вычисления даты и времени будильника по значению секунд 
	int day=1,sec,year=0, month=0, i,leap,tmp;						// взятых c регистра BKP->DR1 BKP->DR2
	sec=((BKP->DR1)<<16 |BKP->DR2)%86400;
	
	// Сначала вычислим минуты и часы и приведем к виду для индикации. Запишем в соответ. ячейки памяти 	 
		
		Alarm.time[0] =(sec/36000)|0x30;
		Alarm.time[1] =((sec/3600)%10)|0x30;
		tmp =sec%3600; 
		Alarm.time[3] =(tmp /600)|0x30;
		Alarm.time[4] =((tmp /60)%10)|0x30;
	
	// Далее вычислим число дней.
	day+=(BKP->DR1<<16 |BKP->DR2)/86400;
	//По количеству дней будем определять год. Сохраним полученное значение в ячейке year/	
	while (day>=366)
		{		
			day-=366;
			year++;
			for (i=0;i<3;i++)
				{	
					if (day<=365)
					break;
					day-=365;
					year++;
				 }
			}
			// Проверим полученный год на високосность	
	if(year%4!=0)
		leap=0;
	else
		leap=1;
	// Из остатка дней вычислим сначала месяц а остаток будет днеём месяца	
	for (i=0; day>days[leap][i];i++)
			{
			month++;
			day-=days[leap][i];
			}
			// Запишем все в соответ. ячейки памяти приведя к виду убодному для индикации.	
	Alarm.date[8]=(year/10) |0x30;
	Alarm.date[9]=(year%10) |0x30;
	Alarm.date[3]=(month/10) |0x30;
	Alarm.date[4]=(month%10) |0x30;
	Alarm.date[0]=(day/10) |0x30;
	Alarm.date[1]=(day%10) |0x30;	
}



//-----------------------------------------------------------------------------------------------------------------------------------------
