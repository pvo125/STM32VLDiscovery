#include <stm32f10x.h>
#include "header.h"


extern volatile BUTTON_TypeDef button;

void _Menu(void){									 									//	*Menu	
					uint8_t i=0,m;
					uint8_t /*tmp_CNT,*/tmp_ARR;
					
					EXTI->PR=EXTI_PR_PR2;
					NVIC->ICER[0]=NVIC_ICER_CLRENA_7|NVIC_ICER_CLRENA_9|NVIC_ICER_CLRENA_29; //EXTI1 EXTI3 TIM3 disable NVIC
					//tmp_CNT=TIM3->CNT;
					tmp_ARR=TIM3->ARR;
					Punkt_menu=0;
					TIM3->ARR=7;
					TIM3->CNT=0;
					
					
repeat: 																												//		0		Set_Time      = "SET TIME"
					ClearLCD();																						//		1		Set_Alarm     = "SET ALARM"
					for(i=Punkt_menu,m=0;(i<2+Punkt_menu)&&(m<2);i++,m++)	//		2		Method_Control= "TIM CONTROL"
					{ 																										//					
						if(i==4)																						// 		3 	ADC						= "ADC View"
						i=0;																								//		
					PutText(Menu[i],0x3+m*64);
					}																												
					PutSimvol(0x7F,0x0F);					// На против вариантов изобразим стрелочку
					while(!((EXTI->PR&EXTI_PR_PR2)||(TIM3->SR&TIM_SR_CC1IF)||(EXTI->PR&EXTI_PR_PR3))){}	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

				if(TIM3->SR&TIM_SR_CC1IF)
					{TIM3->SR&=~TIM_SR_CC1IF;																// Если крутим ручку энкодера 
					 Punkt_menu=TIM3->CNT/2;
											goto repeat;}
////////////////////////////////////////////////////////////////////						
					 else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_menu==0))
						{	
							SetTime_encoder();
							if(EXTI->PR&EXTI_PR_PR2)														//	Пункт для установки времени 	
							{ EXTI->PR=EXTI_PR_PR2;
								goto repeat;}		
						}
///////////////////////////////////////////////////////////////////
						else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_menu==1))
							{
								SetAlarm_encoder();
								if(EXTI->PR&EXTI_PR_PR2)											//	Пункт для установки будильника 
								{ EXTI->PR=EXTI_PR_PR2;
									goto repeat;}
							}
///////////////////////////////////////////////////////////////////								
							else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_menu==2))
								{
								BrezPhase();
									if(EXTI->PR&EXTI_PR_PR2)											// Пункт для включения таймера 
									{EXTI->PR=EXTI_PR_PR2;
										goto repeat;}
								}
//////////////////////////////////////////////////////////////////								
								/*else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_menu==3))
									{		SCB->SCR |=SCB_SCR_SLEEPDEEP;								// Уходим в слипдип
											Enable_Sleep =!Enable_Sleep;
											Delay(300000);
											EXTI->PR=EXTI_PR_PR3;
									}*/
///////////////////////////////////////////////////////////////////								
									else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_menu==3))
										{
											ADC();
											if(EXTI->PR&EXTI_PR_PR2)											// Пункт для индикации каналов АЦП 
											{EXTI->PR=EXTI_PR_PR2;
												goto repeat;}
										}
//////////////////////////////////////////////////////////////////										
							
									/*else if((EXTI->PR&EXTI_PR_PR3)&&(Punkt_menu==5))
										{
											Flash();
											if(EXTI->PR&EXTI_PR_PR2)											// Пункт для Flash menu
											{EXTI->PR=EXTI_PR_PR2;
												goto repeat;}
										}	*/
	
///////////////////////////////////////////////////////////////////	
										else 
										{ Delay(300000);
											EXTI->PR=EXTI_PR_PR2;
										}	
					
					button=BUTTON_DISABLE;
										
					//TIM3->DIER |=TIM_DIER_CC1IE;					  					// Разрешим Capture/Compare 1 interrupt enable
					NVIC->ICPR[0]=NVIC_ICPR_CLRPEND_7|NVIC_ICPR_CLRPEND_9|NVIC_ICPR_CLRPEND_29;
					NVIC->ISER[0]=NVIC_ISER_SETENA_7|NVIC_ISER_SETENA_9|NVIC_ISER_SETENA_29;							
					if(PhaseBrez)
					{
						TIM3->CNT=BrezPower;//tmp_CNT;
						TIM3->ARR=tmp_ARR;						
					}
					else
					{
						TIM3->CNT=PhasePower;//tmp_CNT;
						TIM3->ARR=tmp_ARR;
					
					}
						ClearLCD();		
}
