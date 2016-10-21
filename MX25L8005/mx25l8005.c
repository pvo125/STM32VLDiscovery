
#include "header.h"
#include "mx25l8005.h"

/*---------------------------------------------------------------------------------------------------*/
void WriteDisable(void){
	CS_LOW();
	SPI1->DR=WRDI;			// WRDI= 0x04
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)==SET){}	
	CS_HIGH();
}

/*---------------------------------------------------------------------------------------------------*/
void WriteEnable(void){
	CS_LOW();
	SPI1->DR=WREN;			//WREN=0x06
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)==SET){}	
	CS_HIGH();
}

/*---------------------------------------------------------------------------------------------------*/
void WriteStatusReg_MX25L(uint8_t status){
	WriteEnable();
	CS_LOW();
	SPI1->DR=WRSR;
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET)	{}
	SPI1->DR=status;
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)==SET)	{}	
	CS_HIGH();
	
		/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}
	
		
		
}

/*---------------------------------------------------------------------------------------------------*/
uint8_t ReadStatusReg_MX25L(void){
	uint8_t temp;
	CS_LOW();
	SPI1->DR=RDSR;		//	RDSR= 0x05	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)==SET)	{}
	temp=SPI1->DR;			// Clear RXNE 
	SPI1->DR=0;
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET)	{}	
	temp=SPI1->DR;	
	CS_HIGH();
	return temp;
	
}

/*---------------------------------------------------------------------------------------------------*/
uint32_t ReadID_MX25L (void){
	uint32_t tmp=0;	
	uint8_t i;
	CS_LOW();
		SPI1->DR=RDID;
		while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
		i=SPI1->DR;			// Clear RXNE
	for (i=0;i<3;i++)
			{	
			SPI1->DR=0;
			while((SPI1->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{}
			tmp|=SPI1->DR<<(16-8*i);
			}
	CS_HIGH();		
			return tmp;
}

/*---------------------------------------------------------------------------------------------------*/
void Read_MX25L(uint32_t address,uint16_t bytes,uint8_t *destination){
	int8_t i;
	uint16_t m;
	CS_LOW();
	SPI1->DR=READ;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
/*после отправки команды и адреса в приемнике мусор очищаем RXNE перед циклом приема полезных байт*/			
	i=SPI1->DR;			//  Clear RXNE	
	for(m=0;m<bytes;m++)
			{
			SPI1->DR=0;
			while((SPI1->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{}
			destination[m]=SPI1->DR;
			}
	CS_HIGH();		
}

/*---------------------------------------------------------------------------------------------------*/
void FastRead_MX25L(uint32_t address,uint16_t bytes,uint8_t *destination){
	int8_t i;
	uint16_t m;
	CS_LOW();
	SPI1->DR=FastRead;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	SPI1->DR=0;
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
/*после отправки команды и адреса в приемнике мусор очищаем RXNE перед циклом приема полезных байт*/			
	i=SPI1->DR;			//  Clear RXNE	
	for(m=0;m<bytes;m++)
			{
			SPI1->DR=DummyByte;
			while((SPI1->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{}
			destination[m]=SPI1->DR;
			}
	CS_HIGH();		
}

/*---------------------------------------------------------------------------------------------------*/
void SectorErase_MX25L(uint32_t address){
	int8_t i;	
	WriteEnable();
	CS_LOW();
	SPI1->DR=SE;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
		{
		SPI1->DR=address>>(i*8);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
		}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}
/*---------------------------------------------------------------------------------------------------*/
void BlockErase_MX25L(uint32_t address){
	int8_t i;	
	WriteEnable();
	CS_LOW();
	SPI1->DR=BE;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}

/*---------------------------------------------------------------------------------------------------*/
void ChipErase_MX25L(void){
	WriteEnable();
	CS_LOW();
	SPI1->DR=CE;
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}
/*---------------------------------------------------------------------------------------------------*/
void PagePrg_MX25L(uint32_t address,uint8_t *SourceBuffer){
	int8_t i;
	uint16_t m;
	WriteEnable();
	CS_LOW();
	SPI1->DR=PP;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
		{
		SPI1->DR=address>>(i*8);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
		}
	for (m=0;m<256;m++)
		{
		SPI1->DR=SourceBuffer[m];
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
		}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}

/*------------------------------------------------------------------------------------------------*/
void Prg_MX25L(uint32_t address, uint32_t bytes,uint8_t *SourceBuffer){
	int8_t i;	
	uint16_t temp,k,m;
	uint32_t j=0;
	
	temp=bytes/256;
	WriteEnable();
	CS_LOW();
		for (k=0;k<=temp;k++)
				{
				WriteEnable();
				CS_LOW();
				SPI1->DR=PP;
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
				for (i=2;i>=0;i--)
					{
					SPI1->DR=address>>(i*8);
					while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
					}
				if(k<temp)
				{
					for (m=0;m<256;m++)
							{
							SPI1->DR=SourceBuffer[j];
							j++;	
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
							}
				}
				else
				{
					for (m=0;m<(bytes%256);m++)
							{
							SPI1->DR=SourceBuffer[j];
							j++;	
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
							}
				}				
				while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
				CS_HIGH();
				/*Читаем Status Registr ждем сброса бита WIP*/	
				while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}
				address+=256;
				}
}

void Read_MX25L_DMA(uint32_t address,uint16_t bytes){
	int8_t i;
	CS_LOW();
	SPI1->DR=READ;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	/*после отправки команды и адреса в приемнике мусор очищаем RXNE перед циклом приема полезных байт*/			
	SysTick->LOAD=100000;
	SysTick->VAL=0;
			
		i=SPI1->DR;			//  Clear RXNE	
	DMA1_Channel2->CNDTR=bytes;
	DMA1_Channel3->CNDTR=bytes;	
	DMA_Cmd(DMA1_Channel2,ENABLE);
	DMA_Cmd(DMA1_Channel3,ENABLE);	
		
}
