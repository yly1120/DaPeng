/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "adcx.h"
#include "Light.h"
#include "dht11.h"
#include "Soil_Humidity.h"
#include "CO2.h"
#include "ph.h"
#include "esp8266.h"
#include "onenet.h"
#include "key.h"
#include "beep.h"
#include "pump.h"
#include "led.h"
#include "fan.h"
#include "spray.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t temp,humi;              // 温湿度
uint16_t lux;                   // 光照强度 0-999
uint16_t soil_humi;             // 土壤湿度 0-100
uint16_t co2_ppm;               // CO2浓度   
float soil_ph;                  // 土壤PH值 0.00-14.00

uint8_t Key_Val,Key_Down,Key_Old,Key_Up;
volatile uint8_t Key_Slow_Down;         // 按键减速处理
volatile uint16_t OLED_Slow_Down;       // OLED减速处理
volatile uint32_t cloud_time = 0;              // 云平台发送数据计时

uint8_t OLED_MENU = 0;                  // 菜单 1-主菜单 2-传感器菜单 3-阈值设置 4-设备状态
uint8_t OLED_Submenu_Sensor = 0;        // 传感器子菜单 1-温湿度 2-CO2浓度 3-光照 4-土壤湿度  5-PH值
uint8_t OLED_Submenu_Threshold = 0;       // 阈值设置子菜单 
uint8_t OLED_Submenu_Device = 0;          // 设备状态子菜单


uint8_t Cursor_pos = 0;         // 光标位置 

uint8_t OLED_Clear_Flag = 0;   // OLED清屏标志

volatile uint8_t WifiOK_flag = 0;        // WiFi连接成功标志
volatile uint8_t cloud_flag = 0;         // 云平台发送数据标志位

// ===== 阈值参数(初始设置值）=====
int16_t  TH_Temp      = 35;    // 温度阈值(°C)
uint8_t  TH_Humi      = 25;    // 湿度阈值(%)
uint16_t TH_CO2       = 1000;   // CO2阈值(ppm)
uint16_t TH_Lux       = 800;   // 光照阈值(lx)
uint8_t  TH_SoilHumi  = 5;    // 土壤湿度阈值(%)
uint16_t TH_SoilPH_Low_x100  = 200; // 土壤PH阈值*100 下限
uint16_t TH_SoilPH_High_x100 = 1200; // 土壤PH阈值*100 上限


// ===== 自动报警状态 =====
uint8_t ALM_Temp = 0;     // 温度报警
uint8_t ALM_Humi = 0;     // 湿度报警
uint8_t ALM_CO2 = 0;      // CO2报警
uint8_t ALM_Lux = 0;      // 光照报警
uint8_t ALM_SoilHumi = 0;   // 土壤湿度报警
uint8_t ALM_SoilPH = 0;     // 土壤PH报警

// 手动/自动控制
typedef enum
{
  CTRL_AUTO = 0,
  CTRL_MANUAL_ON,
  CTRL_MANUAL_OFF
} CtrlMode_t;


volatile ControlMode_t control_mode = CONTROL_MODE_AUTO;  // 上电默认自动控制模式

//  这里用的少了，后续删
// LED 不参与阈值联动，纯手动（默认开）
uint8_t LED_ManualState = 0;
uint8_t BEEP_ManualOn = 0;   // 1=手动强制开蜂鸣器
//  这里用的少了，后续删

//  这里用的少了，后续删
// 三个会被阈值联动的外设：自动/手动共存
CtrlMode_t FAN_CtrlMode   = CTRL_AUTO;
CtrlMode_t SPRAY_CtrlMode = CTRL_AUTO;
CtrlMode_t PUMP_CtrlMode  = CTRL_AUTO;
//  这里用的少了，后续删



// ===== 阈值子菜单编辑状态 =====
uint8_t  TH_EditMode = 0;      // 0未编辑 1编辑中
int32_t  TH_EditValue = 0;     // 当前编辑值(通用整型)


volatile uint8_t led_on_state = 0;   // LED状态标志位
volatile uint8_t beep_on_state = 0;  // 蜂鸣器状态标志位
volatile uint8_t fan_on_state = 0;   // 风扇状态标志位
volatile uint8_t spray_on_state = 0; // 水雾状态标志位
volatile uint8_t pump_on_state = 0;  // 水泵状态标志位


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

#define SENSOR_ITEM_COUNT   5    // 传感器菜单项数
#define SENSOR_ITEM_VISIBLE 3    // 传感器菜单可见项数（超过这个数会滚动）
uint8_t Sensor_Menu_Start = 0;   // 当前窗口起始项(0~2)

#define THRESH_ITEM_COUNT   7    // 阈值菜单项数
#define THRESH_ITEM_VISIBLE 3    // 阈值菜单可见项数
uint8_t Threshold_Menu_Start = 0; // 阈值菜单当前窗口起始项

#define DEVICE_ITEM_COUNT   12    // 设备状态菜单项数
#define DEVICE_ITEM_VISIBLE 3     // 设备状态菜单可见项数
uint8_t Device_Menu_Start = 0;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void APPInit()
{
  HAL_TIM_Base_Start_IT(&htim4);
  
  OLED_Init();

  ESP8266_EN();  // 启用ESP8266串口接收（中断方式）

  HAL_ADCEx_Calibration_Start(&hadc1);  // ADC校准

  CO2_UART_StartReceive();  // 启动CO2数据接收 启动后大概要等1分钟才有数据

  Boot_ChargingUI();
  
  ESP8266_Init();  // 初始化ESP8266并连接WiFi

  WifiOK_flag = 1;  // WiFi连接成功标志

// printf("5. ESP8266 Init OK\r\n");
	OLED_Clear(); OLED_ShowString(0,0,(uint8_t *)"ESP8266 Init OK",16, 1); OLED_Refresh(); 
	HAL_Delay(500);
	// // printf("Connect MQTTs Server...\r\n");
	OLED_Clear(); OLED_ShowString(0,0,(uint8_t *)"Connect MQTTs",16, 1);OLED_Refresh();
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "OK"))
	  HAL_Delay(500);
	//	// printf("Connect MQTT Server Success\r\n");
	OLED_ShowString(0,16,(uint8_t *)"Connect Success",16, 1); OLED_Refresh();
	HAL_Delay(500);

	OLED_ShowString(0,32,(uint8_t *)"Device login",16, 1); OLED_Refresh();
	while(OneNet_DevLink())			//接入OneNET
	{
		ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
		HAL_Delay(500);
	}
	OLED_Clear(); OLED_ShowString(0,0,(uint8_t *)"Link OK",16, 1); OLED_Refresh();

	OneNET_Subscribe();		//订阅主题
	OLED_ShowString(0,16,(uint8_t *)"Subscribe OK",16, 1); OLED_Refresh();
  
  HAL_Delay(2000);
  DHT11_Init();   // 上电等待2s，初始化DHT11

  DHT11_Read_Data(&temp, &humi);   // 丢弃第一次数据（可能是旧值）
  HAL_Delay(2000);
  DHT11_Read_Data(&temp, &humi);   // 第二次更接近当前
  
	OLED_ShowString(0,32,(uint8_t *)"System Init OK",16, 1); OLED_Refresh();

  HAL_Delay(1000);  
  OLED_MENU = 1;    // 默认进入主菜单
  Cursor_pos = 0;   // 初始化光标位置
  OLED_Clear();
}

// 读取阈值然后返回
static int32_t TH_GetValueBySub(uint8_t sub)
{
  switch (sub)
  {
    case 1: return TH_Temp;
    case 2: return TH_Humi;
    case 3: return TH_CO2;
    case 4: return TH_Lux;
    case 5: return TH_SoilHumi;
    case 6: return TH_SoilPH_Low_x100;
    case 7: return TH_SoilPH_High_x100;
    default: return 0;
  }
}

// 根据子菜单编号设置对应的阈值
static void TH_SetValueBySub(uint8_t sub, int32_t v)
{
  switch (sub)
  {
    case 1: TH_Temp = (int16_t)v; break;
    case 2: TH_Humi = (uint8_t)v; break;
    case 3: TH_CO2 = (uint16_t)v; break;
    case 4: TH_Lux = (uint16_t)v; break;
    case 5: TH_SoilHumi = (uint8_t)v; break;
    case 6: TH_SoilPH_Low_x100  = (uint16_t)v; break;
    case 7: TH_SoilPH_High_x100 = (uint16_t)v; break;
    default: break;
  }

}
// 返回阈值的最小值（用于编辑时的下限）
static int32_t TH_MinBySub(uint8_t sub)
{
  switch (sub)
  {
    case 1: return 0;       // 温度
    case 2: return 0;       // 湿度
    case 3: return 300;     // CO2
    case 4: return 0;       // 光照
    case 5: return 0;       // 土壤湿度
    case 6: return 0;       // pH下限 0.00
    case 7: return 700;     // pH上限 7.00

    default:return 0;
  }
}
// 返回阈值的最大值（用于编辑时的上限）
static int32_t TH_MaxBySub(uint8_t sub)
{
  switch (sub)
  { 
    case 1: return 60;      // 温度
    case 2: return 100;     // 湿度
    case 3: return 1000;    // CO2
    case 4: return 999;     // 光照
    case 5: return 100;     // 土壤湿度
    case 6: return 700;     // pH下限 7.00
    case 7: return 1400;    // pH上限 14.00
    default:return 0;
  }
}
// 设置阈值的步进值
static int32_t TH_StepBySub(uint8_t sub)
{
  if (sub == 6|| sub == 7) return 5;  // pH每次0.05
  if (sub == 3) return 50; // CO2每次50
  if (sub == 4) return 10; // 光照每次10
  return 1;
}

static void AutoControl_Task(void)
{
  uint16_t ph_x100;
  uint8_t fan_need;
  uint8_t spray_need;
  uint8_t pump_need;
  uint8_t beep_need;
  uint8_t soil_alarm_for_beep;

  if (control_mode == CONTROL_MODE_MANUAL)
  {
    return;   // 手动模式下，不覆盖手动设置的标志位
  }

  ph_x100 = (uint16_t)(soil_ph * 100.0f + 0.5f);

  ALM_Temp     = (temp > TH_Temp) ? 1 : 0;  // 温度大于设定值
  ALM_CO2      = (co2_ppm > TH_CO2) ? 1 : 0;  // CO2浓度大于设定值
  ALM_Humi     = (humi < TH_Humi) ? 1 : 0;  // 湿度小于设定值
  ALM_SoilHumi = (soil_humi < TH_SoilHumi) ? 1 : 0; // 土壤湿度小于设定值
  ALM_SoilPH   = (ph_x100 < TH_SoilPH_Low_x100 || ph_x100 > TH_SoilPH_High_x100) ? 1 : 0; // 土壤PH值超出设定范围
  ALM_Lux      = (lux > TH_Lux) ? 1 : 0;  // 光照强度大于设定值

  soil_alarm_for_beep = (PUMP_CtrlMode == CTRL_AUTO) ? (ALM_SoilHumi || ALM_SoilPH) : 0;
 
  fan_need   = (ALM_Temp || ALM_CO2) ? 1 : 0; // 温度或CO2报警需要开风扇
  spray_need = ALM_Humi ? 1 : 0; // 湿度报警需要开水雾
  pump_need  = (ALM_SoilHumi || ALM_SoilPH) ? 1 : 0;  // 土壤湿度或PH报警需要开水泵
  beep_need  = (ALM_Temp || ALM_CO2 || ALM_Humi || ALM_Lux || soil_alarm_for_beep) ? 1 : 0; // 报警开启BEEP

  // LED 不参与自动控制，保留当前手动状态
  fan_on_state   = fan_need;
  spray_on_state = spray_need;
  pump_on_state  = pump_need;
  beep_on_state  = beep_need;
  
}



void Key_Task()
{
  if(Key_Slow_Down) return;
  Key_Slow_Down = 10;

  Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;

  switch(Key_Down)
  {
	  case 1:	    // 上
      if(OLED_MENU==1)  // 主菜单
      {
        if(--Cursor_pos>240) Cursor_pos=2;  
      }

      if(OLED_MENU==2&&OLED_Submenu_Sensor==0)  // 传感器菜单
      {
        if(--Cursor_pos>240) Cursor_pos = SENSOR_ITEM_COUNT - 1;
      }

      if (OLED_MENU == 3)  // 阈值菜单
      {
        if(OLED_Submenu_Threshold==0)
        {
          if(--Cursor_pos > 240) Cursor_pos = THRESH_ITEM_COUNT - 1;
        }
        else if (TH_EditMode)
        {
          int32_t minv = TH_MinBySub(OLED_Submenu_Threshold);
          int32_t step = TH_StepBySub(OLED_Submenu_Threshold);
          TH_EditValue -= step;
          if (TH_EditValue < minv) TH_EditValue = minv;
        }
      }

      if (OLED_MENU == 4) 
      {
        if (--Cursor_pos > 240) Cursor_pos = DEVICE_ITEM_COUNT - 1;
      }

      break;

    case 2:		  // 下
      if(OLED_MENU==1)  // 主菜单
      {
        if(++Cursor_pos==3) Cursor_pos=0;
      }

      if(OLED_MENU==2&&OLED_Submenu_Sensor==0)  // 传感器菜单
      {
        if (++Cursor_pos >= SENSOR_ITEM_COUNT) Cursor_pos = 0;
      }

      if (OLED_MENU == 3)  // 阈值菜单
      {
        if(OLED_Submenu_Threshold==0)
        {
          if (++Cursor_pos >= THRESH_ITEM_COUNT) Cursor_pos = 0;
        }
        else if (TH_EditMode)
        {
          int32_t maxv = TH_MaxBySub(OLED_Submenu_Threshold);
          int32_t step = TH_StepBySub(OLED_Submenu_Threshold);
          TH_EditValue += step;
          if (TH_EditValue > maxv) TH_EditValue = maxv;
        }
      }

      if (OLED_MENU == 4) 
      {
        if (++Cursor_pos >= DEVICE_ITEM_COUNT) Cursor_pos = 0;
      }

      break;

    case 3:		  // 确认
      OLED_Clear_Flag ^= 1;  // 标记需要清屏
      if(OLED_MENU==1)  // 主菜单
      {
        switch (Cursor_pos)
        {
          case 0: // 传感器菜单
            OLED_MENU = 2; Cursor_pos = 0;Sensor_Menu_Start = 0;
            break;
          case 1: // 阈值设置
            OLED_MENU = 3; Cursor_pos = 0;Threshold_Menu_Start = 0;OLED_Submenu_Threshold=0;TH_EditMode = 0;
            break;
          case 2: // 设备状态
            OLED_MENU = 4; Cursor_pos = 0;Device_Menu_Start = 0;control_mode = CONTROL_MODE_MANUAL;   // 进入设备状态切换手动控制
            break;
          
          default:
            break;
        }
        return;
      }

      else if(OLED_MENU==2&&OLED_Submenu_Sensor==0)  // 传感器菜单
      {
        switch (Cursor_pos)
        {
            case 0: OLED_Submenu_Sensor = 1; break;   // 温湿度
            case 1: OLED_Submenu_Sensor = 2; break;   // CO2
            case 2: OLED_Submenu_Sensor = 3; break;   // 光照
            case 3: OLED_Submenu_Sensor = 4; break;   // 土壤湿度
            case 4: OLED_Submenu_Sensor = 5; break;   // 土壤PH
            default: break;
        }
        OLED_Clear_Flag = 1;    // 切页清屏

      }

      else if (OLED_MENU == 3)
      {
        if (OLED_Submenu_Threshold == 0)
        {
          // 从阈值列表进入某个阈值详情
          OLED_Submenu_Threshold = Cursor_pos + 1;   // 1~6
          TH_EditValue = TH_GetValueBySub(OLED_Submenu_Threshold);
          TH_EditMode = 1;                           // 进入即编辑
          OLED_Clear_Flag = 1;
        }
        else
        {
          // KEY3确认：保存并返回阈值列表
          TH_SetValueBySub(OLED_Submenu_Threshold, TH_EditValue);
          // PH阈值保护
          if (TH_SoilPH_Low_x100 >= TH_SoilPH_High_x100)
          {
            if(OLED_Submenu_Threshold == 6)
            {
              TH_SoilPH_Low_x100 = TH_SoilPH_High_x100 - 1;
            }
            else if(OLED_Submenu_Threshold == 7)
            {
              TH_SoilPH_High_x100 = TH_SoilPH_Low_x100 + 1;
            }
          }
          TH_EditMode = 0;
          OLED_Submenu_Threshold = 0;
          OLED_Clear_Flag = 1;
        }
      }

      else if (OLED_MENU == 4)
      {
          switch (Cursor_pos)
          {
            case 0: // LED
              led_on_state = !led_on_state;
              break;
              
            case 1: // BEEP
              beep_on_state = !beep_on_state;
              break;

            case 2: // FAN
              fan_on_state = !fan_on_state;
              break;

            case 3: // SPRAY
              spray_on_state = !spray_on_state;
              break;

            case 4: // PUMP
              pump_on_state = !pump_on_state;
              break;
            default: break;
          }

          OLED_Clear_Flag = 1; // 可选：让显示立即干净刷新
      }


      break;

    case 4:		  // 返回
      if (OLED_MENU == 2 && OLED_Submenu_Sensor != 0)
      {
          OLED_Submenu_Sensor = 0;     // 返回传感器列表
          OLED_Clear_Flag = 1;
          return;                 // 本次按键处理到此结束
      }
      
      // 
      if (OLED_MENU == 3 && OLED_Submenu_Threshold != 0)
      {
        OLED_Submenu_Threshold = 0;OLED_Clear_Flag = 1;TH_EditMode = 0;
        return;
      }

      // 返回主菜单
      OLED_Clear_Flag ^= 1;  // 标记需要清屏
      
      if (OLED_MENU == 4)
      {
        control_mode = CONTROL_MODE_AUTO;   // 离开设备菜单，恢复自动控制
        OLED_MENU = 1;Cursor_pos = 0;
        //AutoControl_Task();                 // 立即按自动逻辑刷新一次
      }
      else if (OLED_MENU == 2 || OLED_MENU == 3)
      {
        OLED_MENU = 1; Cursor_pos = 0;
      }

      break;
	
  }
  // 菜单滚动控制
  if (OLED_MENU == 2) 
  {
    if (Cursor_pos < Sensor_Menu_Start)
      Sensor_Menu_Start = Cursor_pos;
    else if (Cursor_pos >= Sensor_Menu_Start + SENSOR_ITEM_VISIBLE)
      Sensor_Menu_Start = Cursor_pos - (SENSOR_ITEM_VISIBLE - 1);
  }
  else if (OLED_MENU == 3 && OLED_Submenu_Threshold == 0)
  {
    if (Cursor_pos < Threshold_Menu_Start)
      Threshold_Menu_Start = Cursor_pos;
    else if (Cursor_pos >= Threshold_Menu_Start + THRESH_ITEM_VISIBLE)
      Threshold_Menu_Start = Cursor_pos - (THRESH_ITEM_VISIBLE - 1);
  }
  else if (OLED_MENU == 4)
  {
    if (Cursor_pos < Device_Menu_Start)
      Device_Menu_Start = Cursor_pos;
    else if (Cursor_pos >= Device_Menu_Start + DEVICE_ITEM_VISIBLE)
      Device_Menu_Start = Cursor_pos - (DEVICE_ITEM_VISIBLE - 1);
  }


}

void Oled_Task()
{
  // 菜单滑动用到的变量
	uint8_t row, idx, y, mode;
	
  // 设备菜单用到的变量
  uint8_t name_end_x = 0;   // 名称末尾x（用于放冒号）
  uint8_t is_on = 0;        // 判断设备状态
  uint8_t colon_x = 0;      // 冒号x坐标
  uint8_t status_x = 0;     // 状态文本x坐标


  if(OLED_Slow_Down) return;
  OLED_Slow_Down = 100;


  if(OLED_Clear_Flag)
  {
    OLED_Clear_Flag = 0;
    OLED_Clear();
  }

  switch (OLED_MENU)
  {
    case 1: // 主菜单
      OLED_ShowChinese(40, 0, 0, 16, 1);OLED_ShowChinese(56, 0, 1, 16, 1);OLED_ShowChinese(72, 0, 2, 16, 1); // 主菜单
      
      // 1 2 3
      OLED_ShowString(0, 16, (uint8_t*)"1.", 16, (Cursor_pos==0)?0:1);OLED_ShowString(0, 32, (uint8_t*)"2.", 16, (Cursor_pos==1)?0:1);OLED_ShowString(0, 48, (uint8_t*)"3.", 16, (Cursor_pos==2)?0:1);
      
      // 传感器数据
      OLED_ShowChinese(16, 16, 3, 16, (Cursor_pos==0)?0:1);OLED_ShowChinese(32, 16, 4, 16, (Cursor_pos==0)?0:1);OLED_ShowChinese(48, 16, 5, 16, (Cursor_pos==0)?0:1);OLED_ShowChinese(64, 16, 6, 16, (Cursor_pos==0)?0:1);OLED_ShowChinese(80, 16, 7 , 16, (Cursor_pos==0)?0:1);
      
      // 阈值设置
      OLED_ShowChinese(16, 32, 8, 16, (Cursor_pos==1)?0:1);OLED_ShowChinese(32, 32, 9, 16, (Cursor_pos==1)?0:1);OLED_ShowChinese(48, 32, 10, 16, (Cursor_pos==1)?0:1);OLED_ShowChinese(64, 32, 11, 16, (Cursor_pos==1)?0:1);
      
      // 设备状态
      OLED_ShowChinese(16, 48, 12, 16, (Cursor_pos==2)?0:1);OLED_ShowChinese(32, 48, 13, 16, (Cursor_pos==2)?0:1);OLED_ShowChinese(48, 48, 14, 16, (Cursor_pos==2)?0:1);OLED_ShowChinese(64, 48, 15, 16, (Cursor_pos==2)?0:1);
      
      break;
    
    case 2:     // 传感器菜单
      if (OLED_MENU == 2 && OLED_Submenu_Sensor == 0)
      {
        OLED_ShowChinese(24, 0, 3, 16, 1);OLED_ShowChinese(40, 0, 4, 16, 1);OLED_ShowChinese(56, 0, 5, 16, 1);OLED_ShowChinese(72, 0, 1, 16, 1);OLED_ShowChinese(88, 0, 2, 16, 1); // 传感器菜单
        for (row = 0; row < SENSOR_ITEM_VISIBLE; row++)
        {
          idx = Sensor_Menu_Start + row;
          if (idx >= SENSOR_ITEM_COUNT) break;

          y = 16 * (row + 1);                    // 16, 32, 48

          OLED_ShowString(0, y, (uint8_t*)"                ", 16, 1);  // 清除旧数据

          mode = (idx == Cursor_pos) ? 0 : 1;    // 选中项反色

          switch (idx)
          {
            case 0: // 温湿度
              OLED_ShowChinese(0,  y, 16, 16, mode);
              OLED_ShowChinese(16, y, 17, 16, mode);
              OLED_ShowChinese(32, y, 18, 16, mode);
              break;

            case 1: // CO2浓度
              OLED_ShowChinese(0,  y, 19, 16, mode);
              OLED_ShowChinese(16, y, 20, 16, mode);
              OLED_ShowChinese(32, y, 21, 16, mode);
              OLED_ShowChinese(48, y, 22, 16, mode);
              break;

            case 2: // 光照强度
              OLED_ShowChinese(0,  y, 23, 16, mode);
              OLED_ShowChinese(16, y, 24, 16, mode);
              OLED_ShowChinese(32, y, 25, 16, mode);
              OLED_ShowChinese(48, y, 26, 16, mode);
              break;

            case 3: // 土壤湿度
              OLED_ShowChinese(0,  y, 35, 16, mode);
              OLED_ShowChinese(16, y, 36, 16, mode);
              OLED_ShowChinese(32, y, 37, 16, mode);
              OLED_ShowChinese(48, y, 38, 16, mode);
              break;

            case 4: // 土壤PH值
              OLED_ShowChinese(0,  y, 35, 16, mode);
              OLED_ShowChinese(16, y, 36, 16, mode);
              OLED_ShowString(32, y, (uint8_t*)"PH", 16, mode);
              break;

            default:
              break;
          }
        }
      }
      else
      {
        OLED_ShowString(0, 0,  (uint8_t*)"Sensor Detail", 16, 1);
        switch (OLED_Submenu_Sensor)
        {
            case 1: // 温湿度
                OLED_ShowString(0, 16, (uint8_t*)"Temp:", 16, 1);
                OLED_ShowNum(48, 16, temp, 2, 16, 1);
                OLED_ShowString(72, 16, (uint8_t*)"C", 16, 1);

                OLED_ShowString(0, 32, (uint8_t*)"Humi:", 16, 1);
                OLED_ShowNum(48, 32, humi, 2, 16, 1);
                OLED_ShowString(72, 32, (uint8_t*)"%", 16, 1);
                break;

            case 2: // CO2
                OLED_ShowString(0, 16, (uint8_t*)"CO2:", 16, 1);
                OLED_ShowNum(40, 16, co2_ppm, 4, 16, 1);
                OLED_ShowString(80, 16, (uint8_t*)"ppm", 16, 1);
                break;

            case 3: // 光照
                OLED_ShowString(0, 16, (uint8_t*)"Light:", 16, 1);
                OLED_ShowNum(56, 16, lux, 3, 16, 1);
                OLED_ShowString(88, 16, (uint8_t*)"lx", 16, 1);
                break;

            case 4: // 土壤湿度
                OLED_ShowString(0, 16, (uint8_t*)"Soil:", 16, 1);
                OLED_ShowNum(48, 16, soil_humi, 3, 16, 1);
                OLED_ShowString(72, 16, (uint8_t*)"%", 16, 1);
                break;

            case 5: // 土壤PH
            {
                uint16_t ph_x100 = (uint16_t)(soil_ph * 100.0f + 0.5f);
                uint16_t ph_int  = ph_x100 / 100;
                uint16_t ph_frac = ph_x100 % 100;

                OLED_ShowString(0, 16, (uint8_t*)"PH:", 16, 1);
                OLED_ShowNum(32, 16, ph_int, 2, 16, 1);
                OLED_ShowChar(48, 16, '.', 16, 1);
                OLED_ShowNum(56, 16, ph_frac, 2, 16, 1);
                break;
            }

            default:
                OLED_Submenu_Sensor = 0;
                break;
        }
      }
      
      break;

    case 3:   // 阈值设置
      if (OLED_Submenu_Threshold == 0)
      {
        OLED_ShowChinese(32, 0, 27, 16, 1);
        OLED_ShowChinese(48, 0, 28, 16, 1);
        OLED_ShowChinese(64, 0, 29, 16, 1);
        OLED_ShowChinese(80, 0, 30, 16, 1);
        
        for (row = 0; row < THRESH_ITEM_VISIBLE; row++)
        {
          idx = Threshold_Menu_Start + row;
          if (idx >= THRESH_ITEM_COUNT) break;

          y = 16 * (row + 1);                 // 16,32,48
          OLED_ShowString(0, y, (uint8_t*)"                ", 16, 1);  // 清行防残影
          mode = (idx == Cursor_pos) ? 0 : 1; // 选中反色

          switch (idx)
          {
            case 0: // 温度阈值
              OLED_ShowChinese(0,  y, 16, 16, mode);
              OLED_ShowChinese(16, y, 18, 16, mode);
              OLED_ShowChinese(32, y, 27, 16, mode);
              OLED_ShowChinese(48, y, 28, 16, mode);
              break;

            case 1: // 湿度阈值
              OLED_ShowChinese(0,  y, 17, 16, mode);
              OLED_ShowChinese(16, y, 18, 16, mode);
              OLED_ShowChinese(32, y, 27, 16, mode);
              OLED_ShowChinese(48, y, 28, 16, mode);
              break;

            case 2: // CO2阈值
              OLED_ShowChinese(0,  y, 19, 16, mode);
              OLED_ShowChinese(16, y, 20, 16, mode);
              OLED_ShowChinese(32, y, 21, 16, mode);
              OLED_ShowChinese(48, y, 22, 16, mode);
              OLED_ShowChinese(64, y, 27, 16, mode);
              OLED_ShowChinese(80, y, 28, 16, mode);
              break;

            case 3: // 光照阈值
              OLED_ShowChinese(0,  y, 23, 16, mode);
              OLED_ShowChinese(16, y, 24, 16, mode);
              OLED_ShowChinese(32, y, 25, 16, mode);
              OLED_ShowChinese(48, y, 26, 16, mode);
              OLED_ShowChinese(64, y, 27, 16, mode);
              OLED_ShowChinese(80, y, 28, 16, mode);
              break;

            case 4: // 土壤湿度阈值
              OLED_ShowChinese(0,  y, 35, 16, mode);
              OLED_ShowChinese(16, y, 36, 16, mode);
              OLED_ShowChinese(32, y, 37, 16, mode);
              OLED_ShowChinese(48, y, 38, 16, mode);
              OLED_ShowChinese(64, y, 27, 16, mode);
              OLED_ShowChinese(80, y, 28, 16, mode);
              break;

            case 5: // 土壤PH阈值下限
              OLED_ShowChinese(0,  y, 35, 16, mode);
              OLED_ShowChinese(16, y, 36, 16, mode);
              OLED_ShowString(32, y, (uint8_t*)"PH", 16, mode);
              OLED_ShowChinese(48, y, 27, 16, mode);
              OLED_ShowChinese(64, y, 28, 16, mode);
              OLED_ShowString(80, y, (uint8_t*)"Low", 16, mode);

              break;
            case 6: // 土壤PH阈值上限
              OLED_ShowChinese(0,  y, 35, 16, mode);
              OLED_ShowChinese(16, y, 36, 16, mode);
              OLED_ShowString(32, y, (uint8_t*)"PH", 16, mode);
              OLED_ShowChinese(48, y, 27, 16, mode);
              OLED_ShowChinese(64, y, 28, 16, mode);
              OLED_ShowString(80, y, (uint8_t*)"High", 16, mode);
          }
        }
      }  
      else
      {
        // ===== 阈值详情编辑页 =====
        OLED_ShowString(0, 0, (uint8_t*)"THRESH EDIT", 16, 1);

        // 显示项目名
        switch (OLED_Submenu_Threshold)
        {
          case 1: OLED_ShowString(0, 16, (uint8_t*)"Temp", 16, 1); break;
          case 2: OLED_ShowString(0, 16, (uint8_t*)"Humi", 16, 1); break;
          case 3: OLED_ShowString(0, 16, (uint8_t*)"CO2",  16, 1); break;
          case 4: OLED_ShowString(0, 16, (uint8_t*)"Light",16, 1); break;
          case 5: OLED_ShowString(0, 16, (uint8_t*)"SoilH",16, 1); break;
          case 6: OLED_ShowString(0, 16, (uint8_t*)"SoilPH_Low",16,1); break;
          case 7: OLED_ShowString(0, 16, (uint8_t*)"SoilPH_High",16,1); break;
          default: break;
        }

        // 当前编辑值
        OLED_ShowString(0, 32, (uint8_t*)"SET:", 16, 1);

        if (OLED_Submenu_Threshold == 6|| OLED_Submenu_Threshold == 7)  // pH值特殊显示
        {
          uint16_t v = (uint16_t)TH_EditValue;
          OLED_ShowNum(40, 32, v / 100, 2, 16, 0);   // 反色显示
          OLED_ShowChar(56, 32, '.', 16, 0);
          OLED_ShowNum(64, 32, v % 100, 2, 16, 0);
        }
        else
        {
          OLED_ShowNum(40, 32, (uint32_t)TH_EditValue, 4, 16, 0);  // 反色
        }

        // 单位
        switch (OLED_Submenu_Threshold)
        {
          case 1: OLED_ShowString(88, 32, (uint8_t*)"C",   16, 1); break;
          case 2: OLED_ShowString(88, 32, (uint8_t*)"%",   16, 1); break;
          case 3: OLED_ShowString(80, 32, (uint8_t*)"ppm", 16, 1); break;
          case 4: OLED_ShowString(88, 32, (uint8_t*)"lx",  16, 1); break;
          case 5: OLED_ShowString(88, 32, (uint8_t*)"%",   16, 1); break;
          case 6: OLED_ShowString(88, 32, (uint8_t*)"pH",  16, 1); break;
          case 7: OLED_ShowString(88, 32, (uint8_t*)"pH",  16, 1); break;
          default: break;
        }

        OLED_ShowString(0, 48, (uint8_t*)"K3:Save K4:Back", 16, 1);
      }

      break;

    case 4:   // 设备状态
      OLED_ShowChinese(32, 0, 31, 16, 1);OLED_ShowChinese(48, 0, 32, 16, 1);OLED_ShowChinese(64, 0, 33, 16, 1);OLED_ShowChinese(80, 0, 34, 16, 1); 
            
      for (row = 0; row < DEVICE_ITEM_VISIBLE; row++)
      {
        idx = Device_Menu_Start + row;
        if (idx >= DEVICE_ITEM_COUNT) break;

        y = 16 * (row + 1);           // 16, 32, 48
        mode = (idx == Cursor_pos) ? 0 : 1;

        OLED_ShowString(0, y, (uint8_t*)"                ", 16, 1); // 清本行

        switch (idx)
        {
          case 0: // LED
            OLED_ShowString(0,  y, (uint8_t*)"LED", 16, mode);
            name_end_x = 24;   // 3*8
            is_on = Led_Status ? 1 : 0;
            break;

          case 1: // BEEP
            OLED_ShowChinese(0,  y, 41, 16, mode);
            OLED_ShowChinese(16, y, 42, 16, mode);
            OLED_ShowChinese(32, y, 43, 16, mode);
            name_end_x = 48;   // 3*16
            is_on = BEEP_Status ? 1 : 0;
            break;

          case 2: // 风扇
            OLED_ShowChinese(0,  y, 44, 16, mode);
            OLED_ShowChinese(16, y, 45, 16, mode);
            name_end_x = 32;   // 2*16
            is_on = FAN_Status ? 1 : 0;
            break;
          
          case 3: // 水雾
            OLED_ShowChinese(0,  y, 46, 16, mode);
            OLED_ShowChinese(16, y, 47, 16, mode);
            name_end_x = 32;
            is_on = SPRAY_Status ? 1 : 0;
            break;

          case 4: // 水泵
            OLED_ShowChinese(0,  y, 46, 16, mode);
            OLED_ShowChinese(16, y, 48, 16, mode);
            name_end_x = 32;
            is_on = PUMP_Status ? 1 : 0;
            break;

          case 5: // OLED
            OLED_ShowString(0, y, (uint8_t*)"OLED", 16, mode);
            name_end_x = 32;   // 4*8
            is_on = 1;         // 屏在亮就认为开
            break;

          case 6: // ESP8266
            OLED_ShowString(0, y, (uint8_t*)"WiFi", 16, mode);
            name_end_x = 32;   // 4*8
            is_on = (WifiOK_flag == 1) ? 1 : 0;
            break;

          case 7: // DHT11温湿度传感器
            OLED_ShowChinese(0,  y, 16, 16, mode);
            OLED_ShowChinese(16, y, 17, 16, mode);
            OLED_ShowChinese(32, y, 18, 16, mode);
            name_end_x = 48;   // 3*16
            is_on = (humi <= 100) ? 1 : 0;    // 有数据则认为正常
            break;

          case 8: // CO2传感器
            OLED_ShowChinese(0,  y, 19, 16, mode);
            OLED_ShowChinese(16, y, 20, 16, mode);
            OLED_ShowChinese(32, y, 21, 16, mode);
            OLED_ShowChinese(48, y, 22, 16, mode);
            name_end_x = 64;   // 4*16
            is_on = (co2_ppm > 0) ? 1 : 0;  
            break;

          case 9: // 光照传感器
            OLED_ShowChinese(0,  y, 23, 16, mode);
            OLED_ShowChinese(16, y, 24, 16, mode);
            name_end_x = 32;   // 2*16
            is_on = (lux <= 999) ? 1 : 0;
            break;

          case 10: // 土壤湿度传感器
            OLED_ShowChinese(0,  y, 35, 16, mode);
            OLED_ShowChinese(16, y, 36, 16, mode);
            OLED_ShowChinese(32, y, 37, 16, mode);
            OLED_ShowChinese(48, y, 38, 16, mode);
            name_end_x = 64;   // 4*16
            is_on = (soil_humi <= 100) ? 1 : 0;
            break;

          case 11: // 土壤PH传感器
            OLED_ShowChinese(0,  y, 35, 16, mode);
            OLED_ShowChinese(16, y, 36, 16, mode);
            OLED_ShowString(32, y, (uint8_t*)"PH", 16, mode);
            name_end_x = 48;   // 2*16 + 2*8
            is_on = (soil_ph >= 0.0f && soil_ph <= 14.0f) ? 1 : 0;
            break;
        }

        colon_x = name_end_x;
        if (colon_x > 96) colon_x = 96;
        OLED_ShowString(colon_x, y, (uint8_t*)":", 16, mode);

        // 状态字放在冒号后
        status_x = colon_x + 8;
        if (status_x > 112) status_x = 112;
        OLED_ShowChinese(status_x, y, is_on ? 39 : 40, 16, mode); // 39开 40关
      }
      break;

    default:
      break;
  }

  OLED_Refresh();
}

void Sensor_Task()
{
  static uint32_t t_soil = 0;
  static uint32_t t_env  = 0;
  static uint32_t t_temp_humi = 0;
  uint32_t now = HAL_GetTick();
  
  
  // 1) 土壤湿度/PH：高频采样 + 高频联动（建议100ms）
  if (now - t_soil >= 100)
  {
    t_soil = now;
    soil_humi = Soil_Humidity_GetData();
    soil_ph   = PH_GetData();
    AutoControl_Task();                       // 自动控制任务
  }

  // 2) 其他环境量：中频（200~500ms）
  if (now - t_env >= 300)
  {
    t_env = now;
    lux = Light_LuxData();
    if (Usart3_RxFlag) CO2GetData(&co2_ppm);
    AutoControl_Task();                       // 自动控制任务
  }
  
  if (now - t_temp_humi >= 2000)
  {
    t_temp_humi = now;
    DHT11_Read_Data(&temp, &humi);
    AutoControl_Task();                       // 自动控制任务
  }

}

void OneNet_Task()
{
  unsigned char *dataPtr = NULL;
	
  if(cloud_flag==1)   // 2s上传一次
  {
    cloud_flag = 0;
    OneNet_SendData();
    ESP8266_Clear();
  }

  dataPtr = ESP8266_GetIPD(1);
  if (dataPtr != NULL)
  {
    OneNet_RevPro(dataPtr);
  }

}
	

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  APPInit();  // 应用层初始化

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    Key_Task();
    Sensor_Task();
	  Oled_Task();
	  OneNet_Task();
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
      // 这里每 1ms 进一次
      if(Key_Slow_Down) Key_Slow_Down--;      // 按键处理
      if(OLED_Slow_Down) OLED_Slow_Down--;    // OLED处理
      if(++cloud_time==2000) 
      {
        cloud_time = 0;
        cloud_flag = 1;
      }

      Led_Set(led_on_state ? LED_ON : LED_OFF);           // LED控制
      BEEP_Set(beep_on_state ? BEEP_ON : BEEP_OFF);       // 蜂鸣器控制
      FAN_Set(fan_on_state ? FAN_ON : FAN_OFF);           // 风扇控制
      SPRAY_Set(spray_on_state ? SPRAY_ON : SPRAY_OFF);   // 水雾控制
      PUMP_Set(pump_on_state ? PUMP_ON : PUMP_OFF);       // 水泵控制
    }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
