/************************************************************
                    AD9959 ��������
					AD9959--��Ƭ�� 
Ӳ������:  CS			����PA6;     
          SCLK 		����PB1;   
          UPDATE	����PB0;	  
          SP0    	����PA7;   
          SP1			����PA2;     
          SP2			����PB10; 
          SP3			����PC0;     
          SDIO0		����PA5;     
          SDIO1		����PA4;     
          SDIO2		����PA3;     
          SDIO3		����PA8;     
   AD9959_PWR(PDC)����PA9;     
          RST			����PA10;         
          GND			--GND(0V)  
//AD9959.c
//�������ӹ�����
//˵�������������Ӳ������Ӿ���Ϊ25MHZ
**************************************************************/
//(c1-c4,c9-c6),(r1-r4,a0-a3)
#include "AD9959.h"
															
uint8_t FR1_DATA[3] = {0xD0,0x00,0x00};//VCO gain control[23]=1ϵͳʱ�Ӹ���255Mhz; PLL[22:18]=10100,20��Ƶ,20*25M=500MHZ; Charge pump control = 75uA 


uint8_t FR2_DATA[2] = {0x00,0x00};	// ˫����ɨ�裬������ʼֵɨ������ֵ���ִӽ���ֵɨ����ʼֵ
//uint8_t FR2_DATA[2] = {0x80,0x00};// ������ɨ�裬������ʼֵɨ������ֵ���ִ���ʼֵɨ������ֵ���Դ�����


double ACC_FRE_FACTOR = 8.589934592;	//Ƶ������8.589934592=(2^32)/500000000 ����500M=25M*20(��Ƶ���ɱ��)

uint8_t CFR_DATA[3] = {0x00,0x03,0x02};//default Value = 0x000302	   //ͨ�����ܼĴ���

																	
																
																	
/************************************************************
** �������� ��void AD9959_Init(void)  
** �������� ����ʼ������AD9959��Ҫ�õ���IO��,���Ĵ���
** ��ڲ��� ����
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_Init(void)  
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;       
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //PA,PB,PC�˿�ʱ��ʹ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_2|GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;//��ʼ���ܽ�PA2.3.4.5.6.7.8.9.10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13;//ʼܽPB0.1.10.12.13			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//��ʼ���ܽ�PC0			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC
		
	Intserve();  //IO�ڵ�ƽ״̬��ʼ��
  IntReset();  //AD9959��λ  
	
	//��ʼ�����ܼĴ���
  AD9959_WriteData(FR1_ADD,3,FR1_DATA);//д���ܼĴ���1
  AD9959_WriteData(FR2_ADD,2,FR2_DATA);//
} 

//��ʱ
void delay1 (uint32_t length)
{
	length = length*12;
   while(length--);
}
/************************************************************
** �������� ��void Intserve(void)		   
** �������� ��IO�ڵ�ƽ״̬��ʼ��
**************************************************************/
void Intserve(void)		   
{   
		AD9959_PWR=0;
    CS = 1;
    SCLK = 0;
    UPDATE = 0;
    PS0 = 0;
    PS1 = 0;
    PS2 = 0;
    PS3 = 0;
    SDIO_0 = 0;
    SDIO_1 = 0;
    SDIO_2 = 0;
    SDIO_3 = 0;
}

/************************************************************
** �������� ��void IntReset(void)		   
** �������� ��AD9959��λ
**************************************************************/
void IntReset(void)	  
{
  Reset = 0;
	delay1(1);
	Reset = 1;
	delay1(30);
	Reset = 0;
}

/************************************************************
** �������� void IO_Update(void)  
** �������� �� AD9959��������
**************************************************************/
void IO_Update(void)  
{
	UPDATE = 0;
	delay1(2);
	UPDATE = 1;
	delay1(4);
	UPDATE = 0;
}

/************************************************************
** �������� ��void AD9959_WriteData(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData)
** �������� ��ʹ��ģ��SPI��AD9959д����
** ��ڲ��� ��RegisterAddress: �Ĵ�����ַ
						NumberofRegisters: Ҫд����ֽ���
						*RegisterData: ������ʼ��ַ
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
	uint8_t	ControlValue = 0;
	uint8_t	ValueToWrite = 0;
	uint8_t	RegisterIndex = 0;
	uint8_t	i = 0;

	ControlValue = RegisterAddress;
//д���ַ
	SCLK = 0;
	CS = 0;	 
	for(i=0; i<8; i++)
	{
		SCLK = 0;
		if(0x80 == (ControlValue & 0x80))
		SDIO_0= 1;	  
		else
		SDIO_0= 0;	  
		SCLK = 1;
		ControlValue <<= 1;
	}
	SCLK = 0;
//д������
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			SCLK = 0;
			if(0x80 == (ValueToWrite & 0x80))
			SDIO_0= 1;	  
			else
			SDIO_0= 0;	  
			SCLK = 1;
			ValueToWrite <<= 1;
		}
		SCLK = 0;		
	}	
  CS = 1;
} 

/************************************************************
** �������� ��void Write_CFTW0(uint32_t fre)
** �������� ��дCFTW0ͨ��Ƶ��ת���ּĴ���
** ��ڲ��� �� Freq:	д��Ƶ�ʣ���Χ0~200 000 000 Hz
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_CFTW0(uint32_t fre)
{
		uint8_t CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//�м����
	  uint32_t Temp;            
	  Temp=(uint32_t)fre*ACC_FRE_FACTOR;	 
	  CFTW0_DATA[3]=(uint8_t)Temp;
	  CFTW0_DATA[2]=(uint8_t)(Temp>>8);
	  CFTW0_DATA[1]=(uint8_t)(Temp>>16);
	  CFTW0_DATA[0]=(uint8_t)(Temp>>24);		
		AD9959_WriteData(CFTW0_ADD,4,CFTW0_DATA);//CTW0 address 0x04
}

/************************************************************
** �������� ��void Write_ACR(uint16_t Ampli)
** �������� ��дACRͨ������ת���ּĴ���
** ��ڲ��� ��Ampli:    �������,��Χ0~1023������ֵ0~1023��Ӧ�������0~500mVpp����
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_ACR(uint16_t Ampli)
{ 
	uint32_t A_temp=0;
	uint8_t ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
  A_temp=Ampli|0x1000;
	
	ACR_DATA[1] = (uint8_t)(A_temp>>8); //��λ����
	ACR_DATA[2] = (uint8_t)A_temp;  //��λ����
  AD9959_WriteData(ACR_ADD,3,ACR_DATA); //ACR address 0x06.CHn�趨����
}

/************************************************************
** �������� ��void Write_CPOW0(uint16_t Phase)
** �������� ��дCPOW0ͨ����λת���ּĴ���
** ��ڲ��� ��Phase:		�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_CPOW0(uint16_t Phase)
{
	uint8_t CPOW0_data[2] = {0x00,0x00};
	CPOW0_data[1]=(uint8_t)Phase;
	CPOW0_data[0]=(uint8_t)(Phase>>8);

	AD9959_WriteData(CPOW0_ADD,2,CPOW0_data);//CPOW0 address 0x05.CHn�趨��λ
}

/************************************************************
** �������� ��void Write_LSRR(uint8_t rsrr,uint8_t fsrr)
** �������� ��дLSRR����ɨ��б�ʼĴ���
** ��ڲ��� ��	rsrr:	����б��,��Χ��0~255
							fsrr:	�½�б��,��Χ��0~255
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_LSRR(uint8_t rsrr,uint8_t fsrr)
{
	uint8_t LSRR_data[2]={0x00,0x00};

	LSRR_data[1]=rsrr;	
	LSRR_data[0]=fsrr;//��8λ�½�б��

	AD9959_WriteData(LSRR_ADD,2,LSRR_data);
}

/************************************************************
** �������� ��void Write_RDW(uint32_t r_delta)
** �������� ��дRDW���������Ĵ���
** ��ڲ��� ��r_delta:��������,0-4294967295
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_RDW(uint32_t r_delta)
{
		uint8_t RDW_data[4] ={0x00,0x00,0x00,0x00};	//�м����          
 
	  RDW_data[3]=(uint8_t)r_delta;
	  RDW_data[2]=(uint8_t)(r_delta>>8);
	  RDW_data[1]=(uint8_t)(r_delta>>16);
	  RDW_data[0]=(uint8_t)(r_delta>>24);		
		AD9959_WriteData(RDW_ADD,4,RDW_data);
}

/************************************************************
** �������� ��void Write_FDW(uint32_t f_delta)
** �������� ��дFDW�½������Ĵ���
** ��ڲ��� ��f_delta:�½�����,0-4294967295
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_FDW(uint32_t f_delta)
{
		uint8_t FDW_data[4] ={0x00,0x00,0x00,0x00};	//�м����          
 
	  FDW_data[3]=(uint8_t)f_delta;
	  FDW_data[2]=(uint8_t)(f_delta>>8);
	  FDW_data[1]=(uint8_t)(f_delta>>16);
	  FDW_data[0]=(uint8_t)(f_delta>>24);		
		AD9959_WriteData(FDW_ADD,4,FDW_data);
}

/************************************************************
** �������� ��void Write_Profile_Fre(uint8_t profile,uint32_t data)
** �������� ��дProfile�Ĵ���
** ��ڲ��� ��profile:	profile��(0~14)
							data:	д��Ƶ�ʣ���Χ0~200 000 000 Hz
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_Profile_Fre(uint8_t profile,uint32_t data)
{
		uint8_t profileAddr;
		uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//�м����
	  uint32_t Temp;            
	  Temp=(uint32_t)data*ACC_FRE_FACTOR;	   //������Ƶ�����ӷ�Ϊ�ĸ��ֽ�  4.294967296=(2^32)/500000000
	  Profile_data[3]=(uint8_t)Temp;
	  Profile_data[2]=(uint8_t)(Temp>>8);
	  Profile_data[1]=(uint8_t)(Temp>>16);
	  Profile_data[0]=(uint8_t)(Temp>>24);
		profileAddr = PROFILE_ADDR_BASE + profile;
		
		AD9959_WriteData(profileAddr,4,Profile_data);
}
/************************************************************
** �������� ��void Write_Profile_Ampli(uint8_t profile,uint16_t data)
** �������� ��дProfile�Ĵ���
** ��ڲ��� ��profile:	profile��(0~14)
							data:	 д�����,��Χ0~1023��
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_Profile_Ampli(uint8_t profile,uint16_t data)
{
		uint8_t profileAddr;
		uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//�м����

		//Ϊ���ȵ���ʱ��������Ϊ[31:22]λ
	  Profile_data[1]=(uint8_t)(data<<6);//[23:22]
	  Profile_data[0]=(uint8_t)(data>>2);//[31:24]
		
		profileAddr = PROFILE_ADDR_BASE + profile;
		
		AD9959_WriteData(profileAddr,4,Profile_data);//д��32λ����
}
/************************************************************
** �������� ��void Write_Profile_Phase(uint8_t profile,uint16_t data)
** �������� ��дProfile�Ĵ���
** ��ڲ��� ��profile:	profile��(0~14)
							data:	 д����λ,��Χ��0~16383
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_Profile_Phase(uint8_t profile,uint16_t data)
{
		uint8_t profileAddr;
		uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//�м����

		//Ϊ��λ����ʱ��λ����Ϊ[31:18]λ
	  Profile_data[1]=(uint8_t)(data<<2);//[23:18]
	  Profile_data[0]=(uint8_t)(data>>6);//[31:24]
		
		profileAddr = PROFILE_ADDR_BASE + profile;
		
		AD9959_WriteData(profileAddr,4,Profile_data);//д��32λ����
}



/************************************************************
** �������� ��void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq)
** �������� ������ͨ�������Ƶ��
** ��ڲ��� ��Channel:  ���ͨ��  CH0-CH3
						 Freq:     ���Ƶ�ʣ���Χ0~200 000 000 Hz
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq)
{	 
		uint8_t CHANNEL[1] = {0x00};
		
		CHANNEL[0]=Channel;
		AD9959_WriteData(CSR_ADD,1,CHANNEL);//���ƼĴ���д��CHnͨ����ѡ��CHn
    Write_CFTW0(Freq);//���CHn�趨Ƶ��																																			 
} 

/************************************************************
** �������� ��void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli)
** �������� ������ͨ�����������
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							Ampli:    �������,��Χ0~1023������ֵ0~1023��Ӧ�������0~500mVpp����
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli)
{ 
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn
	Write_ACR(Ampli);							//	CHn�趨����
}

/************************************************************
** �������� ��void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase)
** �������� ������ͨ���������λ
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							Phase:		�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase)
{
	uint8_t CHANNEL[1] = {0x00};
	CHANNEL[0]=Channel;

	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn
	Write_CPOW0(Phase);//CHn�趨��λ
}

/************************************************************
** �������� ��void AD9959_Modulation_Init(uint8_t Channel,uint8_t Modulation,uint8_t Sweep_en,uint8_t Nlevel)
** �������� �����ø���ͨ���ĵ���ģʽ��
** ��ڲ��� �� Channel:  	���ͨ�� CH0-CH3
							Modulation:	����ģʽDISABLE_Mod��ASK��FSK��PSK
							Sweep_en:		����ɨ��ģʽ SWEEP_ENABLE���á�SWEEP_DISABLE�����ã�����ʱNlevelֻ����LEVEL_MOD_2
							Nlevel��		���Ƶ�ƽѡ�� LEVEL_MOD_2��4��8��16
** ���ڲ��� ����
** ����˵�� ���罫���Ƶ�ƽ����Ϊ2��ƽ����ʱ����Ӧ��P0-P3�����ϵĸߵ͵�ƽ�ֱ����CH0-CH3ͨ��(�����Ӧͨ�������Ļ�)
							�罫���Ƶ�ƽ����Ϊ4��ƽ����ʱ����Ӧ��P0��P1��P2,P3�����ϵĸߵ͵�ƽ�ֱ����CH0-CH1ͨ��(�����Ӧͨ�������Ļ�)
							����AD9959ֻ��P0-P3,4�����ڵ��ƿ��Ƶ����ţ���������4��ƽ����ʱ��ֻ����2��ͨ��ͬʱ����Ϊ���������
							8��ƽ��16��ƽ����ʱ��ֻ����1��ͨ��ͬʱ����Ϊ������������ʵ����ü���ƽ����������Ӧ������

**ע�⣡���������ó�4��ƽ����ʱ�����ͨ��ֻ��ѡ��CH0-1
							���ó�8,16��ƽ����ʱ�����ͨ��ֻ��ѡ��CH0
							������δ������ͨ�����ݣ����巽����ο�AD9959оƬ�ֲ�22-23ҳ������FR1[14:12]Ϊ��Ӧֵ��
**************************************************************/
void AD9959_Modulation_Init(uint8_t Channel,uint8_t Modulation,uint8_t Sweep_en,uint8_t Nlevel)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};
	uint8_t FR1_data[3];
	uint8_t FR2_data[2];
	uint8_t CFR_data[3];
	for(i=0;i<3;i++)//����Ĭ��ֵ
	{
		FR1_data[i]=FR1_DATA[i];
		CFR_data[i]=CFR_DATA[i];
	}
	FR2_data[0]=FR2_DATA[0];
	FR2_data[1]=FR2_DATA[1];
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	
	FR1_data[1]=Nlevel;
	CFR_data[0]=Modulation;
	CFR_data[1]|=Sweep_en;
	CFR_data[2]=0x00;

	if(Channel!=0)//��ͨ����ʹ��
	{
		AD9959_WriteData(FR1_ADD,3,FR1_data);//д���ܼĴ���1
		AD9959_WriteData(FR2_ADD,2,FR2_data);//д���ܼĴ���1
		AD9959_WriteData(CFR_ADD,3,CFR_data);//дͨ�����ܼĴ���
	}
}


/************************************************************
** �������� ��void AD9959_SetFSK(uint8_t Channel, uint32_t *data,uint16_t Phase)
** �������� ������FSK���ƵĲ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							*data:	����Ƶ�����ݵ���ʼ��ַ
							Phase:	�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ��FSKʱ�źŷ���Ĭ��Ϊ���
**************************************************************/
void AD9959_SetFSK(uint8_t Channel, uint32_t *data,uint16_t Phase)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	Write_CPOW0(Phase);//������λ
	
	Write_CFTW0(data[0]);
	for(i=0;i<15;i++)
		Write_Profile_Fre(i, data[i+1]);	
}

/************************************************************
** �������� ��void AD9959_SetASK(uint8_t Channel, uint32_t *data,uint32_t fre,uint16_t Phase)
** �������� ������ASK���ƵĲ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							*data: �����������ݵ���ʼ��ַ
							Freq:		���Ƶ�ʣ���Χ0~200 000 000 Hz
							Phase:	�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_SetASK(uint8_t Channel, uint16_t *data,uint32_t fre,uint16_t Phase)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	Write_CFTW0(fre);//����Ƶ��
	Write_CPOW0(Phase);//������λ
	
	Write_ACR(data[0]);
	for(i=0;i<15;i++)
		Write_Profile_Ampli(i, data[i+1]);	
}

/************************************************************
** �������� ��void AD9959_SetPSK(uint8_t Channel, uint16_t *data,uint32_t fre,uint16_t Phase)
** �������� ������PSK���ƵĲ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							*data:	������λ���ݵ���ʼ��ַ
							Freq:		���Ƶ�ʣ���Χ0~200 000 000 Hz
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_SetPSK(uint8_t Channel, uint16_t *data,uint32_t Freq)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	Write_CFTW0(Freq);
	
	Write_CPOW0(data[0]);
	for(i=0;i<15;i++)
		Write_Profile_Phase(i, data[i+1]);	
}

/************************************************************
** �������� ��void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data,uint32_t e_data,uint8_t fsrr,uint8_t rsrr,uint32_t r_delta,uint32_t f_delta,uint16_t Phase)
** �������� ����������ɨƵ�Ĳ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							s_data:	��ʼƵ�ʣ���Χ0~200 000 000 Hz
							e_data:	����Ƶ�ʣ���Χ0~200 000 000 Hz
							r_delta:��������Ƶ��,0~200 000 000Hz
							f_delta:�½�����Ƶ��,0~200 000 000Hz

							rsrr:		����б��,��Χ��1~255��ϵͳʱ��Ϊ500Mhzʱһ��������ԼΪ8ns
							fsrr:		�½�б��,��Χ��1~255
							Ampli:	�������,��Χ0~1023������ֵ0~1023��Ӧ�������0~500mVpp����
							Phase:	�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ��Ƶ����Ƶ���ͣ��ʱ�� dT = Xsrr*8 ��λns��ɨ�����=(��ʼ-����)/����
							ɨƵ��ʱ��=��ɨ��Ƶ����*dT
**************************************************************/
void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data,uint32_t e_data,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint16_t Ampli,uint16_t Phase)
{
	uint8_t CHANNEL[1]={0x00};
	uint32_t Fer_data=0;            
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	
	Write_CPOW0(Phase);//������λ
	Write_ACR(Ampli); //��������
	
	Write_LSRR(rsrr,fsrr);//б��
	
	Fer_data=(uint32_t)r_delta*ACC_FRE_FACTOR;	 //Ƶ��ת���ɿ�����
	Write_RDW(Fer_data);//��������
	
	Fer_data=(uint32_t)f_delta*ACC_FRE_FACTOR;
	Write_FDW(Fer_data);//�½�����
	
	Write_CFTW0(s_data);//��ʼƵ��
	Write_Profile_Fre(0, e_data);//����Ƶ��
}

/************************************************************
** �������� ��void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli,uint16_t e_Ampli,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Phase)
** �������� ����������ɨ���Ĳ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							s_Ampli:	��ʼ���ȣ�����ֵ0~1023��Ӧ�������0~500mVpp����
							e_Ampli:	�������ȣ�
							
							r_delta:	������������,0~1023
							f_delta:	�½���������,0~1023

							rsrr:			����б��,��Χ��1~255��ϵͳʱ��Ϊ500Mhzʱһ��������ԼΪ8ns
							fsrr:			�½�б��,��Χ��1~255

							fre:			���Ƶ�ʣ���Χ0~200 000 000 Hz
							Phase:		�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ������������ͣ��ʱ�� dT = Xsrr*8 ��λns��ɨ�����=(��ʼ-����)/����
							ɨ����ʱ��=��ɨ�������*dT
**************************************************************/
void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli,uint16_t e_Ampli,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Phase)
{
	uint8_t CHANNEL[1]={0x00};
	uint8_t ACR_data[3] = {0x00,0x00,0x00};
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	
	Write_CFTW0(fre); //����Ƶ��
	Write_CPOW0(Phase);//������λ
	
	Write_LSRR(rsrr,fsrr);//б��
	
	Write_RDW(r_delta<<22);//��������
	
	Write_FDW(f_delta<<22);//�½�����

	ACR_data[1] = (uint8_t)(s_Ampli>>8); //��λ����
	ACR_data[2] = (uint8_t)s_Ampli;  //��λ����
  AD9959_WriteData(ACR_ADD,3,ACR_data); //ACR �趨��ʼ����
					
	Write_Profile_Ampli(0, e_Ampli);//��������
}


/************************************************************
** �������� ��void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data,uint16_t e_data,uint16_t r_delta,uint16_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Ampli)
** �������� ����������ɨ��Ĳ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							s_data:	��ʼ��λ����Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
							e_data:	������λ��
							r_delta:��������,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
							f_delta:�½�����,

							rsrr:		����б��,��Χ��1~255��ϵͳʱ��Ϊ500Mhzʱһ��������ԼΪ8ns
							fsrr:		�½�б��,��Χ��1~255
							fre:		���Ƶ�ʣ���Χ0~200 000 000 Hz
							Ampli:	�������,��Χ0~1023������ֵ0~1023��Ӧ�������0~500mVpp����
** ���ڲ��� ����
** ����˵�� ��Ƶ����Ƶ���ͣ��ʱ�� dT = Xsrr*8 ��λns��ɨ�����=(��ʼ-����)/����
							ɨƵ��ʱ��=��ɨ��Ƶ����*dT
**************************************************************/
void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data,uint16_t e_data,uint16_t r_delta,uint16_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Ampli)
{
	uint8_t CHANNEL[1]={0x00};
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	
	Write_CFTW0(fre); //����Ƶ��
	Write_ACR(Ampli); //��������
	
	Write_LSRR(rsrr,fsrr);//б��
	
	Write_RDW(r_delta<<18);//��������
	
	Write_FDW(f_delta<<18);//�½�����
	
	Write_CPOW0(s_data);//��ʼ��λ
	Write_Profile_Phase(0, e_data);//������λ
}

