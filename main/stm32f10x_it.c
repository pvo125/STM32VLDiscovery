#include <stm32f10x.h>
#include "header.h"
#include "CAN.h"

extern Time_Type 									Time;
extern volatile BUTTON_TypeDef		button;
extern Count_Type 								CNT;

extern volatile uint8_t refreshLCD_encoder;
extern volatile uint8_t refresh_lcd;
volatile uint8_t timesleep=0;	

void RTC_IRQHandler (void){															// Функция Вычисления времени. Каждое прерывание от секунд	
	int tmp, tmp1;																				// будем вычислять часы-минуты -секунды и обновлять соответ. ячейки памяти.
	static uint8_t count_refresh=0;
		
		tmp=(RTC->CNTH<<16)|RTC->CNTL;
		tmp %= 86400;		
		if (tmp==0)																			// Если настала полночь
			DateCalc();																	// то "сбегаем" обновим дату в соотв. ячейках памяти.
	
	// Вычисленное время запишем в соотв. ячейки памяти чтобы выйдя из прерывания отображать текущее время на экране 	
	  Time.time[0] =((tmp/3600)/10)|0x30;
		Time.time[1] =((tmp/3600)%10)|0x30;
		tmp1 =tmp%3600; 
		Time.time[3] =((tmp1 /60)/10)|0x30;
		Time.time[4] =((tmp1 /60)%10)|0x30;
		
		RTC->CRL &=~RTC_CRL_SECF;
		
		count_refresh++;
		if(count_refresh>1)
		{
			count_refresh=0;
			refresh_lcd=1;
		}
		timesleep++;
		if(timesleep>30)
		{
			timesleep=0;
			Enable_Sleep=1;
			SCB->SCR |=SCB_SCR_SLEEPDEEP;
		}
	
}

void EXTI1_IRQHandler (void){ 													// Функция перевода STM32 в режим Standby
	
	if(Enable_Sleep==1)
	{
	SystemInit();
	SCB->SCR &=~SCB_SCR_SLEEPDEEP;
	}
	else
		SCB->SCR |=SCB_SCR_SLEEPDEEP;
	
	Delay(300000);	
	Enable_Sleep =!Enable_Sleep;
	
	EXTI->PR=EXTI_PR_PR1;
		
}	
void EXTI3_IRQHandler(void){
	
	
	Delay(300000);
	button=BUTTON_SET;
	EXTI->PR=EXTI_PR_PR3;

}	

void ADC1_2_IRQHandler (){
		uint16_t temp, temp1;
		uint8_t i;
		
	for(i=0;i<2;i++)
	{
		temp= (3000*ADC1Buff[i])/4095;
		ADC_Ch[i][0]=temp/1000|0x30;
		ADC_Ch[i][2]=(temp%1000)/100|0x30;
		temp1=(temp%1000)%100;
		ADC_Ch[i][3]=(temp1/10)|0x30;
		ADC_Ch[i][4]=(temp1%10)|0x30;
	}
		
		
	
		temp= (3000*ADC1Buff[1])/4095;
		temp1=((1400-temp)*10000)/4478+250;
		Temperature[0]=temp1/100|0x30;
		Temperature[1]=(temp1%100)/10|0x30;
		Temperature[3]=(temp1%100)%10|0x30;
		
	
}

/*------------------------------------------------------------*/
void TIM2_IRQHandler (void){
		TIM2->SR &=~TIM_SR_CC1IF;
		//Brez_Count++; 
		BrezErr+=BrezKoeff;
		if (BrezErr>.5f)
		{
			TIM2->CCER|=TIM_CCER_CC2E;  //Подключаем выход таймера к каналу сравнения
			BrezErr-=1;
		}
		else 
			TIM2->CCER &=~TIM_CCER_CC2E;	//Отключаем выход таймера от канала сравнения
}

void TIM3_IRQHandler (void){
				
	TIM3->SR &=~TIM_SR_CC1IF;	
	NVIC->ICPR[0]=NVIC_ICPR_CLRPEND_29;
	refreshLCD_encoder=1;		
}


void RTCAlarm_IRQHandler (void){
	/*TIM2->ARR=31;
	TIM2->CCR2=1;
	TIM3->CNT=16;
	Power_Value=10;
	Brez_Phase='B';
	Brez_Count=0;
	TIM2->DIER |=TIM_DIER_UIE;*/												//Разрешим прерывания по UE в таймере TIM2	
	EXTI->PR=EXTI_PR_PR17;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  This function handles USB_LP_CAN1_RX0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void){
	
	CAN_Receive_IRQHandler(0);
	CAN_RXProcess0();
	
}
void CAN1_RX1_IRQHandler(void){
	
	CAN_Receive_IRQHandler(1);
	CAN_RXProcess1();
	timesleep=0;
	
}

