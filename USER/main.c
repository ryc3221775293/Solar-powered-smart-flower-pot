#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "adc.h"
#include "timer.h"
#include "usart3.h"
#include "gizwits_product.h" 
#include "lsens.h"
/* 用户区当前设备状态结构体*/
dataPoint_t currentDataPoint;

//WIFI连接状态
//wifi_sta 0: 断开
//         1: 已连接
u8 wifi_sta=0;

//协议初始化
void Gizwits_Init(void)
{	
	TIM3_Int_Init(9,7199);//1MS系统定时
    usart3_init(9600);//WIFI初始化
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化
	gizwitsInit();//缓冲区初始化
}
 int main(void)
 {	 
  u16 adcx,adcy,adcz;
	float temp,temp1;
	float percent;
	int key;
	u8 wifi_con=0;//记录wifi连接状态 1:连接 0:断开  
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化		 	
 	Adc_Init();		  		//ADC初始化
  KEY_Init();
	Lsens_Init(); 			//初始化光敏传感器
	Gizwits_Init();         //协议初始化
	printf("--------机智云IOT-DHT11温湿度检测报警实验----------\r\n");
	printf("KEY1:AirLink连接模式\t KEY_UP:复位\r\n\r\n");
	while(1)
	{
		
		if(wifi_con!=wifi_sta)
	   {
		   wifi_con=wifi_sta;
		   wifi_con?printf("connect"):printf("close");
	   }
    adcx=Get_Adc_Average(ADC_Channel_1,10);
		adcy=Get_Adc_Average(ADC_Channel_2,10);
		temp=(float)adcx*(3.3/4096);
		temp1=((float)adcy*(3.3/4096))*5;
		percent=(3.3-temp)/3.3*100;
		adcz=Lsens_Get_Val();
		currentDataPoint.valueHumidity = percent;
		currentDataPoint.valueVoltage = temp1;
		currentDataPoint.valueLight = adcz;	
		//printf("Humity:%f   %%\r\n",percent);
		//printf("ADC_channel2:%f\r\n",temp1);
		if(0x01 == currentDataPoint.valueAutomatic)
		{
			currentDataPoint.valueWatering=0;
			if(percent<40)
			{
					LED0=1;
			}else
			{
					LED0=0;
			}
		}else
		{
				if(0x01 == currentDataPoint.valueWatering)
        {
          //user handle
					LED0=1;
        }
        else
        {
          //user ha.ndle
					LED0=0;
        }
		}	
		if(temp1<2)
		{
				currentDataPoint.valueWarning = 1;
		}else
		{
				currentDataPoint.valueWarning = 0;
		}
		
    gizwitsHandle((dataPoint_t *)&currentDataPoint);//协议处理
 		
	  key = KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按键
		{
			printf("WIFI进入AirLink连接模式\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-link模式接入
		}			
		if(key==WKUP_PRES)//KEY_UP按键
		{  
			printf("WIFI复位，请重新配置连接\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);//WIFI复位 
			wifi_sta=0;//标志wifi已断开
		}
		delay_ms(200);	
	}
 }

