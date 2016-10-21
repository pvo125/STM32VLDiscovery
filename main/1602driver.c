#include <stm32f10x.h>
#include "header.h"
//
void Delay (uint32_t ticks){														// Функция задержки на таймере SysTick
	SysTick->LOAD=(ticks*9);
	SysTick->VAL=0;
	while(!(SysTick->CTRL &SysTick_CTRL_COUNTFLAG_Msk)){}	
}

//
void LCDInit (void){
	
	// Пауза 40ms
	Delay(15000);																		
		
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0 
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =(3<<8);																								// A0 R/W DB7 DB6 DB5 DB4 
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// 0  0   0   0   1   1
	Delay(4100);																				// Пауза 4.1ms
	// Function set
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =(3<<8);																								// A0 R/W DB7 DB6 DB5 DB4 
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// 0  0   0   0   1   1
	Delay(100);																				// Пауза 100us
	// Function set
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =(3<<8);																								// A0 R/W DB7 DB6 DB5 DB4 
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// 0  0   0   0   1   1
	Delay(40);																				// Пауза 100us
	// Function set
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =(2<<8);																								// A0 R/W DB7 DB6 DB5 DB4 
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// 0  0   0   0   1   0
	Delay(40);																				// Пауза 100us 
	// Function set
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0D000200;
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// A0 R/W DB7 DB6 DB5 DB4
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1						// 0  0   0   0   1   0
	GPIOB->BSRR =0x07000800;																						// 0  0   1   0   0   0	
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0
	Delay(40);
	// Display ON/OFF control
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0F000000;
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// A0 R/W DB7 DB6 DB5 DB4
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1						// 0  0   0   0   0   0
	GPIOB->BSRR =0x07000800;																						// 0  0   1   0   0   0
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0
	Delay(40);
	// Clear Display 
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0F000000;
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// A0 R/W DB7 DB6 DB5 DB4
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1						// 0  0   0   0   0   0
	GPIOB->BSRR =0x0E000100;																						// 0  0   0   0   0   1
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0
	Delay(1700);
	// Entry Mode Set 
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0F000000;
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// A0 R/W DB7 DB6 DB5 DB4
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1						// 0  0   0   0   0   0
	GPIOB->BSRR =0x09000600;																						// 0  0   0   1   1   0
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0
	Delay(40);
// Display ON/OFF control
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0F000000;
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// A0 R/W DB7 DB6 DB5 DB4
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1						// 0  0   0   0   0   0
	GPIOB->BSRR =0x03000C00;																						// 0  0   1   1   0   0
	Delay(1);
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0
	Delay(40);
}



//
void ClearLCD (void){
	// Clear Display 
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0F000000;
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// A0 R/W DB7 DB6 DB5 DB4
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1						// 0  0   0   0   0   0
	GPIOB->BSRR =0x0E000100;																						// 0  0   0   0   0   1
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0
	Delay(1600);
}



// Функция установки курсора в позицию pos для последующего вывода символа
 void Set_Cursor (uint8_t pos){
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos |0x80)<<4;																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos<<8);																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	Delay(40);																				// Пауза 40us
}



//Функция сдвига курсора на одну позицию влево 
 void Shift_cursor (uint32_t shift){	
	GPIOC->BSRR =GPIO_BSRR_BR10;											// A0=0 
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0E000100; 																									// A0 R/W DB7 DB6 DB5 DB4
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0									// 0  0   0   0   0   1
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1									// 0  0   0   1   0   0
	GPIOB->BSRR =0x0F000F00 & (~shift);																							
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	Delay(40);																				// Пауза 40us
}



// Функция стирания символа на n позиций влево или вправо
 void Backspace_cursor(uint32_t shift,uint8_t n){
		int i;
	 for (i=0;i<n;i++)
	 {
		Shift_cursor (Left);
		GPIOC->BSRR =GPIO_BSRR_BS10;				// A0=1 RW=0
		GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
		GPIOB->ODR =0x00000200; 																									
		GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0									
		
		GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1									
		GPIOB->ODR =0x00000000; 																							
		GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
		Delay(40);																				// Пауза 40us
		Shift_cursor (Left);
	 }
}





// Функция вывода символов c массива simvol[] в позицию pos
 void PutText(uint8_t array[],char pos){
	char i;
	// Устанавливаем курсор в позицию pos
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0 
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos |0x80)<<4;																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos<<8);																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	Delay(40);																				// Пауза 40us
	
	 // Выводим символы с ячейки STM32
	for (i=0; array[i] !='\0'; i++){
	GPIOC->BSRR =GPIO_BSRR_BS10;				// A0=1 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR = array[i]<<4;																				
	GPIOC->BSRR =GPIO_BSRR_BR11;										 	// E=0											
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR = array[i]<<8;
	GPIOC->BSRR =GPIO_BSRR_BR11;										 	// E=0
	Delay(40);																				// Пауза 40us
	}
}





// Функция вывода n-символов в позицию pos с ячейки памяти  *addr
 void PutChar (uint8_t *addr,char pos,char n){
	char i;
	
		
	// Устанавливаем курсор в позицию pos
	GPIOC->BSRR =GPIO_BSRR_BR10;				//A0=0 RW=0 
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos |0x80)<<4;																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos<<8);																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	Delay(40);																				// Пауза 40us
	// Выводим символ с адреса addr в позицию курсора
	for (i=0; i<n; i++)
			{
			GPIOC->BSRR |=GPIO_BSRR_BS10;			// A0=1 RW=0 
			GPIOC->BSRR |=GPIO_BSRR_BS11;											// E=1
			GPIOB->ODR =*addr<<4;																				
			GPIOC->BSRR |=GPIO_BSRR_BR11;										 	// E=0											
		
			GPIOC->BSRR |=GPIO_BSRR_BS11;											// E=1
			GPIOB->ODR =*addr<<8;
			GPIOC->BSRR |=GPIO_BSRR_BR11;										 	// E=0
			Delay(40);																				// Пауза 40us
			addr++;
			}
	
}

//
void PutSimvol (uint8_t simvol ,char pos){
		
	// Устанавливаем курсор в позицию pos
	GPIOC->BSRR =GPIO_BSRR_BR10;				// A0=0 RW=0
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos |0x80)<<4;																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->ODR =(pos<<8);																								
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						
	Delay(40);																				// Пауза 40us
	// Выводим символ simvol в позицию курсора
			GPIOC->BSRR |=GPIO_BSRR_BS10;			// A0=1 RW=0
			GPIOC->BSRR |=GPIO_BSRR_BS11;											// E=1
			GPIOB->ODR =simvol<<4;																				
			GPIOC->BSRR |=GPIO_BSRR_BR11;										 	// E=0											
			
	    GPIOC->BSRR |=GPIO_BSRR_BS11;											// E=1
			GPIOB->ODR =simvol<<8;
			GPIOC->BSRR |=GPIO_BSRR_BR11;										 	// E=0
			Delay(40);																				// Пауза 40us
			
}



//Функция вывода символа в текущую позицию курсора с ячейки памяти  *addr
 void PutChar_Curent (uint8_t *addr){
			
	// Выводим символ с адреса addr в позицию курсора
			{
			GPIOC->BSRR =GPIO_BSRR_BS10;				// A0=1 
			GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
			GPIOB->ODR =*addr<<4;																				
			GPIOC->BSRR =GPIO_BSRR_BR11;										 	// E=0											
			
			GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
			GPIOB->ODR =*addr<<8;
			GPIOC->BSRR =GPIO_BSRR_BR11;										 	// E=0
			Delay(40);																				// Пауза 40us
			}	
}



// Функция изменения типа отображения курсора
void Cursor_type(uint32_t type){
	
	GPIOC->BSRR =GPIO_BSRR_BR10;											// A0=0 
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1
	GPIOB->BSRR =0x0F000000;
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0						// A0 R/W DB7 DB6 DB5 DB4
	
	GPIOC->BSRR =GPIO_BSRR_BS11;											// E=1						// 0  0   0   0   0   0
	GPIOB->BSRR =0x00000F00 &~type;																			// 0  0   1   1   1(0)   1(0)
	GPIOC->BSRR =GPIO_BSRR_BR11;											// E=0
	Delay(40);
}

//
void Sleepdeep (void) {

			ClearLCD();
			PutText (GoodBye,0x5);
			Delay(1000000);
			GPIOB->BSRR=GPIO_BSRR_BR8;
			GPIOC->BSRR=GPIO_BSRR_BR10|
									GPIO_BSRR_BS6;
			EXTI->IMR&=~EXTI_IMR_MR2&~EXTI_IMR_MR3;
			__WFE();
			GPIOC->BSRR =GPIO_BSRR_BR6;
			LCDInit();
			EXTI->IMR|=EXTI_IMR_MR1|EXTI_IMR_MR2|EXTI_IMR_MR3;
			RTC->CRH |= RTC_CRH_SECIE;
}
