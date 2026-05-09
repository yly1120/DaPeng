#include "onenet.h"


#define PROID			"Kgrb6PkwHq"	// 产品ID

#define ACCESS_KEY		"cG5KTEhDc0hHckp2OEtsQ0Z3RW9sbUVDVDFIWjZPeFo="  // 设备密钥

#define DEVICE_NAME		"System"			// 设备名称

#define Timestamp 		1956499200			// 时间戳



char devid[16];

char key[48];


extern unsigned char esp8266_buf[512];
extern volatile uint8_t led_on_state;
extern volatile uint8_t beep_on_state;
extern volatile uint8_t fan_on_state;
extern volatile uint8_t spray_on_state;
extern volatile uint8_t pump_on_state;


extern int16_t  TH_Temp;    			// 温度阈值(°C)
extern uint8_t  TH_Humi;    			// 湿度阈值(%)
extern uint16_t TH_CO2;   				// CO2阈值(ppm)
extern uint16_t TH_Lux;   				// 光照阈值(lx)
extern uint8_t  TH_SoilHumi;    		// 土壤湿度阈值(%)
extern uint16_t TH_SoilPH_Low_x100; 	// 土壤PH阈值*100 下限
extern uint16_t TH_SoilPH_High_x100; 	// 土壤PH阈值*100 上限

/*
************************************************************
*	函数名称：	OTA_UrlEncode
*
*	函数功能：	sign需要进行URL编码
*
*	入口参数：	sign：加密结果
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		+			%2B
*				空格		%20
*				/			%2F
*				?			%3F
*				%			%25
*				#			%23
*				&			%26
*				=			%3D
************************************************************
*/
static unsigned char OTA_UrlEncode(char *sign)
{

	char sign_t[40];
	unsigned char i = 0, j = 0;
	unsigned char sign_len = strlen(sign);
	
	if(sign == (void *)0 || sign_len < 28)
		return 1;
	
	for(; i < sign_len; i++)
	{
		sign_t[i] = sign[i];
		sign[i] = 0;
	}
	sign_t[i] = 0;
	
	for(i = 0, j = 0; i < sign_len; i++)
	{
		switch(sign_t[i])
		{
			case '+':
				strcat(sign + j, "%2B");j += 3;
			break;
			
			case ' ':
				strcat(sign + j, "%20");j += 3;
			break;
			
			case '/':
				strcat(sign + j, "%2F");j += 3;
			break;
			
			case '?':
				strcat(sign + j, "%3F");j += 3;
			break;
			
			case '%':
				strcat(sign + j, "%25");j += 3;
			break;
			
			case '#':
				strcat(sign + j, "%23");j += 3;
			break;
			
			case '&':
				strcat(sign + j, "%26");j += 3;
			break;
			
			case '=':
				strcat(sign + j, "%3D");j += 3;
			break;
			
			default:
				sign[j] = sign_t[i];j++;
			break;
		}
	}
	
	sign[j] = 0;
	
	return 0;

}

/*
************************************************************
*	函数名称：	OTA_Authorization
*
*	函数功能：	计算Authorization
*
*	入口参数：	ver：参数组版本号，日期格式，目前仅支持格式"2018-10-31"
*				res：产品id
*				et：过期时间，UTC秒值
*				access_key：访问密钥
*				dev_name：设备名
*				authorization_buf：缓存token的指针
*				authorization_buf_len：缓存区长度(字节)
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		当前仅支持sha1
************************************************************
*/
#define METHOD		"sha1"
static unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	
	size_t olen = 0;
	
	char sign_buf[64];								//保存签名的Base64编码结果 和 URL编码结果
	char hmac_sha1_buf[64];							//保存签名
	char access_key_base64[64];						//保存access_key的Base64编码结合
	char string_for_signature[72];					//保存string_for_signature，这个是加密的key

//----------------------------------------------------参数合法性--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------将access_key进行Base64解码----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
//	printf("access_key_base64: %s\r\n", access_key_base64);
	
//----------------------------------------------------计算string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);
//	printf("string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------加密-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
//	printf("hmac_sha1_buf: %s\r\n", hmac_sha1_buf);
	
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
	OTA_UrlEncode(sign_buf);
//	printf("sign_buf: %s\r\n", sign_buf);
	
//----------------------------------------------------计算Token--------------------------------------------------------------------
	if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
//	printf("Token: %s\r\n", authorization_buf);
	
	return 0;
}

//==========================================================
//	函数名称：	OneNET_RegisterDevice
//
//	函数功能：	在产品中注册一个设备
//
//	入口参数：	access_key：访问密钥
//				pro_id：产品ID
//				serial：唯一设备号
//				devid：保存返回的devid
//				key：保存返回的key
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool OneNET_RegisterDevice(void)
{

	_Bool result = 1;
	unsigned short send_len = 11 + strlen(DEVICE_NAME);
	char *send_ptr = NULL, *data_ptr = NULL;
	
	char authorization_buf[144];													//加密的key
	
	send_ptr = malloc(send_len + 240);
	if(send_ptr == NULL)
		return result;
	
	while(ESP8266_SendCmd("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n", "CONNECT"))
		HAL_Delay(500);
	
	OneNET_Authorization("2018-10-31", PROID, Timestamp, ACCESS_KEY, NULL,
							authorization_buf, sizeof(authorization_buf), 1);
	
	snprintf(send_ptr, 280 + send_len, "POST /mqtt/v1/devices/reg HTTP/1.1\r\n"
					"Authorization:%s\r\n"
					"Host:ota.heclouds.com\r\n"
					"Content-Type:application/json\r\n"
					"Content-Length:%d\r\n\r\n"
					"{\"name\":\"%s\"}",
	
					authorization_buf, 11 + strlen(DEVICE_NAME), DEVICE_NAME);
	
	ESP8266_SendData((unsigned char *)send_ptr, strlen(send_ptr));
	
	/*
	{
	  "request_id" : "f55a5a37-36e4-43a6-905c-cc8f958437b0",
	  "code" : "onenet_common_success",
	  "code_no" : "000000",
	  "message" : null,
	  "data" : {
		"device_id" : "589804481",
		"name" : "mcu_id_43057127",
		
	"pid" : 282932,
		"key" : "indu/peTFlsgQGL060Gp7GhJOn9DnuRecadrybv9/XY="
	  }
	}
	*/
	
	data_ptr = (char *)ESP8266_GetIPD(250);							//等待平台响应
	
	if(data_ptr)
	{
		data_ptr = strstr(data_ptr, "device_id");
	}
	
	if(data_ptr)
	{
		char name[16];
		int pid = 0;
		
		if(sscanf(data_ptr, "device_id\" : \"%[^\"]\",\r\n\"name\" : \"%[^\"]\",\r\n\r\n\"pid\" : %d,\r\n\"key\" : \"%[^\"]\"", devid, name, &pid, key) == 4)
		{
			printf("create device: %s, %s, %d, %s\r\n", devid, name, pid, key);
			result = 0;
		}
	}
	
	free(send_ptr);
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "OK");
	
	return result;
}

//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	char authorization_buf[160];
	
	_Bool status = 1;
	
	OneNET_Authorization("2018-10-31", PROID, Timestamp, ACCESS_KEY, DEVICE_NAME,
								authorization_buf, sizeof(authorization_buf), 0);
	// 这里先注释，到时候在取消
	// printf("OneNET_DevLink\r\n"
	// 						"NAME: %s,	PROID: %s,	KEY:%s\r\n"
    //                     , DEVICE_NAME, PROID, authorization_buf);
	
	if(MQTT_PacketConnect(PROID, authorization_buf, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0://printf("Tips:	连接成功\r\n");
						status = 0;
						break;
					
					case 1://printf("WARN:	连接失败：协议错误\r\n");
					
						break;
					case 2://printf("WARN:	连接失败：非法的clientid\r\n");
					
						break;
					case 3://printf("WARN:	连接失败：服务器失败\r\n");
					
						break;
					case 4://printf("WARN:	连接失败：用户名或密码错误\r\n");
					
						break;
					case 5://printf("WARN:	连接失败：非法链接(比如token非法)\r\n");
					
						break;
					
					default://printf("ERR:	连接失败：未知错误\r\n");
						
						break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
	{
//		//先注释，到时候在取消
//		// printf("WARN:	MQTT_PacketConnect Failed\r\n"); 
	}

	return status;
}

extern uint8_t temp,humi;
extern uint16_t lux, soil_humi, co2_ppm;
extern float soil_ph;


uint16_t OneNet_FillBuf(char *buf)
{	
	char text[50];		// 防止溢出		
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"id\":\"123\",\"params\":{");
	// 温度
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temp\":{\"value\":%d},", temp);
	strcat(buf, text);

	// 温度阈值
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temp_th\":{\"value\":%d},", TH_Temp);
	strcat(buf, text);

	// CO2阈值
	memset(text, 0, sizeof(text));
	sprintf(text, "\"CO2_ppm_th\":{\"value\":%d},", TH_CO2);
	strcat(buf, text);

	// 湿度阈值
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Humi_th\":{\"value\":%d},", TH_Humi);
	strcat(buf, text);

	// 光照阈值
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Light_th\":{\"value\":%d},", TH_Lux);
	strcat(buf, text);

	// 土壤湿度阈值
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Soil_Humi_th\":{\"value\":%d},", TH_SoilHumi);
	strcat(buf, text);

	// 土壤PH上限阈值
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Soil_ph_high_x100\":{\"value\":%d},", TH_SoilPH_High_x100);
	strcat(buf, text);

//	// 土壤PH下限阈值
	memset(text, 0, sizeof(text));
	sprintf(text, "\"x100_soil_ph_low\":{\"value\":%d},", TH_SoilPH_Low_x100);
	strcat(buf, text);

	// 湿度
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Humi\":{\"value\":%d},", humi);
	strcat(buf, text);

	// 光照强度
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Light\":{\"value\":%d},", lux);
	strcat(buf, text);

	// 土壤湿度
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Soil_Humi\":{\"value\":%d},", soil_humi);
	strcat(buf, text);

	// CO2浓度
	memset(text, 0, sizeof(text));
	sprintf(text, "\"CO2_ppm\":{\"value\":%d},", co2_ppm);
	strcat(buf, text);

	// 土壤PH值
	memset(text, 0, sizeof(text));
	snprintf(text, sizeof(text), "\"Soil_ph\":{\"value\":%.2f},", soil_ph);
	strcat(buf, text);

	// 风扇状态
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Fan\":{\"value\":%s},", fan_on_state ? "true" : "false");
	strcat(buf, text);

	// 水雾状态
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Mist\":{\"value\":%s},", spray_on_state ? "true" : "false");
	strcat(buf, text);

	// 水泵状态
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Pump\":{\"value\":%s},", pump_on_state ? "true" : "false");
	strcat(buf, text);

	// BEEP状态
	memset(text, 0, sizeof(text));
	sprintf(text, "\"BEEP\":{\"value\":%s},", beep_on_state ? "true" : "false");
	strcat(buf, text);

	// 因为OneNet格式原因，风扇、水泵、水雾的状态只能放后边，而且最后一个不能有逗号
	memset(text, 0, sizeof(text));
	sprintf(text, "\"LED\":{\"value\":%s}", led_on_state ? "true" : "false");
	strcat(buf, text);

	strcat(buf, "}}");
	
	return strlen(buf);
}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[512];		// 这里要注意堆栈大小，防止溢出
	
	uint16_t body_len = 0, i = 0;
	
//	printf("Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(PROID, DEVICE_NAME, body_len, NULL, &mqttPacket) == 0)				//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
//			printf("Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
		{
			//printf("WARN:	EDP_NewBuffer Failed\r\n");
		}
	}
}

//==========================================================
//	函数名称：	OneNET_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	printf("Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}
}

//==========================================================
//	函数名称：	OneNET_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_Subscribe(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	char topic_buf[58];
	const char *topic = topic_buf;
	
	
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROID, DEVICE_NAME);
	
	// 这里先注释 后续在取消注释
	//printf("Subscribe Topic: %s\r\n", topic_buf);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char type = 0;
	
	short result = 0;

		
	cJSON *raw_json, *params_json;
	cJSON *led_json, *fan_json, *mist_json, *pump_json, *beep_json;
	cJSON *temp_th_json, *humi_th_json, *co2_th_json, *lux_th_json, *soil_humi_th_json, *soil_ph_low_th_json, *soil_ph_high_th_json;

	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_PUBLISH:																//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				// char *data_ptr = NULL;
				
				// 这里先注释，以后用到了在取消注释
				//printf("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
				//													cmdid_topic, topic_len, req_payload, req_len);
					
				raw_json = cJSON_Parse(req_payload);
				if(raw_json == NULL) break;
				params_json = cJSON_GetObjectItem(raw_json, "params");
				if(params_json == NULL) { cJSON_Delete(raw_json); break; }

				// 解析控制命令
				led_json  = cJSON_GetObjectItem(params_json, "LED");
				fan_json  = cJSON_GetObjectItem(params_json, "Fan");
				mist_json = cJSON_GetObjectItem(params_json, "Mist");
				pump_json = cJSON_GetObjectItem(params_json, "Pump");
				beep_json = cJSON_GetObjectItem(params_json, "BEEP");

				// 解析阈值设置
				humi_th_json = cJSON_GetObjectItem(params_json,"Humi_th");
				temp_th_json = cJSON_GetObjectItem(params_json,"Temp_th");
				co2_th_json = cJSON_GetObjectItem(params_json,"CO2_ppm_th");
				lux_th_json = cJSON_GetObjectItem(params_json,"Light_th");
				soil_humi_th_json = cJSON_GetObjectItem(params_json,"Soil_Humi_th");
				soil_ph_low_th_json = cJSON_GetObjectItem(params_json,"x100_soil_ph_low");
				soil_ph_high_th_json = cJSON_GetObjectItem(params_json,"Soil_ph_high_x100");


				/* 只有收到设备控制命令时，才切手动；否则保持自动 */
				if (led_json != NULL || fan_json != NULL || mist_json != NULL ||
					pump_json != NULL || beep_json != NULL)
				{
					control_mode = CONTROL_MODE_MANUAL;
				}
				
				if(humi_th_json != NULL || temp_th_json != NULL || co2_th_json != NULL || lux_th_json != NULL ||
					soil_humi_th_json != NULL || soil_ph_low_th_json != NULL || soil_ph_high_th_json != NULL)
				{
					control_mode = CONTROL_MODE_AUTO;
				}
				
				// 解析LED状态
				if (led_json != NULL)
				{
					if (led_json->type == cJSON_True) led_on_state = 1;
					else led_on_state = 0;
				}
				
				// 解析风扇状态
				if (fan_json != NULL)
				{
					if (fan_json->type == cJSON_True) fan_on_state = 1;
					else fan_on_state = 0;
				}

				// 解析水雾状态
				if (mist_json != NULL)
				{
					if (mist_json->type == cJSON_True) spray_on_state = 1;
					else spray_on_state = 0;
				}

				// 解析水泵状态
				if (pump_json != NULL)
				{
					if (pump_json->type == cJSON_True) pump_on_state = 1;
					else pump_on_state = 0;
				}

				// 解析蜂鸣器状态
				if (beep_json != NULL)
				{
					if (beep_json->type == cJSON_True) beep_on_state = 1;
					else beep_on_state = 0;
				}

				// 解析湿度阈值
				if(humi_th_json != NULL)
				{
					TH_Humi = humi_th_json->valueint;
				}

				// 解析温度阈值
				if(temp_th_json != NULL)
				{
					TH_Temp = temp_th_json->valueint;
				}

				// 解析CO2阈值
				if(co2_th_json != NULL)
				{
					TH_CO2 = co2_th_json->valueint;
				}

				// 解析光照阈值
				if(lux_th_json != NULL)
				{
					TH_Lux = lux_th_json->valueint;
				}

				// 解析土壤湿度阈值
				if(soil_humi_th_json != NULL)
				{
					TH_SoilHumi = soil_humi_th_json->valueint;
				}

				// 解析土壤PH下限阈值
				if(soil_ph_low_th_json != NULL)
				{
					TH_SoilPH_Low_x100 = soil_ph_low_th_json->valueint;
				}

				// 解析土壤PH上限阈值
				if(soil_ph_high_th_json != NULL)
				{
					TH_SoilPH_High_x100 = soil_ph_high_th_json->valueint;
				}

				cJSON_Delete(raw_json);
				
			}
			break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				//先注释，以后有用了在取消注释
				//printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		case MQTT_PKT_SUBACK:																//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				//printf("Tips:	MQTT Subscribe OK\r\n");
			//else
				//printf("Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
