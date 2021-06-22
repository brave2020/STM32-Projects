#include "sys.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "led.h"
#include "dht11.h"
#include "timer.h"
#include "oled.h"
#include "esp8266.h"
#include "onenet.h"
#include "beep.h"
#include "key.h"
#include "exti.h"

u8 humidityH;
u8 humidityL;
u8 temperatureH;
u8 temperatureL;
char PUB_BUF[256];   //�ϴ�����
u8 alarm_is_ok;      //�Ƿ��������ı�־
/************************************************
 ALIENTEK��ӢSTM32������ʵ��1
 �����ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
 int main(void)
 {	
	const char *devSubTopic[] = {"/smarthome/sub"};
	const char devPubTopic[] = {"/smarthome/pub"};
	
	unsigned short timeCount = 0;	//���ͼ������
	
	unsigned char *dataPtr = NULL;
	alarm_is_ok=1;      //�Ƿ��������ı�־
	
	BEEP_Init();
	KEY_Init();
	delay_init();	    //��ʱ������ʼ��	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�	
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	TIM2_Int_Init(4999,7199);
	TIM3_Int_Init(4999,7199);
	Usart1_Init(115200);
	Usart2_Init(115200);
	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Clear();
	DHT11_Init();
	ESP8266_Init();
	
	EXTIX_Init();         	//��ʼ���ⲿ�ж����� 
	

	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
		
	BEEP=1;				//������ʾ����ɹ�
	delay_ms(250);
  BEEP=0;

	OneNet_Subscribe(devSubTopic, 1); 
	
	while(1)
	{
		if(timeCount%40==0)			                //ÿ1s��ȡһ��
		{									  
		DHT11_Read_Data(&temperatureH,&temperatureL,&humidityH,&humidityL); 
		u1_printf("ʪ�ȣ�%d.%d �¶ȣ�%d.%d\n",humidityH,humidityL,temperatureH,temperatureL);
		}

		if(++timeCount >= 200)									//���ͼ��5s
		{
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d}",humidityH,humidityL,temperatureH,temperatureL);

			OneNet_Publish(devPubTopic, PUB_BUF);

			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
	 	delay_ms(10);

		
	}
 }
