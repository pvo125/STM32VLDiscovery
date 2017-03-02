#include <stm32f10x.h>
#include "header.h"
#include "CAN.h"

extern CANTX_TypeDef CAN_Data_TX;
extern Time_Type Time;
extern Alarm_Type Alarm;

void SetTime_encoder(void){
	uint32_t sec_,sec=0;
	uint16_t day=0;
	uint8_t month=0,year=0;	
	uint8_t tmp,i,leap;
	uint8_t CNT,ARR;
	
	CNT=TIM3->CNT;
	ARR=TIM3->ARR;
	
	TIM3->ARR=19;
	ClearLCD();
	Cursor_type(C_); 
	
	PutChar (&Time.time[0],0x6,5);
	PutChar (&Time.date[0],0x46,10);
	Set_Cursor (0x6);
	Delay(1000000);
	
	EXTI->PR=EXTI_PR_PR3;
	
	for (i=0;i<5;i++)																		// Цикл принятия цифр по USART1 и вывода на экран
	{			
				tmp=Time.time[i];	
				TIM3->CNT=(Time.time[i]-0x30)*2;
repeat_time:			while(!((EXTI->PR&EXTI_PR_PR2)||(TIM3->SR&TIM_SR_CC1IF)||(EXTI->PR&EXTI_PR_PR3))){}
			
			if (EXTI->PR&EXTI_PR_PR3)
			{
				Delay(500000);
				Shift_cursor(Right);
				EXTI->PR=EXTI_PR_PR3;
			}
			else if(TIM3->SR&TIM_SR_CC1IF)
			{ Delay(1000);
				tmp=(TIM3->CNT/2)|0x30;
				PutChar_Curent (&tmp);
				Shift_cursor(Left);
				TIM3->SR&=~TIM_SR_CC1IF;
				goto repeat_time;
			}
			else if (EXTI->PR&EXTI_PR_PR2)
				break;
			if	(i==0)
					sec+=(tmp-0x30)*36000;
				else if (i==1){
					sec+=(tmp-0x30)*3600;	
					Shift_cursor (Right);
					i++;
					}
				else if (i==3)
					sec+=(tmp-0x30)*600;
				else if (i==4)
					sec+=(tmp-0x30)*60;
  }
	Set_Cursor (0x46);														// После ввода времени часы-минуты перейдем к вводу даты
	
	for (i=0;i<10;i++)
	{
				tmp=Time.date[i];	
				TIM3->CNT=(Time.date[i]-0x30)*2;
repeat_date:			while(!((EXTI->PR&EXTI_PR_PR2)||(TIM3->SR&TIM_SR_CC1IF)||(EXTI->PR&EXTI_PR_PR3))){}
			
			if (EXTI->PR&EXTI_PR_PR3)
			{
				Delay(500000);
				Shift_cursor(Right);
				EXTI->PR=EXTI_PR_PR3;
				
			}
			else if(TIM3->SR&TIM_SR_CC1IF)
			{ Delay(1000);
				tmp=(TIM3->CNT/2)|0x30;
				PutChar_Curent (&tmp);
				Shift_cursor(Left);
				TIM3->SR&=~TIM_SR_CC1IF;
				goto repeat_date;
			}
			else if (EXTI->PR&EXTI_PR_PR2)
				break;	
				if	(i==0)
					day+=(tmp-0x30)*10;
				else if (i==1){
					day+=(tmp-0x30);	
					Shift_cursor (Right);
					i++;
					}
				else if (i==3)
					month+=(tmp-0x30)*10;
				else if (i==4)
				{
					month+=(tmp-0x30);
					Shift_cursor (Right);
					Shift_cursor (Right);
					Shift_cursor (Right);
				  i+=3;
					}	
				else if (i==8)
					year+=(tmp-0x30)*10;
				else if (i==9)
					year+=(tmp-0x30);
		 }		
	if (year%4!=0)
		leap=0;
	else
		leap=1;
	for (i=0;i<month;i++)
		day+=days[leap][i];
	while (year>0)
	{
		day+=366;
		year--;
			for(i=0;i<3;i++)
				if (year<=0)
				 break;
				else
				{
				 day+=365;
				 year--;
				}
	 }
	 sec_=RTC->CNTH<<16 |RTC->CNTL;
		if (sec==0 && day==0)
			sec=sec_;
		else if(day==0) 
			sec+=(sec_/86400)*86400;
		else if(sec==0)
			sec=(day-1)*86400+sec_%86400;
		else
			sec+=(day-1)*86400;

	//PWR_BackupAccessCmd(ENABLE);
	RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
	RTC->CNTL = sec;			
	RTC->CNTH = sec>>16;	
	RTC->CRL &=~ RTC_CRL_CNF;																		
	while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}			// Дождемся готовности часов для записи
	//PWR_BackupAccessCmd(DISABLE);
	DateCalc();
	Cursor_type(C_NONE);
	Delay(200000);	
	ClearLCD();
	TIM3->CNT=CNT;
	TIM3->ARR=ARR;	
}

void SetAlarm_encoder(void){
	uint32_t sec_,sec=0;
	uint16_t day=0;
	uint8_t month=0,year=0;	
	uint8_t tmp,i,leap;
	uint8_t CNT,ARR;
	
	CNT=TIM3->CNT;
	ARR=TIM3->ARR;
	
	TIM3->ARR=19;
	ClearLCD();
	Cursor_type(C_); 
	
	PutChar (&Alarm.time[0],0x6,5);
	PutChar (&Alarm.date[0],0x46,10);
	Set_Cursor (0x6);
	Delay(1000000);
	EXTI->PR=EXTI_PR_PR3;
	
	for (i=0;i<5;i++)																		// Цикл принятия цифр по USART1 и вывода на экран
	{			
				tmp=Alarm.time[i];	
				TIM3->CNT=(Alarm.time[i]-0x30)*2;
repeat_time:			while(!((EXTI->PR&EXTI_PR_PR2)||(TIM3->SR&TIM_SR_CC1IF)||(EXTI->PR&EXTI_PR_PR3))){}
			
			if (EXTI->PR&EXTI_PR_PR3)
			{
				Delay(500000);
				Shift_cursor(Right);
				EXTI->PR=EXTI_PR_PR3;
			}
			else if(TIM3->SR&TIM_SR_CC1IF)
			{ Delay(1000);
				tmp=(TIM3->CNT/2)|0x30;
				PutChar_Curent (&tmp);
				Shift_cursor(Left);
				TIM3->SR&=~TIM_SR_CC1IF;
				goto repeat_time;
			}
			else if (EXTI->PR&EXTI_PR_PR2)
				break;
				if	(i==0)
					sec+=(tmp-0x30)*36000;
				else if (i==1){
					sec+=(tmp-0x30)*3600;	
					Shift_cursor (Right);
					i++;
					}
				else if (i==3)
					sec+=(tmp-0x30)*600;
				else if (i==4)
					sec+=(tmp-0x30)*60;
  }
	Set_Cursor (0x46);														// После ввода времени часы-минуты перейдем к вводу даты
	for (i=0;i<10;i++)
	{
				tmp=Alarm.date[i];	
				TIM3->CNT=(Alarm.date[i]-0x30)*2;
repeat_date:			while(!((EXTI->PR&EXTI_PR_PR2)||(TIM3->SR&TIM_SR_CC1IF)||(EXTI->PR&EXTI_PR_PR3))){}
			
			if (EXTI->PR&EXTI_PR_PR3)
			{
				Delay(500000);
				Shift_cursor(Right);
				EXTI->PR=EXTI_PR_PR3;
				
			}
			else if(TIM3->SR&TIM_SR_CC1IF)
			{ Delay(1000);
				tmp=(TIM3->CNT/2)|0x30;
				PutChar_Curent (&tmp);
				Shift_cursor(Left);
				TIM3->SR&=~TIM_SR_CC1IF;
				goto repeat_date;
			}
			else if (EXTI->PR&EXTI_PR_PR2)
				break;	
				if	(i==0)
					day+=(tmp-0x30)*10;
				else if (i==1){
					day+=(tmp-0x30);	
					Shift_cursor (Right);
					i++;
					}
				else if (i==3)
					month+=(tmp-0x30)*10;
				else if (i==4)
				{
					month+=(tmp-0x30);
					Shift_cursor (Right);
					Shift_cursor (Right);
					Shift_cursor (Right);
				  i+=3;
					}	
				else if (i==8)
					year+=(tmp-0x30)*10;
				else if (i==9)
					year+=(tmp-0x30);
		 }		
	if (year%4!=0)
		leap=0;
	else
		leap=1;
	for (i=0;i<month;i++)
		day+=days[leap][i];
	while (year>0)
	{
		day+=366;
		year--;
			for(i=0;i<3;i++)
				if (year<=0)
				 break;
				else
				{
				 day+=365;
				 year--;
				}
	 }
  sec_=BKP->DR1<<16 |BKP->DR2;
	if (sec==0 && day==0)
		sec=sec_;
		else if(day==0) 
			sec+=(sec_/86400)*86400;
			else if(sec==0)
				sec=(day-1)*86400+sec_%86400;
				else
					sec+=(day-1)*86400;
					
	//PWR_BackupAccessCmd(ENABLE);
	RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
	RTC->ALRL = sec;			
	BKP->DR2 = sec;
	RTC->ALRH = sec>>16;	
	BKP->DR1 =sec>>16;
	RTC->CRL &=~ RTC_CRL_CNF;																		
	while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}			// Дождемся готовности часов для записи
	//PWR_BackupAccessCmd(DISABLE);
	
	AlarmCalc();
	Cursor_type(C_NONE);
	Delay(200000);	
	ClearLCD();
	TIM3->CNT=CNT;
	TIM3->ARR=ARR;	
}

void BrezPhase(void){
	uint8_t CNT,ARR;
	Delay(300000);
	CNT=TIM3->CNT;
	ARR=TIM3->ARR;
	TIM3->ARR=100;
	TIM3->CNT=10;
	ClearLCD();
	EXTI->PR=EXTI_PR_PR3;
	// Напишем варианты управления тиристором фазовый и метод Брезенхема
	PutText(Method_Control1,0x0);
	PutText(Method_Control2,0x40);
	
	PutSimvol(0x7F,(0x0E+0x40*PhaseBrez));					// На против вариантов изобразим стрелочку
	
repeat:
				while(!((EXTI->PR&EXTI_PR_PR2)||(TIM3->SR&TIM_SR_CC1IF)||(EXTI->PR&EXTI_PR_PR3))){}	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	if(TIM3->SR&TIM_SR_CC1IF)
		{
			if(TIM3->CNT>10)
				{	PutSimvol(0x7F,0x4E);
				PutSimvol(0x20,0x0E);
				TIM3->CNT=10;
					
				PhaseBrez=1;
				}
			else if(TIM3->CNT<10) 
			{	PutSimvol(0x7F,0x0E);
				PutSimvol(0x20,0x4E);
				TIM3->CNT=10;
				
				PhaseBrez=0;
			}
			
			TIM3->SR&=~TIM_SR_CC1IF;
			goto repeat;}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																					       Фазовый
			else if((EXTI->PR&EXTI_PR_PR3)&&(PhaseBrez==0))
				{	
					ClearLCD();																	
					if(TimerONOFF==0)
						{
							TIM2->CCMR1 &=~TIM_CCMR1_IC1PSC_0;	// Канал захвата настроим на каждый спад импульса
							TIM2->DIER &=~TIM_DIER_CC1IE;				//Если меняем режим на Фазовый то для TIM2 по Capture/Compare 1 interrupt disable
							TIM2->ARR=1000-PhasePower*10;
							TIM2->CCR2=970-PhasePower*10;
							
							TIM2->CCER|=TIM_CCER_CC1E|TIM_CCER_CC2E;		// Enable capture CC1 / Output Enable  CC2 
							
							TIM3->CNT=0;
							TimerONOFF=1;
							PutText(Phase_Method_Enabled,0x0);}
							else
								{	TIM2->CCER &=~(TIM_CCER_CC1E|TIM_CCER_CC2E);		// Disable capture CC1 / Output Disable  CC2 
									TimerONOFF=0;
									PutText(Phase_Method_Disabled,0x0);}
				}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																										Брезенхема		
				else if((EXTI->PR&EXTI_PR_PR3)&&(PhaseBrez==1))			
					{	
						ClearLCD();
						if(TimerONOFF==0)
							{	
								TIM2->CCMR1 |=TIM_CCMR1_IC1PSC_0; 			// Канал захвата настроим - прерывание на каждый второй спад импульса
								TIM2->DIER |=TIM_DIER_CC1IE;						//Если меняем режим на Брезенхема то для TIM2 по Capture/Compare 1 interrupt enable
								TIM2->ARR=45;
								TIM2->CCR2=15;
								
								TIM2->CCER|=TIM_CCER_CC1E|TIM_CCER_CC2E;		// Enable capture CC1 / Output Enable  CC2 
								
								TIM3->CNT=0;
								TimerONOFF=1;
								PutText(Phase_Method_Enabled,0x0);}
							else 
								{	TIM2->CCER &=~(TIM_CCER_CC1E|TIM_CCER_CC2E);	// Disable capture CC1 / Output Disable  CC2 
									TIM2->DIER &=~TIM_DIER_CC1IE;									// Запретим  прерывания по UE в таймере TIM2
									BrezErr=0;
									Brez_Count=0;
									TimerONOFF=0;
									PutText(Phase_Method_Disabled,0x0);}
					}
	Delay(300000);
	TIM3->CNT=CNT;
	TIM3->ARR=ARR;
	EXTI->PR=EXTI_PR_PR3;
	NVIC->ICPR[0]=NVIC_ICPR_CLRPEND_9|NVIC_ICPR_CLRPEND_29;
	ClearLCD();
}

void ADC(void){
	ClearLCD();
	EXTI->PR=EXTI_PR_PR3;
	while(!(EXTI->PR&EXTI_PR_PR2))
	{
		PutText(Vin, 0x0);
		PutChar(ADC_Ch_0,0xA,5);
		PutText(Tchip, 0x40);
		PutChar(Temperature,0x4A,5);
	/*PutChar(ADC_Ch_15,0x40,5);
	PutChar(ADC_Ch_8,0x4A,5);*/
		
	}
	Delay(200000);
}
//
#if 0
void Flash(void){
	uint8_t CNT,ARR,i,m;
	uint8_t Punkt_flash=0;
	CNT=TIM3->CNT;
	ARR=TIM3->ARR;
	TIM3->CNT=0;
	TIM3->ARR=9;
	
	Delay(300000);
	EXTI->PR=EXTI_PR_PR3;
	
repeat:
				ClearLCD();
				for(i=Punkt_flash,m=0;(i<2+Punkt_flash)&&(m<2);i++,m++)	//		0		Read_Flash[]=	"Read Flash";
					{ 																										//		1		PagePrg[]=		"Prog page";					
						if(i==4)																						// 		2 	ReadID[]=			"Read ID";
						i=0;																								//		3 	Read_StatusReg[]="Read Status";
					PutText(Menu_flash[i],0x3+m*64);
					}																												
					PutSimvol(0x7F,0x0F);																	// На против вариантов изобразим стрелочку
				while(!((EXTI->PR&EXTI_PR_PR2)||(TIM3->SR&TIM_SR_CC1IF)||(EXTI->PR&EXTI_PR_PR3))){}	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
					if(TIM3->SR&TIM_SR_CC1IF)
							{
								TIM3->SR&=~TIM_SR_CC1IF;																// Если крутим ручку энкодера 
								Punkt_flash=TIM3->CNT/2;
								goto repeat;
							}
////////////////////////////////////////////////////////////////////						
					else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_flash==0))				//	Пункт для чтения flash
							{	
								ClearLCD();
								EXTI->PR=EXTI_PR_PR3;
								SysTick->VAL=0;
								//Read_MX25L_DMA(0x21c20,4000);
								//PutChar(&SPIBufferRX[1000],0x0,8);
								while(!(EXTI->PR&EXTI_PR_PR2)){}
								EXTI->PR=EXTI_PR_PR2;
									goto repeat;		
							}
///////////////////////////////////////////////////////////////////
					else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_flash==1))				//	Пункт для программирования страницы MX25L
							{
								ClearLCD();
								EXTI->PR=EXTI_PR_PR3;
								//PagePrg_MX25L(0x30000,SPIBufferRX);
								PutChar(&SPIBufferRX[0],0x0,8);
								while(!(EXTI->PR&EXTI_PR_PR2)){}
								EXTI->PR=EXTI_PR_PR2;
								goto repeat;		
							}
///////////////////////////////////////////////////////////////////								
					else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_flash==2))			// Пункт для чтения ID flash
							{
								ClearLCD();
								EXTI->PR=EXTI_PR_PR3;
								//MX25L_ID=ReadID_MX25L();
								//PutChar(&MX25L_ID,0x0,8);	
								while(!(EXTI->PR&EXTI_PR_PR2)){}
								EXTI->PR=EXTI_PR_PR2;
								goto repeat;			
							}
//////////////////////////////////////////////////////////////////		// Пункт для чтения Status Registr flash							
					else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_flash==3))
							{	
								ClearLCD();
								EXTI->PR=EXTI_PR_PR3;
								//Status_Reg=ReadStatusReg_MX25L();
								PutChar(&Status_Reg,0x0,1);	
								while(!(EXTI->PR&EXTI_PR_PR2)){}
								EXTI->PR=EXTI_PR_PR2;
								goto repeat;			
							}	
//////////////////////////////////////////////////////////////////
					TIM3->CNT=CNT;
					TIM3->ARR=ARR;						
					ClearLCD();						
}
#endif

