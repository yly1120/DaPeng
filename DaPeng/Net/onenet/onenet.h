#ifndef _ONENET_H_
#define _ONENET_H_


#include "mqttkit.h"
#include "base64.h"
#include "hmac_sha1.h"
#include "usart.h"
#include <string.h>
#include "cJSON.h"
#include "main.h"
#include "esp8266.h"


_Bool OneNET_RegisterDevice(void);

_Bool OneNet_DevLink(void);

void OneNet_SendData(void);

void OneNET_Subscribe(void);

void OneNet_RevPro(unsigned char *cmd);

#endif
