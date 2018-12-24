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
/* �û�����ǰ�豸״̬�ṹ��*/
dataPoint_t currentDataPoint;

//WIFI����״̬
//wifi_sta 0: �Ͽ�
//         1: ������
u8 wifi_sta=0;

//Э���ʼ��
void Gizwits_Init(void)
{	
	TIM3_Int_Init(9,7199);//1MSϵͳ��ʱ
    usart3_init(9600);//WIFI��ʼ��
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ��
	gizwitsInit();//��������ʼ��
}
 int main(void)
 {	 
  u16 adcx,adcy,adcz;
	float temp,temp1;
	float percent;
	int key;
	u8 wifi_con=0;//��¼wifi����״̬ 1:���� 0:�Ͽ�  
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��		 	
 	Adc_Init();		  		//ADC��ʼ��
  KEY_Init();
	Lsens_Init(); 			//��ʼ������������
	Gizwits_Init();         //Э���ʼ��
	printf("--------������IOT-DHT11��ʪ�ȼ�ⱨ��ʵ��----------\r\n");
	printf("KEY1:AirLink����ģʽ\t KEY_UP:��λ\r\n\r\n");
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
		
    gizwitsHandle((dataPoint_t *)&currentDataPoint);//Э�鴦��
 		
	  key = KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1����
		{
			printf("WIFI����AirLink����ģʽ\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-linkģʽ����
		}			
		if(key==WKUP_PRES)//KEY_UP����
		{  
			printf("WIFI��λ����������������\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);//WIFI��λ 
			wifi_sta=0;//��־wifi�ѶϿ�
		}
		delay_ms(200);	
	}
 }

