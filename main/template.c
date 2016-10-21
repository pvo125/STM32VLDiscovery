#include <stm32f10x.h>
#include "header.h"


void assert_failed(uint8_t* file, uint32_t line)
{
	while(1){};
}


/*----------------------------------------------Буфер RX SPI------------------------------------------------------------ */

/*uint8_t DummyByte=0;
uint32_t MX25L_ID;
uint8_t SPIBufferTX[100];
uint8_t SPIBufferRX[100];
uint8_t Status_Reg;*/

/*-----------------------------------------------	Переменные-------------------------------------------------------------*/

uint8_t Temperature[4];

uint8_t Error_LSE=0;			/*Признак не запуска кварца LSE (начальное значение =0)  */

uint8_t Enable_Sleep;			/*Флаг перехода в режим пониженного потребления */

uint8_t TimerONOFF;				/*Флаг вкл -выкл таймера*/
uint8_t PhaseBrez=0;		/*Режим работы таймера фазовый-брезенхема(начальное значение(0) -phase) */
uint8_t PhasePower=0;		/*Значение мощности (phase )при работе таймера. Начальное значение=0  */
uint8_t BrezPower=0;		/*Значение мощности (Brez )при работе таймера. Начальное значение=0  */
float BrezErr=0;
float BrezKoeff=0;

uint8_t Brez_Count;				/*Счетчик периодов при работе таймера в режиме брезенхема */
uint8_t Punkt_menu=0;
/*------------------------------------------------- АЦП --------------------------------------------------------------*/

uint16_t ADC1Buff[4];			/*Буфер из 4 двухбайтных слов для складывания значений из ADC->DR для каждого канала ацп */
uint8_t ADC_Ch_0[5];			/*Ячейка в виде 5-ти байтного буфера для результата напряжения на канале ацп_0 */
uint8_t ADC_Ch_16[5];			/*Ячейка в виде 5-ти байтного буфера для результата напряжения на канале ацп_17 (Т датчик)*/
uint8_t ADC_Ch_15[5];			/*Ячейка в виде 5-ти байтного буфера для результата напряжения на канале ацп_15 */
uint8_t ADC_Ch_8[5];			/*Ячейка в виде 5-ти байтного буфера для результата напряжения на канале ацп_8 */

uint8_t *ADC_Ch[]={ADC_Ch_0,ADC_Ch_16,ADC_Ch_15,ADC_Ch_8};	/*Массив указателей на ячейки результатов с каждого канала АЦП */
/*-------------------------------------------------	Меню ------------------------------------------------------------------*/

uint8_t Set_Time []="Set time";
uint8_t Set_Alarm []="Set alarm";
uint8_t Method_Control []="TIM ctrl";
//uint8_t Sleep []="Sleep";
uint8_t ADC_View []="ADC View"; 
//uint8_t Flash_menu[]="Flash menu";
uint8_t *Menu[]={Set_Time,Set_Alarm,Method_Control,ADC_View};			/*Массив указателей на пункты меню	*/
//uint8_t *Menu_flash[]={Read_Flash,PagePrg,ReadID,Read_StatusReg};
/*-------------------------------------------------	Текст -------------------------------------------------------------------*/

uint8_t Vin []="Vin = ";
uint8_t Tchip []="Tchip = ";
uint8_t OSC32_Failed []="OSC32 Failed";
uint8_t Power []="Power";
uint8_t Method_Control1 []="Method Phase";
uint8_t Method_Control2 []="       Brezen";
uint8_t GoodBye []="GoodBye!";
uint8_t Enable_Method []="Turn on Timer";
uint8_t Phase_Method_Enabled []="Method Enabled";
uint8_t Phase_Method_Disabled []="Method Disabled";
//uint8_t ReadID[]="Read ID";
//uint8_t PagePrg[]="Prog page";
//uint8_t Read_Flash[]="Read flash";
//uint8_t Read_StatusReg[]="Read status";
//uint8_t Sector_Erase[]="Sector erase";




/*--------------------------------------- Массив дней для високосного и не високосного года----------------------------------*/

uint8_t days [2][13]={
	{0,31,28,31,30,31,30,31,31,30,31,30,31},
	{0,31,29,31,30,31,30,31,31,30,31,30,31}
	};		
