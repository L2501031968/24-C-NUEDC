#ifndef _AD9959_H_
#define _AD9959_H_
#include "sys.h"
#include "stdint.h"

//AD9959 ???  
#define CS			PAout(6)
#define SCLK		PBout(1)
#define UPDATE	PBout(0)

#define PS0			PAout(7)
#define PS1			PAout(2)
#define PS2			PBout(10)
#define PS3			PCout(0)

// SDIO          ӳ 䣬 ͷ PA4/PA5    DAC
#define SDIO_0   PBout(13) //PB13
#define SDIO_1   PBout(12) // PB12
#define SDIO_2   PAout(3)  // PA3
#define SDIO_3   PAout(8)  // PA8

#define AD9959_PWR	PAout(9)
#define Reset		PAout(10)

//AD9959 ?     ?    
#define CSR_ADD   0x00   //CSR ?  ?  ?   
#define FR1_ADD   0x01   //FR1    ??   1
#define FR2_ADD   0x02   //FR2    ??   2
#define CFR_ADD   0x03   //CFR ?     ??   

#define CFTW0_ADD 0x04   //CTW0 ?  ?  ?   ??   
#define CPOW0_ADD 0x05   //CPW0 ?      ?   ??   
#define ACR_ADD   0x06   //ACR    ?  ??   

#define LSRR_ADD  0x07   //LSR     ?     ??   
#define RDW_ADD   0x08   //RDW     ?       ?   
#define FDW_ADD   0x09   //FDW  ? ?       ?   

#define PROFILE_ADDR_BASE   0x0A   //Profile ?   ,     ?  ?     ?  ?

//CSR[7:4]?  ?        
#define CH0 0x10
#define CH1 0x20
#define CH2 0x40
#define CH3 0x80

//FR1[9:8]    ? ??    
#define LEVEL_MOD_2  	0x00//2  ?     2 ?   
#define LEVEL_MOD_4		0x01//4  ?    	4 ?   
#define LEVEL_MOD_8		0x02//8  ?    	8 ?   
#define LEVEL_MOD_16	0x03//16  ?    	16 ?   

//CFR[23:22]    ?      AFP  ?    
#define	DISABLE_Mod		0x00	//00	     ?   
#define	ASK 					0x40	//01	      ?    ?   
#define	FSK 					0x80	//10	? ?  ? ? ?   
#define	PSK 					0xc0	//11	       ?    ?   

//  CFR[14]      ?       sweep enable																				
#define	SWEEP_ENABLE	0x40	//1	    
#define	SWEEP_DISABLE	0x00	//0	      
		
void delay1 (uint32_t length);//  ?
void IntReset(void);	 			//AD9959    
void IO_Update(void); 		  //AD9959        
void Intserve(void);				//IO ? ???  ?  
void AD9959_Init(void);			//IO ? ?  

/***********************AD9959     ?           *****************************************/
void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData);//  AD9959      
void Write_CFTW0(uint32_t fre);										//  CFTW0?  ?  ?   ??   
void Write_ACR(uint16_t Ampli);										//  ACR?      ?   ??   
void Write_CPOW0(uint16_t Phase);									//  CPOW0?      ?   ??   

void Write_LSRR(uint8_t rsrr,uint8_t fsrr);				//  LSRR    ?     ??   
void Write_RDW(uint32_t r_delta);									//  RDW         ?   
void Write_FDW(uint32_t f_delta);									//  FDW ?      ?   

void Write_Profile_Fre(uint8_t profile,uint32_t data);//  Profile ?   ,?  
void Write_Profile_Ampli(uint8_t profile,uint16_t data);//  Profile ?   ,    
void Write_Profile_Phase(uint8_t profile,uint16_t data);//  Profile ?   ,    
/********************************************************************************************/


/*****************************  ?        ***********************************/
void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq); //  ?  
void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli);//      
void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase);//      
/****************************************************************************/

/*****************************   ?         ***********************************/
void AD9959_Modulation_Init(uint8_t Channel,uint8_t Modulation,uint8_t Sweep_en,uint8_t Nlevel);//   ?   ?   ?   ??  
void AD9959_SetFSK(uint8_t Channel, uint32_t *data,uint16_t Phase);//    FSK   ??   
void AD9959_SetASK(uint8_t Channel, uint16_t *data,uint32_t fre,uint16_t Phase);//    ASK   ??   
void AD9959_SetPSK(uint8_t Channel, uint16_t *data,uint32_t Freq);//    PSK   ??   

void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data,uint32_t e_data,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint16_t Ampli,uint16_t Phase);//        ?? ?   
void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli,uint16_t e_Ampli,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Phase);//        ?   ?   
void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data,uint16_t e_data,uint16_t r_delta,uint16_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Ampli);//        ?  ?   
/********************************************************************************************/

#endif








