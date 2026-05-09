#include "dht11.h"


// 空气温湿度相关代码

static uint8_t dht11_dwt_inited = 0;

static void DHT11_DWT_Init(void)
{
	if (dht11_dwt_inited)
		return;

	/* Enable DWT CYCCNT for accurate us delay (Cortex-M3) */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	dht11_dwt_inited = 1;
}

// 微秒级延迟函数
static void DHT11_DelayUs(uint32_t us)
{
	uint32_t start_ticks;
	uint32_t wait_ticks;

	if (!dht11_dwt_inited)
	{
		DHT11_DWT_Init();
	}

	start_ticks = DWT->CYCCNT;
	wait_ticks = us * (SystemCoreClock / 1000000U);
	while ((uint32_t)(DWT->CYCCNT - start_ticks) < wait_ticks)
	{	;	}
}
//复位DHT11
void DHT11_Rst(void)	   
{                 
	DHT11_Mode(OUT); 	//SET OUTPUT
	DHT11_Low; 	      //拉低DQ
	HAL_Delay(20);	    	//主机拉低18~30ms
	DHT11_High; 			//DQ=1 
	DHT11_DelayUs(13);	    	//主机拉高10~35us
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	DHT11_Mode(IN);//SET INPUT	 
	while ((HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) && retry < 100)//DHT11会拉低40~80us
	{
		retry++;
		DHT11_DelayUs(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
	while ((HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET) && retry < 100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		DHT11_DelayUs(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

//从DHT11读取一个位
//返回值：1/0
uint8_t DHT11_Read_Bit(void) 			 
{
	uint8_t retry=0;
	while ((HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) && retry < 100)//等待变为低电平
	{
		retry++;
		DHT11_DelayUs(1);
	}
	retry=0;
	while ((HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET) && retry < 100)//等待变高电平
	{
		retry++;
		DHT11_DelayUs(1);
	}
	DHT11_DelayUs(40);//等待40us
	if (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) return 1;
	else return 0;		   
}

//从DHT11读取一个字节
//返回值：读到的数据
uint8_t DHT11_Read_Byte(void)    
{        
	uint8_t i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_Read_Bit();
	}						    
	return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)    
{        
	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}
	else return 1;
	return 0;	    
}

//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DHT11_Init(void)
{
    DHT11_DWT_Init();     // 初始化 us 延时(DWT)

    DHT11_Mode(OUT);      // 开漏输出
    DHT11_High;           // 释放总线(空闲为高)

    // 注意：上电后第一次使用前，外部一般要 HAL_Delay(2000);

    DHT11_Rst();          // 起始信号
    return DHT11_Check(); // 检测回应
}

void DHT11_Mode(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Pin = DHT11_Pin;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

	if (mode)
	{
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
	}
	else
	{
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
	}
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStructure);
}

