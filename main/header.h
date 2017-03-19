#ifndef _HEADER_H_
#define _HEADER_H_
#include <stm32f10x.h>

#define C_NONE	0x0C000300
#define C_			0x0E000100
#define C_BLINK	0x00000000
#define Left  	0xFFFFFFFF
#define Right		0x04000B00

typedef enum {
		BUTTON_DISABLE=0,
		BUTTON_SLEEP,
		BUTTON_EXIT,
		BUTTON_SET,
}BUTTON_TypeDef;

typedef struct{
  	uint8_t CNT100;
		uint8_t CNT10;
		uint8_t CNT1;
		uint8_t persent;
} Count_Type;

typedef struct	
{	
	uint8_t time[8];
	uint8_t date[10];
} Time_Type;

typedef struct 
{	
	uint8_t time[8];
	uint8_t date[10];
} Alarm_Type;

extern uint8_t DummyByte;
extern uint32_t MX25L_ID;
extern uint8_t SPIBufferTX[];
extern uint8_t SPIBufferRX[];

extern uint8_t Status_Reg;

extern  uint8_t ADC_Ch_0[];
extern  uint8_t ADC_Ch_16[];
extern  uint8_t ADC_Ch_15[];
extern  uint8_t ADC_Ch_8[];

extern  uint8_t *ADC_Ch[];
extern  uint16_t ADC1Buff[];

extern  uint8_t *Menu [];
extern uint8_t *Menu_flash[];
extern  uint8_t Temperature[];
extern  uint8_t Error_LSE;
extern  uint8_t Enable_Sleep;

extern  uint8_t TimerONOFF;
extern 	uint8_t PhaseBrez;
extern  uint8_t PhasePower;
extern  uint8_t BrezPower;
extern float BrezErr;
extern float BrezKoeff;

extern  uint8_t Brez_Count;
extern  uint8_t Punkt_menu;

extern  uint8_t Vin [];
extern  uint8_t Tchip [];
extern  uint8_t OSC32_Failed [];
extern  uint8_t Set_Time [];
extern  uint8_t Set_Alarm [];
extern  uint8_t GoodBye [];
extern  uint8_t Power [];
extern  uint8_t Sleep [];
extern  uint8_t Method_Control [];
extern  uint8_t ADC_View [];
extern uint8_t Flash_menu[];
extern uint8_t ReadID[];
extern uint8_t PagePrg[];
extern uint8_t Read_Flash[];
extern uint8_t Read_StatusReg[];
extern uint8_t Sector_Erase[];




extern  uint8_t Method_Control1 [];
extern  uint8_t Method_Control2 [];
extern  uint8_t Enable_Method [];
extern  uint8_t Phase_Method_Enabled [];
extern  uint8_t Phase_Method_Disabled [];
extern  uint8_t days [2][13];
extern 	uint8_t MX25L[];

 void SetTime_encoder(void);
 void SetAlarm_encoder(void);
 void BrezPhase(void);
 
 void _Menu(void);
 void Refresh_LCD(uint8_t cmd);
 void assert_failed(uint8_t* file, uint32_t line);
 void DateCalc(void);
 void AlarmCalc(void);
 void LCDInit(void);
 void ClearLCD(void);
 void DisplayOFF(void);
 void Delay (uint32_t c);
 void Set_Cursor (uint8_t pos); 
 void PutText(uint8_t simvol[],char pos);
 void PutChar (uint8_t *addr,char pos,char n);
 void PutSimvol (uint8_t simvol ,char pos);
 void Backspace_cursor(uint32_t shift,uint8_t n);
 void PutChar_Curent (uint8_t *addr);
 void Cursor_type (uint32_t type);
 void Shift_cursor (uint32_t shift);
 void Sleepdeep (void);
 void ADC(void);
 void Flash (void);

#endif
