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
char PUB_BUF[256];   //上传数据
u8 alarm_is_ok;      //是否允许报警的标志
/************************************************
 ALIENTEK精英STM32开发板实验1
 跑马灯实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
 int main(void)
 {	
	const char *devSubTopic[] = {"/smarthome/sub"};
	const char devPubTopic[] = {"/smarthome/pub"};
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	alarm_is_ok=1;      //是否允许报警的标志
	
	BEEP_Init();
	KEY_Init();
	delay_init();	    //延时函数初始化	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	
	LED_Init();		  	//初始化与LED连接的硬件接口
	TIM2_Int_Init(4999,7199);
	TIM3_Int_Init(4999,7199);
	Usart1_Init(115200);
	Usart2_Init(115200);
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	OLED_Clear();
	DHT11_Init();
	ESP8266_Init();
	
	EXTIX_Init();         	//初始化外部中断输入 
	

	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
		
	BEEP=1;				//鸣叫提示接入成功
	delay_ms(250);
  BEEP=0;

	OneNet_Subscribe(devSubTopic, 1); 
	
	while(1)
	{
		if(timeCount%40==0)			                //每1s读取一次
		{									  
		DHT11_Read_Data(&temperatureH,&temperatureL,&humidityH,&humidityL); 
		u1_printf("湿度：%d.%d 温度：%d.%d\n",humidityH,humidityL,temperatureH,temperatureL);
		}

		if(++timeCount >= 200)									//发送间隔5s
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
