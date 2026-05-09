#include "Menu.h"


// 来自 main.c 的全局变量（用于高亮哪一项）
extern uint8_t OLED_SubMENU;

static const char *MENU_ITEMS[] = {
  "DHT11",
  "Light",
  "Soil",
  "CO2",
  "PH",
  "ESP8266",
  "OneNET",
};

#define MENU_COUNT   ((uint8_t)(sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0])))
#define MENU_VISIBLE 3  // 标题占1行，菜单显示3行（y=16/32/48）

void OLED_DrawMainMenu(void)
{
  uint8_t selected = OLED_SubMENU;
  if (selected >= MENU_COUNT) selected = 0;

  // 计算滚动起始项，让选中项尽量出现在可见区域
  uint8_t start = 0;
  if (MENU_COUNT > MENU_VISIBLE)
  {
    if (selected < MENU_VISIBLE) start = 0;
    else start = (uint8_t)(selected - (MENU_VISIBLE - 1));

    if (start > (uint8_t)(MENU_COUNT - MENU_VISIBLE))
      start = (uint8_t)(MENU_COUNT - MENU_VISIBLE);
  }

  OLED_Clear();

  // 标题行
  OLED_ShowString(0, 0, (uint8_t *)"DaPeng MENU", 16, 1);

  // 菜单三行（y=16/32/48），选中项反色 mode=0
  for (uint8_t i = 0; i < MENU_VISIBLE; i++)
  {
    uint8_t idx = (uint8_t)(start + i);
    if (idx >= MENU_COUNT) break;

    uint8_t y = (uint8_t)(16 * (i + 1));
    uint8_t mode = (idx == selected) ? 0 : 1;

    OLED_ShowString(0, y, (uint8_t *)MENU_ITEMS[idx], 16, mode);
  }
}
