#include <stm32f10x.h>
#include "CAN.h"
#include "header.h"

CANTX_TypeDef CAN_Data_TX;
CANRX_TypeDef CAN_Data_RX[2];

//extern TimeAlarm_TypeDef  Time,Alarm;
extern Time_Type 								Time;
extern Alarm_Type 							Alarm;

#define NETNAME_INDEX  03   //32VLDisc 

/****************************************************************************************************************
*														bxCAN_Init
****************************************************************************************************************/
void bxCAN_Init(void){

	GPIO_InitTypeDef GPIO_InitStruct;
		
	
	/*Включаем тактирование CAN в модуле RCC*/	
	RCC->APB1ENR|=RCC_APB1ENR_CAN1EN;
	/*Настройка выводов CAN  CAN1_TX=PA12   CAN1_RX=PA11  */
		
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	CAN1->RF1R|=CAN_RF0R_RFOM0;
	CAN1->RF1R|=CAN_RF1R_RFOM1;
	
	/*Настройка NVIC для bxCAN interrupt*/
	NVIC_SetPriority( USB_LP_CAN1_RX0_IRQn,1);
	NVIC_SetPriority(CAN1_RX1_IRQn, 1);
	

//			Init mode				//

	//CAN1->MCR|=CAN_MCR_RESET;
	
	/*Exit SLEEP mode*/
	CAN1->MCR&=~CAN_MCR_SLEEP;
	/*Enter Init mode bxCAN*/
	CAN1->MCR|=CAN_MCR_INRQ;  /*Initialization Request */
	while((CAN1->MSR&CAN_MSR_INAK)!=CAN_MSR_INAK)		{}   /*while Initialization Acknowledge*/

	CAN1->MCR|=0x00010000;//CAN_MCR_DBF;	 CAN работает в режиме отладки//CAN останавливается в режиме отладки
	CAN1->MCR&=~CAN_MCR_ABOM;
	CAN1->MCR&=~CAN_MCR_TTCM;
	CAN1->MCR&=~CAN_MCR_AWUM;
	CAN1->MCR&=~CAN_MCR_NART;	
	CAN1->MCR&=~CAN_MCR_RFLM;
	CAN1->MCR&=~CAN_MCR_TXFP;	
	
	/*Тестовый режиим работы выключен CAN  SILM=0  LBKM=0 */
	CAN1->BTR&=~CAN_BTR_LBKM;	
	CAN1->BTR&=~CAN_BTR_SILM;	

	CAN1->BTR|=CAN_BTR_BRP&35;																	/* tq=(35+1)*tPCLK1=1uS  */
	CAN1->BTR|=0x01000000;	//CAN_BTR_SJW_0;											/*SJW[1:0]=1  (SJW[1:0]+1)*tq=tRJW	*/		
	
	CAN1->BTR&=~((uint32_t)0x00010000);
	CAN1->BTR|=0x00060000;	//CAN_BTR_TS1_2;											/* TS1[3:0]=0X06  tBS1=1*(6+1)=7uS */
	
	CAN1->BTR&=~((uint32_t)0x00200000);
	CAN1->BTR|=0x00100000;	//CAN_BTR_TS2_0|CAN_BTR_TS2_1;				/* TS2[2:0]=0X01  tBS2=1*(1+1)=2uS */
																																// | 1uS |  		7uS  |  	2uS| 		T=10uS f=100kHz
																																// |-----------------|-------|		
																																// 								Sample point = 80%	
	/*Init filters*/
	
	CAN1->FMR|=	CAN_FMR_FINIT;																		// Filter Init Mode
	CAN1->FM1R|=CAN_FM1R_FBM0|CAN_FM1R_FBM1|CAN_FM1R_FBM2;  				// Filters bank 0 1 2  mode ID List
	CAN1->FS1R&=~(CAN_FS1R_FSC0|CAN_FS1R_FSC1|CAN_FS1R_FSC2);				// Filters bank 0 1 2  scale 16 bits
	CAN1->FFA1R&=~(CAN_FFA1R_FFA0|CAN_FFA1R_FFA1|CAN_FFA1R_FFA2);		// Filters bank 0 1 2  FIFO0		
		
	CAN1->FM1R|=CAN_FM1R_FBM3|CAN_FM1R_FBM4|CAN_FM1R_FBM5;					// Filters bank 3 4 5  mode ID List		
	CAN1->FS1R&=~(CAN_FS1R_FSC3|CAN_FS1R_FSC4|CAN_FS1R_FSC5);				// Filters bank 3 4 5  scale 16 bits	
	CAN1->FFA1R|=CAN_FFA1R_FFA3|CAN_FFA1R_FFA4|CAN_FFA1R_FFA5;			// Filters bank 3 4 5 FIFO1		

	/*ID filters */
  //FOFO0
	CAN1->sFilterRegister[0].FR1=0x10109000;	//Filters bank 0 fmi 00 ID=0x480 IDE=0 RTR=0	// 
																						//							 fmi 01 ID=0x080 IDE=0 RTR=1	// GET_RTC(remote) 
	CAN1->sFilterRegister[0].FR2=0x10509020;	//Filters bank 0 fmi 02 ID=0x481 IDE=0 RTR=0	// 
																						//							 fmi 03 ID=0x082 IDE=0 RTR=1	// GET_TIMER_DATA(remote)
	CAN1->sFilterRegister[1].FR1=0x90509040;	//Filters bank 1 fmi 04 ID=0x482 IDE=0 RTR=0	// 
																						//							 fmi 05 ID=0x482 IDE=0 RTR=1	// ENABLE_TIMER
	CAN1->sFilterRegister[1].FR2=0x90709060;	//Filters bank 1 fmi 06 ID=0x483 IDE=0 RTR=0	// SET_TIMER_DATA
																						//							 fmi 07 ID=0x483 IDE=0 RTR=1	// DISABLE_TIMER
	CAN1->sFilterRegister[2].FR1=0x90909080;	//Filters bank 2 fmi 08 ID=0x484 IDE=0 RTR=0	// GET_ALARM_A
																						//							 fmi 09 ID=0x484 IDE=0 RTR=1	// ENABLE ALARM_A	
	CAN1->sFilterRegister[2].FR2=0x90B090A0;	//Filters bank 2 fmi 10 ID=0x485 IDE=0 RTR=0	// SET_ALARM_A
																						//							 fmi 11 ID=0x485 IDE=0 RTR=1	// DISABLE ALARM_A				
	//FIFO1  
	CAN1->sFilterRegister[3].FR1=0x90D090C0;	//Filters bank 3 fmi 00 ID=0x486 IDE=0 RTR=0	// GET_ALARM_B
																						//							 fmi 01 ID=0x486 IDE=0 RTR=1	// ENABLE ALARM_B
	CAN1->sFilterRegister[3].FR2=0x90F090E0;	//Filters bank 3 fmi 02 ID=0x487 IDE=0 RTR=0	// SET_ALARM_B
																						//							 fmi 03 ID=0x487 IDE=0 RTR=1	// DISABLE ALARM_B
	CAN1->sFilterRegister[4].FR1=0x91109100;	//Filters bank 4 fmi 04 ID=0x488 IDE=0 RTR=0	 
																						//							 fmi 05 ID=0x488 IDE=0 RTR=1
	CAN1->sFilterRegister[4].FR2=0x91309120;	//Filters bank 4 fmi 06 ID=0x489 IDE=0 RTR=0	 
																						//							 fmi 07 ID=0x489 IDE=0 RTR=1
	
	CAN1->sFilterRegister[5].FR1=0x10F010E0;	//Filters bank 5 fmi 08 ID=0x087 IDE=0 RTR=0	 
																						//							 fmi 09 ID=0x087 IDE=0 RTR=1	
	CAN1->sFilterRegister[5].FR2=0x11101100;	//Filters bank 5 fmi 10 ID=0x088 IDE=0 RTR=0	//  
																						//							 fmi 11 ID=0x088 IDE=0 RTR=1	// 	GET_NET_NAME																			
	
	/* Filters activation  */	
	CAN1->FA1R|=CAN_FFA1R_FFA0|CAN_FFA1R_FFA1|CAN_FFA1R_FFA2|
							CAN_FFA1R_FFA3|CAN_FFA1R_FFA4|CAN_FFA1R_FFA5;		//
							
	/*Exit filters init mode*/
	CAN1->FMR&=	~CAN_FMR_FINIT;
	
	/*Разрешение прерываний FIFO0 FIFO1*/
	CAN1->IER|=CAN_IER_FMPIE0|CAN_IER_FMPIE1;

//	 Exit Init mode bxCAN	

	CAN1->MCR&=~CAN_MCR_INRQ;  														/*Initialization Request */	
	while((CAN1->MSR&CAN_MSR_INAK)==CAN_MSR_INAK)		{}   /*while Initialization Acknowledge*/		

	
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	NVIC_EnableIRQ(CAN1_RX1_IRQn);		
}
/*****************************************************************************************************************
*													CAN_Transmit_DataFrame
******************************************************************************************************************/
CAN_TXSTATUS CAN_Transmit_DataFrame(CANTX_TypeDef *Data){
		uint32_t temp=0;
		uint8_t mailbox_index;
	
	if((CAN1->TSR&CAN_TSR_TME0)==CAN_TSR_TME0)
		mailbox_index=0;
	else if((CAN1->TSR&CAN_TSR_TME1)==CAN_TSR_TME1)
		mailbox_index=1;
	else if((CAN1->TSR&CAN_TSR_TME2)==CAN_TSR_TME2)
		mailbox_index=2;
	else
		return CAN_TXBUSY;
	

	CAN1->sTxMailBox[mailbox_index].TIR=(uint32_t)(Data->ID<<21);//&0xffe00000);
	
	CAN1->sTxMailBox[mailbox_index].TDTR&=(uint32_t)0xfffffff0;
	CAN1->sTxMailBox[mailbox_index].TDTR|=Data->DLC;
	
	temp=(Data->Data[3]<<24)|(Data->Data[2]<<16)|(Data->Data[1]<<8)|(Data->Data[0]);
	CAN1->sTxMailBox[mailbox_index].TDLR=temp;
	temp=(Data->Data[7]<<24)|(Data->Data[6]<<16)|(Data->Data[5]<<8)|(Data->Data[4]);
	CAN1->sTxMailBox[mailbox_index].TDHR=temp;
	
	/*Send message*/
	CAN1->sTxMailBox[mailbox_index].TIR|=CAN_TI0R_TXRQ;
	return CAN_TXOK;
		
}	
/*****************************************************************************************************************
*													CAN_Transmit_RemoteFrame
******************************************************************************************************************/

CAN_TXSTATUS CAN_Transmit_RemoteFrame(uint16_t ID){
	
	uint8_t mailbox_index;
	
	if((CAN1->TSR&CAN_TSR_TME0)==CAN_TSR_TME0)
		mailbox_index=0;
	else if((CAN1->TSR&CAN_TSR_TME1)==CAN_TSR_TME1)
		mailbox_index=1;
	else if((CAN1->TSR&CAN_TSR_TME2)==CAN_TSR_TME2)
		mailbox_index=2;
	else
		return CAN_TXBUSY;
	
	CAN1->sTxMailBox[mailbox_index].TIR=(uint32_t)((ID<<21)|0x2);
	CAN1->sTxMailBox[mailbox_index].TDTR&=(uint32_t)0xfffffff0;
	
	/*Send message*/
	CAN1->sTxMailBox[mailbox_index].TIR|=CAN_TI0R_TXRQ;
	return CAN_TXOK;
}
/*****************************************************************************************************************
*													CAN_Receive
******************************************************************************************************************/
void CAN_Receive_IRQHandler(uint8_t FIFONumber){
	
	
	if((CAN1->sFIFOMailBox[FIFONumber].RIR&CAN_RI0R_RTR)!=CAN_RI0R_RTR)
	{
		
		CAN_Data_RX[FIFONumber].Data[0]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR);
		CAN_Data_RX[FIFONumber].Data[1]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR>>8);
		CAN_Data_RX[FIFONumber].Data[2]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR>>16);
		CAN_Data_RX[FIFONumber].Data[3]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR>>24);
		
		CAN_Data_RX[FIFONumber].Data[4]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR);
		CAN_Data_RX[FIFONumber].Data[5]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR>>8);
		CAN_Data_RX[FIFONumber].Data[6]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR>>16);
		CAN_Data_RX[FIFONumber].Data[7]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR>>24);
		
		CAN_Data_RX[FIFONumber].DLC=(uint8_t)0x0F & CAN1->sFIFOMailBox[FIFONumber].RDTR;
		
	}
		CAN_Data_RX[FIFONumber].ID= (uint16_t)0x7FF & (CAN1->sFIFOMailBox[FIFONumber].RIR>>21);
		CAN_Data_RX[FIFONumber].FMI=(uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDTR>>8);
	
	/*Запрет прерываний FIFO0 FIFO1 на время обработки сообщения*/
		if(FIFONumber)
			CAN1->IER&=~CAN_IER_FMPIE1;	
		else
			CAN1->IER&=~CAN_IER_FMPIE0;
	/*Release FIFO*/
	
	if(FIFONumber)
		CAN1->RF1R|=CAN_RF1R_RFOM1;
	else	
		CAN1->RF0R|=CAN_RF0R_RFOM0;

}

/*****************************************************************************************************************
*													CAN_RXProcess0
******************************************************************************************************************/
void CAN_RXProcess0(void){
	int temp;
	switch(CAN_Data_RX[0].FMI) {
		case 1://(id=080 remote GET_RTC)
		//
		
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x80;  
		CAN_Data_TX.DLC=6;
		CAN_Data_TX.Data[0]=NETNAME_INDEX; //netname_index для 32VLDisc
		CAN_Data_TX.Data[1]=((Time.time[0]&0x0F)<<4)|(Time.time[1]&0x0F);//  Time.hour;
		CAN_Data_TX.Data[2]=((Time.time[3]&0x0F)<<4)|(Time.time[4]&0x0F);// Time.min;
		CAN_Data_TX.Data[3]=((Time.date[0]&0x0F)<<4)|(Time.date[1]&0x0F);// Time.day;
		CAN_Data_TX.Data[4]=((Time.date[3]&0x0F)<<4)|(Time.date[4]&0x0F);// Time.month;
		CAN_Data_TX.Data[5]=((Time.date[8]&0x0F)<<4)|(Time.date[9]&0x0F);// Time.year;
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;
		
		case 2://(id=481 data set_rtc)
		//
		/*Time.hour=CAN_Data_RX[0].Data[1];
		Time.min=CAN_Data_RX[0].Data[2];
		//RTC_SetTime(RTC_Format_BIN, &RTC_Time);
		Time.day=CAN_Data_RX[0].Data[3];
		Time.month=CAN_Data_RX[0].Data[4];
		Time.year=CAN_Data_RX[0].Data[5];
		//RTC_SetDate(RTC_Format_BIN, &RTC_Date);*/
		break;
		
		case 3://(id=082 GET_TIMER_DATA remote )
		//
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x82;  
		CAN_Data_TX.Data[0]=NETNAME_INDEX; //netname_index для 32VLDisc
		CAN_Data_TX.Data[1]=(uint8_t)PhaseBrez;
		CAN_Data_TX.Data[2]=(uint8_t)PhasePower;
		CAN_Data_TX.Data[3]=(uint8_t)BrezPower;
		CAN_Data_TX.Data[4]=(uint8_t)TimerONOFF;
		CAN_Data_TX.Data[7]=(uint8_t)(CAN1->ESR>>24);		//REC
		CAN_Data_TX.Data[6]=(uint8_t)(CAN1->ESR>>16);		//TEC
		CAN_Data_TX.Data[5]=(uint8_t)(CAN1->ESR);				//ERF
		CAN_Data_TX.DLC=8;
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;
		case 5://(id=482 remote enable timer )
		//
			TIM2->CCER |=TIM_CCER_CC2E;												// Enable канал сравнения CC2
			TIM2->CCER|=TIM_CCER_CC1E;												// Enable capture CC1
			TimerONOFF=1;
		
		break;
		case 6://(id=483 data set_timer)
		//
			PhaseBrez=CAN_Data_RX[0].Data[1];
			if(PhaseBrez)
			{
				TIM2->CCMR1 |=TIM_CCMR1_IC1PSC_0; // Канал захвата настроим - прерывание на каждый второй спад импульса
				TIM2->DIER |=TIM_DIER_CC1IE;			//Если меняем режим на Брезенхема то для TIM2 по Capture/Compare 1 interrupt enable
				TIM2->ARR=35;
				TIM2->CCR2=5;
				BrezPower=CAN_Data_RX[0].Data[3];
				temp=BrezPower/2;
				BrezKoeff=temp/50.0f;
			}
			else
			{
				PhasePower=CAN_Data_RX[0].Data[2];
				TIM2->CCMR1 &=~TIM_CCMR1_IC1PSC_0;	// Канал захвата настроим на каждый спад импульса
				TIM2->DIER &=~TIM_DIER_CC1IE;				//Если меняем режим на Фазовый то для TIM2 по Capture/Compare 1 interrupt disable
				TIM2->ARR=1000-PhasePower*10;
				TIM2->CCR2=970-PhasePower*10;
			}
		break;
		case 7://(id=483 remote disable_timer)
		//
			TIM2->CCER &=~TIM_CCER_CC2E;											// Disable канал сравнения CC2
			TIM2->CCER&=~TIM_CCER_CC1E;												//Disable capture CC1
			Brez_Count=0;
			BrezErr=0;
			TimerONOFF=0;
		break;
		case 8://(id=384 data get alarm_a)
		//
		
		break;
		case 10://(id=385 data set alarm_a)
		//
		
		
		break;
		case 11://(id=385 remote disable alarm_a)
		//
		
		
		break;
		default:
		break;	
	
	}

	/*Разрешение прерываний FIFO0 */
	CAN1->IER|=CAN_IER_FMPIE0;
	//new_message=0;

}	

/*
*
*/
/*****************************************************************************************************************
*													CAN_RXProcess1
******************************************************************************************************************/

void CAN_RXProcess1(void){

	switch(CAN_Data_RX[1].FMI) {
		case 0://(id=486 data get alarm_b)
		//
		break;
		case 1://(id=486 remote enable alarm_b)
		//
		break;
		case 2://(id=487 data set alarm_b)
		//
		break;
		case 3://(id=487 remote disable alarm_b)
		//
		break;
		case 11://(id=088 remote get net name)
		//
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x88;
		CAN_Data_TX.DLC=1;
		CAN_Data_TX.Data[0]=NETNAME_INDEX;  // // netname_index для 32VLDisc
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;
		default:
		break;
	}
	/*Разрешение прерываний FIFO1*/
	CAN1->IER|=CAN_IER_FMPIE1;
}

