/**********************************************************
* @ File name -> usart.c
* @ Version   -> V1.0
* @ Date      -> 12-26-2013
* @ Brief     -> ϵͳ����������صĺ���
**********************************************************/

#include "usart.h"

/**********************************************************
   �������´��룬֧��printf������������Ҫѡ��use MicroLIB
**********************************************************/

#if 1

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	while((USART1->SR & 0x40) == 0);	//ѭ�����ͣ�ֱ���������   
    USART1->DR = (uint8_t)ch;	//��������      
	return ch;
}
#endif

/**********************   end   **************************/


/**********************************************************
* �������� ---> ��ʼ��IO ����1
* ��ڲ��� ---> bound��������	
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void USARTx_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#ifdef EN_USART1_RX	//���ʹ���˽����ж�
	
	USART1->CR1 |= 1 << 8;	//PE�ж�ʹ��
	USART1->CR1 |= 1 << 5;	//���ջ������ǿ��ж�ʹ��
	
	MY_NVIC_Init(3, 3, USART1_IRQn, NVIC_PriorityGroup_2);	//�жϷ���2��������ȼ�
	
#endif
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
		
	USART_DeInit(USART1);  //��λ����1
	
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10
  
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;	//���ò����ʣ�һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;	//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
}

//=========================================================
 
#ifdef EN_USART1_RX   //���ʹ���˽���

//=========================================================

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ��� 

uint8_t USART_RX_BUF[USART_REC_LEN];	//���ջ���,���USART_REC_LEN���ֽ�

//����״̬
//bit15��������ɱ�־
//bit14�����յ�0x0d
//bit13~0�����յ�����Ч�ֽ���Ŀ�����512�ֽ�

uint16_t USART_RX_STA=0;	//����״̬���	  

/**********************************************************
* �������� ---> ����1�����жϷ������
* ��ڲ��� ---> bound��������	
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/
void USART1_IRQHandler(void)
{
	uint8_t res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //���յ�����
	{
		res = USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA & 0x8000) == 0)//����δ���
		{
			if(USART_RX_STA & 0x4000)//���յ���0x0d
			{
				/***********************************************
                                  �޸���������
                    ���û����ݵ�����0x0d��ʱ�������Ĵ�����ж�
				***********************************************/
				
				if(res != 0x0a)
				{
					USART_RX_BUF[USART_RX_STA & 0x3fff] = 0x0d;	//���϶�ʧ��0x0d����
					USART_RX_STA++;
					USART_RX_BUF[USART_RX_STA & 0x3fff] = res;	//������������
					USART_RX_STA++;
					USART_RX_STA &= 0xbfff;						//���0x0d��־
				}
				
				/***********************************************
                                      �޸����
				***********************************************/
				
				else	USART_RX_STA |= 0x8000;	//���������
			}
			else //��û�յ�0x0d
			{	
				if(res == 0x0d)	USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0x3fff] = res;
					USART_RX_STA++;
					if(USART_RX_STA > (USART_REC_LEN - 1))	USART_RX_STA = 0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}	//end ����δ���   		 
	}	//end ���յ�����
}

//=========================================================

#endif	//endʹ�ܽ���

//=========================================================

