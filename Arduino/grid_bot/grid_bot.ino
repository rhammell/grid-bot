#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TouchScreen.h"


const uint16_t UNDO_ICON_COLOR[] PROGMEM = {
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71, 0xB596, 0xCE79, 0xE71C, 0xF7BE, 0xF7BE, 0xE71C, 0xCE59, 0xAD75,  // 0x0010 (16) pixels
  0x8C51, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xDEDB, 0xE73C, 0x8C71, 0x7BCF, 0x7BCF, 0x7BEF, 0xC638, 0xFFDF,  // 0x0020 (32) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xC638, 0x7BEF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0030 (48) pixels
  0xFFFF, 0xFFFF, 0xF79E, 0x8C71, 0x9CF3, 0xEF7D, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0040 (64) pixels
  0xFFFF, 0xFFFF, 0xEF5D, 0x9CF3, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0050 (80) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xBDF7, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0060 (96) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0xCE59, 0xAD75, 0x8C71, 0x8C71, 0xB596, 0xCE79, 0xF79E,  // 0x0070 (112) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x9CF3, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xD69A,  // 0x0080 (128) pixels
  0x8430, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8430, 0xD69A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xEF7D, 0x7BEF, 0x7BCF,  // 0x0090 (144) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF79E, 0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x00A0 (160) pixels
  0x7BCF, 0xB596, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xCE59, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xEF7D,  // 0x00B0 (176) pixels
  0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xD69A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8C71,  // 0x00C0 (192) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xE73C, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x00D0 (208) pixels
  0x7BCF, 0x7BCF, 0x8430, 0xF79E, 0xFFFF, 0xFFFF, 0xFFFF, 0xB596, 0xE71C, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x00E0 (224) pixels
  0xDEDB, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xCE79, 0xFFFF, 0xFFFF, 0xFFFF, 0xCE59,  // 0x00F0 (240) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0100 (256) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0xB596, 0xFFFF, 0xFFFF, 0xFFFF, 0xE71C, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0110 (272) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE,  // 0x0120 (288) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0130 (304) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x8C71, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0140 (320) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xB596, 0xFFFF, 0xFFFF, 0xFFFF, 0xE71C,  // 0x0150 (336) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0160 (352) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0xCE79, 0xFFFF, 0xFFFF, 0xFFFF, 0xCE59, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0170 (368) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8430, 0xF79E, 0xFFFF, 0xFFFF, 0xFFFF, 0xB596,  // 0x0180 (384) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BEF, 0x9CF3, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0190 (400) pixels
  0x7BCF, 0x7BCF, 0xD69A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BEF, 0xE71C, 0xFFFF, 0xC638, 0x7BCF,  // 0x01A0 (416) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xB596, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xCE59, 0x7BCF,  // 0x01B0 (432) pixels
  0x7BCF, 0x7BCF, 0x7BEF, 0xE71C, 0xFFFF, 0xFFFF, 0xFFFF, 0xDEDB, 0x8410, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8430,  // 0x01C0 (448) pixels
  0xD69A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xEF7D, 0x7BEF, 0x7BCF, 0x7BCF, 0x7BCF, 0xA514, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x01D0 (464) pixels
  0xF79E, 0xD6BA, 0xBDD7, 0x9492, 0x8C71, 0xB596, 0xCE79, 0xF79E, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x9CF3, 0x7BCF, 0x7BCF,  // 0x01E0 (480) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0xBDD7, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x01F0 (496) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xBDF7, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xB5B6, 0xFFDF, 0xFFFF, 0xFFFF,  // 0x0200 (512) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xEF5D, 0x9CF3, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0210 (528) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71, 0xDEDB, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0220 (544) pixels
  0xFFFF, 0xC638, 0x7BEF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71,  // 0x0230 (560) pixels
  0xC618, 0xE73C, 0xF79E, 0xFFDF, 0xF7BE, 0xE71C, 0xCE59, 0xAD75, 0x8C51, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0240 (576) pixels
};


const uint16_t SETTINGS_ICON_COLOR[] PROGMEM = {
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x9CD3, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0x9CD3,  // 0x0010 (16) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0020 (32) pixels
  0xC618, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xC638, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0030 (48) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xD6BA, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xD6BA,  // 0x0040 (64) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8410, 0xC618, 0xAD75, 0x7BEF, 0x7BCF, 0x8410,  // 0x0050 (80) pixels
  0xEF5D, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xEF5D, 0x8410, 0x7BCF, 0x7BEF, 0xAD75, 0xC618, 0x7BEF, 0x7BCF, 0x7BCF,  // 0x0060 (96) pixels
  0x7BCF, 0x7BCF, 0xD69A, 0xFFFF, 0xFFFF, 0xF7BE, 0xCE79, 0xF79E, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0070 (112) pixels
  0xF79E, 0xCE79, 0xF7BE, 0xFFFF, 0xFFFF, 0xCE79, 0x7BCF, 0x7BCF, 0x7BCF, 0x94B2, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0080 (128) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x94B2, 0x7BCF,  // 0x0090 (144) pixels
  0x7BCF, 0xE71C, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x00A0 (160) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xDEFB, 0x7BCF, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x00B0 (176) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55,  // 0x00C0 (192) pixels
  0xD6BA, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0xBDD7, 0x8C71, 0x8C71, 0xB5B6, 0xF7BE, 0xFFFF,  // 0x00D0 (208) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xD6BA, 0x9CF3, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x00E0 (224) pixels
  0xF7BE, 0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C51, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0x9CF3,  // 0x00F0 (240) pixels
  0x7BCF, 0x8C71, 0xE73C, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xBDD7, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xB5B6,  // 0x0100 (256) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xE73C, 0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0110 (272) pixels
  0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8C71, 0x7BCF, 0x7BCF,  // 0x0120 (288) pixels
  0x7BCF, 0x7BCF, 0x8C71, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71,  // 0x0130 (304) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C71, 0xE73C, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0140 (320) pixels
  0xBDD7, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xB5B6, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xE73C, 0x8C71, 0x7BCF,  // 0x0150 (336) pixels
  0x9CF3, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0x8C71, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x8C51, 0xF7BE,  // 0x0160 (352) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0x9CF3, 0xD6BA, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0170 (368) pixels
  0xFFFF, 0xF7BE, 0xBDD7, 0x8C71, 0x8C71, 0xB5B6, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xD6BA,  // 0x0180 (384) pixels
  0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x0190 (400) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD75, 0x7BCF, 0xDEFB, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x01A0 (416) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xE71C, 0x7BCF,  // 0x01B0 (432) pixels
  0x7BCF, 0x94B2, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x01C0 (448) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x94B2, 0x7BCF, 0x7BCF, 0x7BCF, 0xCE79, 0xFFFF, 0xFFFF, 0xF7BE, 0xCE79, 0xF79E,  // 0x01D0 (464) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF79E, 0xCE79, 0xF7BE, 0xFFFF, 0xFFFF, 0xD69A, 0x7BCF, 0x7BCF,  // 0x01E0 (480) pixels
  0x7BCF, 0x7BCF, 0x7BEF, 0xC618, 0xB596, 0x7BEF, 0x7BCF, 0x8410, 0xEF5D, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xEF5D,  // 0x01F0 (496) pixels
  0x8410, 0x7BCF, 0x7BEF, 0xAD75, 0xBDF7, 0x7BEF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0200 (512) pixels
  0xD6BA, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xD6BA, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0210 (528) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xC618, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xC638,  // 0x0220 (544) pixels
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0230 (560) pixels
  0x94B2, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0x9CD3, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF,  // 0x0240 (576) pixels
};

// TFT Pins
#define TFT_CS 17
#define TFT_DC 18
#define TFT_RST 19
#define TFT_LED 20

// TFT display object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
///Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

// Touchscreen Pins
#define YP A4  // must be an analog pin, use "An" notation!
#define XP 22  // can be any digital pin
#define YM 23  // can be any digital pin
#define XM A7  // must be an analog pin, use "An" notation!

// Touchscreen object
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Touch screen calibration values
#define TOUCH_MIN_X 790
#define TOUCH_MAX_X 993
#define TOUCH_MIN_Y 785
#define TOUCH_MAX_Y 992

// Display brightness percent
int display_brightness = 60;

// Screen dimensions
int screen_width;
int screen_height;

// Grid cell size
int cell_size = 20;

// Grid dimensions and offsets
int num_rows;
int num_cols;
int grid_width;
int grid_height;
int x_offset;
int y_offset;

// Grid display colors
uint16_t background_color = ILI9341_BLACK;
uint16_t selected_color = ILI9341_BLACK;
uint16_t empty_color = ILI9341_WHITE;
uint16_t grid_color = ILI9341_DARKGREY;
uint16_t selectable_color = tft.color565(75, 225, 75);
uint16_t arrow_color = ILI9341_WHITE;

// Init grid values array
int** grid_vals;

// Path cell structure
struct PathCell {
  int row;
  int col;
};

// Initi path cell array and length
PathCell* path;
int path_length = 0;

// Button dimension values
int button_height = 36;
int button_margin = 2;

// Start button position params
int start_button_width;
int start_button_x;
int start_button_y;

// Undo button position params
int undo_button_width = 36;
int undo_button_x;
int undo_button_y;

// Settings button position params
int settings_button_width = 36;
int settings_button_x;
int settings_button_y;

// Button display colors
int button_idle_color = tft.color565(75, 75, 225);
int button_counting_color = tft.color565(240, 124, 36);
int button_running_color = tft.color565(255, 75, 75);
int button_complete_color = tft.color565(75, 255, 75);
int button_text_color = ILI9341_WHITE;

// Settings menu position params
int settings_menu_width;
int settings_menu_height;
int settings_menu_x;
int settings_menu_y;

// Settings menu arrow params
int settings_arrow_width = 20;
int settings_arrow_height = 30;
int settings_arrow_padding = 10;
int settings_arrow_xmargin = 14;

// Brightness arrow position params
int brightness_value_x;
int brightness_value_y;
int brightness_value_width;
int brightness_left_arrow_x;
int brightness_right_arrow_x;
int brightness_arrow_y;

// Size arrow position params
int distance_value_x;
int distance_value_y;
int distance_value_width;
int distance_left_arrow_x;
int distance_right_arrow_x;
int distance_arrow_y;

// Speed arrow position params
int speed_value_x;
int speed_value_y;
int speed_value_width;
int speed_left_arrow_x;
int speed_right_arrow_x;
int speed_arrow_y;

// States
enum UIState {
  IDLE,
  COUNTING,
  RUNNING,
  SETTINGS,
  COMPLETE
};

// Set current state
UIState ui_state = IDLE;

// Driving state enum
enum DriveState {
  STOPPED,
  TURNING,
  DRIVING
};

// Set current state
DriveState drive_state = STOPPED;

// Drive speed enum
enum DriveSpeed {
  SPEED_SLOW,
  SPEED_STANDARD,
  SPEED_FAST
};

// Set current drive speed
DriveSpeed drive_speed = SPEED_STANDARD;

// Drive speed labels
const char* DRIVE_SPEED_LABELS[] = { "Slow", "Standard", "Fast" };

// Grid size enum
enum DriveDistance {
  DISTANCE_COMPACT,
  DISTANCE_STANDARD,
  DISTANCE_EXTENDED
};

// Set current grid size
DriveDistance drive_distance = DISTANCE_STANDARD;

// Grid size labels
const char* DRIVE_DISTANCE_LABELS[] = { "Compact", "Standard", "Extended" };

// Defound countdown state
unsigned long countdown_start = 0;
const int countdown_duration = 5000;
int countdown_number = -1;

// Color scheme for settings menu
uint16_t settings_menu_background_color = ILI9341_DARKGREY;
uint16_t settings_menu_text_color = ILI9341_WHITE;
uint16_t settings_menu_button_color = tft.color565(100, 100, 100);

// Movement timing constants
const unsigned long FORWARD_MOVE_TIME = 750;  // Time to move forward one cell
const unsigned long TURN_MOVE_TIME = 500;     // Time to execute a 90-degree turn

// Current movement tracking
int current_path_index = 0;     // Current position in path array
int current_direction = 0;      // 0=up, 1=right, 2=down, 3=left
unsigned long move_start_time;  // Start time of current movement
bool is_turning = false;        // Whether bot is currently executing a turn

void setup() {
  Serial.begin(9600);

  // Set ADC resolution for touchsceen
  analogReadResolution(10);

  // Configure display LED pin
  pinMode(TFT_LED, OUTPUT);

  // Set display brightness
  setBrightness();

  // start by disabling both SD and TFT
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  // Initialize display object
  tft.begin();
  tft.setRotation(0);

  // Fill screen with black background
  tft.fillScreen(background_color);

  // Dynamically get screen dimensions
  screen_width = tft.width();
  screen_height = tft.height();

  // Initialize grid values
  initGrid();

  // Initialize path array with default points
  initPath();

  // Draw UI elements
  drawUserInterface();
}

void setBrightness() {
  // Ensure global brightness is in valid range
  display_brightness = constrain(display_brightness, 0, 100);

  // Convert percentage (0-100) to PWM value (0-255)
  int pwm_value = map(display_brightness, 0, 100, 0, 255);

  // Set PWM output
  analogWrite(TFT_LED, pwm_value);
}

void initGrid() {

  // Calculate grid size, dimensions, and offsets
  int available_height = screen_height - button_height - 1 - button_margin;
  num_rows = (available_height - 1) / cell_size;
  num_cols = ((screen_width - 1) / cell_size) - (((screen_width - 1) / cell_size) % 2 == 0 ? 1 : 0);
  grid_width = cell_size * num_cols;
  grid_height = cell_size * num_rows;
  x_offset = (screen_width - grid_width - 1) / 2;
  y_offset = (screen_height - grid_height - 1 - button_height - button_margin) / 2;

  // Initialized 2D grid of cell values
  grid_vals = new int*[num_rows];
  for (int i = 0; i < num_rows; i++) {
    grid_vals[i] = new int[num_cols];
  }

  // Reset all grid values to 0
  resetGridValues();
}

void resetGridValues() {
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      grid_vals[i][j] = 0;
    }
  }
}

void initPath() {
  // Inititilze array of path points
  path = new PathCell[num_rows * num_cols];

  // Set two initial default path points
  resetDefaultPath();
}

void resetDefaultPath() {
  // Set path length to zero
  path_length = 0;

  // Add to points at center bottom of grid
  int start_row = num_rows - 1;
  int start_col = num_cols / 2;
  pathAdd(start_row, start_col);
  pathAdd(start_row - 1, start_col);
}

void pathAdd(int row, int col) {
  // Add coordinate to the list of path points
  path[path_length] = { row, col };
  path_length += 1;

  // Update the grid value of point
  grid_vals[row][col] = 1;
}

bool isSelectable(int row, int col) {
  // If cell is already selected, it's not selectable
  if (grid_vals[row][col] == 1) {
    return false;
  }

  // Get the last point in the path
  PathCell last_point = path[path_length - 1];

  // Check if cell is adjacent (not diagonal) to last point
  bool is_adjacent = (
    // Check horizontal adjacency
    (row == last_point.row && abs(col - last_point.col) == 1) ||
    // Check vertical adjacency
    (col == last_point.col && abs(row - last_point.row) == 1));

  return is_adjacent;
}


void drawUserInterface() {
  // Draw all UI elements
  drawGridLines();
  drawGridCells(0, num_rows - 1, 0, num_cols - 1);
  drawUndoButton();
  drawStartButton();
  drawSettingsButton();
}

void drawGridLines() {
  // Draw horizontal grid lines
  for (int i = 0; i < num_rows + 1; i++) {
    tft.drawLine(
      x_offset,
      i * cell_size + y_offset,
      x_offset + grid_width,
      i * cell_size + y_offset,
      grid_color);
  }

  // Draw verticalgrid lines
  for (int i = 0; i < num_cols + 1; i++) {
    tft.drawLine(
      i * cell_size + x_offset,
      y_offset,
      i * cell_size + x_offset,
      y_offset + grid_height,
      grid_color);
  }
}


void drawGridCells(int startRow, int endRow, int startCol, int endCol) {
  // Clamp values to grid bounds
  startRow = max(0, min(startRow, num_rows - 1));
  endRow = max(0, min(endRow, num_rows - 1));
  startCol = max(0, min(startCol, num_cols - 1));
  endCol = max(0, min(endCol, num_cols - 1));

  // Loop through row and col values
  for (int i = startRow; i <= endRow; i++) {
    for (int j = startCol; j <= endCol; j++) {
      Serial.println("looping");

      // Determine cell color
      int color;

      // Check if cell is activated
      if (grid_vals[i][j] == 1) {

        if (ui_state == RUNNING || ui_state == COMPLETE) {
          for (int p = 0; p <= current_path_index; p++) {
            if (i == path[p].row && j == path[p].col) {
              color = selectable_color;
              break;
            }
          }
        } else {
          color = selected_color;
        }
      }
      // UI is
      else if (ui_state == IDLE && isSelectable(i, j)) {
        color = selectable_color;  // Green for selectable cells
      }
      //
      else {
        color = empty_color;
      }


      // Draw cell rect
      tft.fillRect(
        j * cell_size + x_offset + 1,
        i * cell_size + y_offset + 1,
        cell_size - 1,
        cell_size - 1,
        color);

      // Loop through path point indexes
      for (int p = 0; p < path_length; p++) {

        // Check if current cell is equal to path point
        if (i == path[p].row && j == path[p].col) {

          // Current and next path point
          PathCell currentPoint = path[p];
          PathCell nextPoint;

          // If only one path point, define next point to draw up arrow
          if (path_length == 1) {
            nextPoint = { currentPoint.row - 1, currentPoint.col };
          }
          // If last path point, use current point to draw circle
          else if (p == path_length - 1) {
            nextPoint = currentPoint;
          }
          // If any other path point, use next point in path
          else {
            nextPoint = path[p + 1];
          }

          // Draw cell arrow using current and next
          drawCellDirection(currentPoint.row, currentPoint.col, nextPoint.row, nextPoint.col);
        }
      }
    }
  }
}

void drawCellDirection(int row, int col, int next_row, int next_col) {
  // Calculate cell center
  int center_x = col * cell_size + x_offset + (cell_size / 2);
  int center_y = row * cell_size + y_offset + (cell_size / 2);

  // If last cell in path draw circle
  if (row == path[path_length - 1].row && col == path[path_length - 1].col) {
    int circle_radius = 3;
    tft.fillCircle(center_x, center_y, circle_radius, arrow_color);
    return;
  }

  // Calculate direction - reversed from before
  int dx = next_col - col;  // Direction is next minus current
  int dy = next_row - row;

  // Draw small filled triangle pointing in direction of movement
  if (dx > 0) {  // Moving right
    tft.fillTriangle(
      center_x - 3, center_y - 3,
      center_x - 3, center_y + 3,
      center_x + 3, center_y,
      arrow_color);
  } else if (dx < 0) {  // Moving left
    tft.fillTriangle(
      center_x + 3, center_y - 3,
      center_x + 3, center_y + 3,
      center_x - 3, center_y,
      arrow_color);
  } else if (dy > 0) {  // Moving down
    tft.fillTriangle(
      center_x - 3, center_y - 3,
      center_x + 3, center_y - 3,
      center_x, center_y + 3,
      arrow_color);
  } else if (dy < 0) {  // Moving up
    tft.fillTriangle(
      center_x - 3, center_y + 3,
      center_x + 3, center_y + 3,
      center_x, center_y - 3,
      arrow_color);
  }
}

void drawUndoButton() {
  // Calculate button x/y start
  undo_button_y = y_offset + grid_height + button_margin + 1;
  undo_button_x = x_offset;

  // Draw button background
  tft.fillRect(undo_button_x, undo_button_y, undo_button_width, button_height, ILI9341_DARKGREY);

  // Calculate center position for icon
  int icon_x = undo_button_x + (undo_button_width - 24) / 2;
  int icon_y = undo_button_y + (button_height - 24) / 2;

  // Draw the icon
  tft.drawRGBBitmap(icon_x, icon_y, UNDO_ICON_COLOR, 24, 24);
}

void drawStartButton() {
  // Calculate button x/y start
  start_button_y = y_offset + grid_height + button_margin + 1;
  start_button_x = undo_button_x + undo_button_width + button_margin;

  // Calculate start button width
  start_button_width = grid_width - undo_button_width - button_margin - settings_button_width - button_margin + 1;

  // Init color and text
  uint16_t current_color;
  String button_text;

  switch (ui_state) {
    case COUNTING:
      {
        current_color = button_counting_color;
        button_text = String(countdown_number);
        break;
      }

    case RUNNING:
      current_color = button_running_color;
      button_text = "Stop";
      break;

    case COMPLETE:
      current_color = button_complete_color;
      button_text = "Done!";
      break;

    case IDLE:
    default:
      current_color = button_idle_color;
      button_text = "Start";
      break;
  }

  // Draw button background
  tft.fillRect(start_button_x, start_button_y, start_button_width, button_height, current_color);

  // Set text properties
  tft.setTextColor(button_text_color);
  tft.setTextSize(2);

  // Calculate text width (6 pixels per character at size 2)
  int char_width = 6 * 2;  // Each character is 6 pixels * text size 2
  int text_width = button_text.length() * char_width;

  // Center text horizontally and vertically
  int text_x = x_offset + (grid_width - text_width) / 2;
  int text_y = start_button_y + (button_height - 16) / 2;  // 16 is character height at size 2

  // Draw text
  tft.setCursor(text_x, text_y);
  tft.print(button_text);
}

void drawSettingsButton() {
  // Calculate button x/y start
  settings_button_y = y_offset + grid_height + button_margin + 1;
  settings_button_x = start_button_x + start_button_width + button_margin;

  // Draw button background
  tft.fillRect(settings_button_x, settings_button_y, settings_button_width, button_height, ILI9341_DARKGREY);

  // Calculate center position for icon
  int icon_x = settings_button_x + (settings_button_width - 24) / 2;  // Center 20px wide icon
  int icon_y = settings_button_y + (button_height - 24) / 2;          // Center 20px tall icon

  // Draw the icon
  tft.drawRGBBitmap(icon_x, icon_y, SETTINGS_ICON_COLOR, 24, 24);
}

void drawSettingsMenu() {
  // Calculate menu dimensions and position
  settings_menu_width = grid_width * 0.85;
  settings_menu_height = 225;
  settings_menu_x = x_offset + (grid_width - settings_menu_width) / 2;
  settings_menu_y = y_offset + (grid_height - settings_menu_height) / 2;

  // Draw menu background
  tft.fillRect(settings_menu_x, settings_menu_y, settings_menu_width, settings_menu_height, settings_menu_background_color);
  tft.drawRect(settings_menu_x, settings_menu_y, settings_menu_width, settings_menu_height, ILI9341_WHITE);

  // Set text size and color
  tft.setTextSize(2);
  tft.setTextColor(settings_menu_text_color);
  int char_width = 12;

  // Draw brightness setting label
  String brightness_label = "Brightness";
  int brightness_label_x = settings_menu_x + (settings_menu_width - brightness_label.length() * char_width) / 2;
  int brightness_label_y = settings_menu_y + 20;
  tft.setCursor(brightness_label_x, brightness_label_y);
  tft.print(brightness_label);

  // Calculate brightness arrow positions
  brightness_left_arrow_x = settings_menu_x + settings_arrow_xmargin;
  brightness_right_arrow_x = settings_menu_x + settings_menu_width - settings_arrow_xmargin - settings_arrow_width;
  brightness_arrow_y = brightness_label_y + 20;

  // Draw left arrow button
  tft.fillTriangle(
    brightness_left_arrow_x, brightness_arrow_y + settings_arrow_height / 2,
    brightness_left_arrow_x + settings_arrow_width, brightness_arrow_y + settings_arrow_height / 4,
    brightness_left_arrow_x + settings_arrow_width, brightness_arrow_y + (3 * settings_arrow_height) / 4,
    settings_menu_text_color);

  // Draw right arrow button
  tft.fillTriangle(
    brightness_right_arrow_x + settings_arrow_width, brightness_arrow_y + settings_arrow_height / 2,
    brightness_right_arrow_x, brightness_arrow_y + settings_arrow_height / 4,
    brightness_right_arrow_x, brightness_arrow_y + (3 * settings_arrow_height) / 4,
    settings_menu_text_color);

  // Calculate and store brightness value position
  int max_brightness_chars = 4;
  brightness_value_y = brightness_arrow_y + (settings_arrow_height - 16) / 2;
  brightness_value_width = max_brightness_chars * char_width;
  brightness_value_x = settings_menu_x + (settings_menu_width - brightness_value_width) / 2 + 2;

  // Draw brightness value
  updateBrightnessDisplay();

  // Draw speed setting label
  String speed_label = "Drive Speed";
  int speed_label_x = settings_menu_x + (settings_menu_width - speed_label.length() * char_width) / 2;
  int speed_label_y = brightness_label_y + 70;
  tft.setCursor(speed_label_x, speed_label_y);
  tft.print(speed_label);

  // Calculate size arrow positions
  speed_left_arrow_x = settings_menu_x + settings_arrow_xmargin;
  speed_right_arrow_x = settings_menu_x + settings_menu_width - settings_arrow_xmargin - settings_arrow_width;
  speed_arrow_y = speed_label_y + 20;

  // Draw left arrow button
  tft.fillTriangle(
    speed_left_arrow_x, speed_arrow_y + settings_arrow_height / 2,
    speed_left_arrow_x + settings_arrow_width, speed_arrow_y + settings_arrow_height / 4,
    speed_left_arrow_x + settings_arrow_width, speed_arrow_y + (3 * settings_arrow_height) / 4,
    settings_menu_text_color);

  // Draw right arrow button
  tft.fillTriangle(
    speed_right_arrow_x + settings_arrow_width, speed_arrow_y + settings_arrow_height / 2,
    speed_right_arrow_x, speed_arrow_y + settings_arrow_height / 4,
    speed_right_arrow_x, speed_arrow_y + (3 * settings_arrow_height) / 4,
    settings_menu_text_color);

  // Calculate and store speed value position
  int max_speed_chars = 8;  // "Standard" is the longest label
  speed_value_y = speed_arrow_y + (settings_arrow_height - 16) / 2;
  speed_value_width = max_speed_chars * char_width;
  speed_value_x = settings_menu_x + (settings_menu_width - speed_value_width) / 2 + 2;

  // Draw initial speed value
  updateDriveSpeedDisplay();

  // Draw grid size setting label
  String distance_label = "Drive Distance";
  int distance_label_x = settings_menu_x + (settings_menu_width - distance_label.length() * char_width) / 2;
  int distance_label_y = speed_label_y + 70;
  tft.setCursor(distance_label_x, distance_label_y);
  tft.print(distance_label);

  // Calculate size arrow positions
  distance_left_arrow_x = settings_menu_x + settings_arrow_xmargin;
  distance_right_arrow_x = settings_menu_x + settings_menu_width - settings_arrow_xmargin - settings_arrow_width;
  distance_arrow_y = distance_label_y + 20;

  // Draw left arrow button
  tft.fillTriangle(
    distance_left_arrow_x, distance_arrow_y + settings_arrow_height / 2,
    distance_left_arrow_x + settings_arrow_width, distance_arrow_y + settings_arrow_height / 4,
    distance_left_arrow_x + settings_arrow_width, distance_arrow_y + (3 * settings_arrow_height) / 4,
    settings_menu_text_color);

  // Draw right arrow button
  tft.fillTriangle(
    distance_right_arrow_x + settings_arrow_width, distance_arrow_y + settings_arrow_height / 2,
    distance_right_arrow_x, distance_arrow_y + settings_arrow_height / 4,
    distance_right_arrow_x, distance_arrow_y + (3 * settings_arrow_height) / 4,
    settings_menu_text_color);

  // Calculate and store size value position
  int max_size_chars = 8;  // "Standard" is the longest label
  distance_value_y = distance_arrow_y + (settings_arrow_height - 16) / 2;
  distance_value_width = max_size_chars * char_width;
  distance_value_x = settings_menu_x + (settings_menu_width - distance_value_width) / 2 + 2;

  // Draw initial size value
  updateDriveDistanceDisplay();
}

void updateBrightnessDisplay() {
  // Clear the entire value area
  tft.fillRect(
    brightness_value_x,
    brightness_value_y,
    brightness_value_width,
    16,
    settings_menu_background_color);

  // Set text size
  tft.setTextSize(2);
  int char_width = 12;

  // Calculate width of current value text
  String brightness_text = String(display_brightness) + "%";
  int actual_text_width = brightness_text.length() * char_width;

  // Calculate centered position for current text
  int text_x = settings_menu_x + (settings_menu_width - actual_text_width) / 2 + 2;
  tft.setTextColor(settings_menu_text_color);
  tft.setCursor(text_x, brightness_value_y);
  tft.print(brightness_text);
}

void updateDriveSpeedDisplay() {
  // Clear the entire value area
  tft.fillRect(
    speed_value_x,
    speed_value_y,
    speed_value_width,
    16,
    settings_menu_background_color);

  // Set text size
  tft.setTextSize(2);
  int char_width = 12;

  // Get current speed label
  String speed_text = DRIVE_SPEED_LABELS[drive_speed];
  int actual_text_width = speed_text.length() * char_width;

  // Calculate centered position for current text
  int text_x = settings_menu_x + (settings_menu_width - actual_text_width) / 2 + 2;
  tft.setTextColor(settings_menu_text_color);
  tft.setCursor(text_x, speed_value_y);
  tft.print(speed_text);
}

void updateDriveDistanceDisplay() {
  // Clear the entire value area
  tft.fillRect(
    distance_value_x,
    distance_value_y,
    distance_value_width,
    16,
    settings_menu_background_color);

  // Set text size
  tft.setTextSize(2);
  int char_width = 12;

  // Get current size label
  String distance_text = DRIVE_DISTANCE_LABELS[drive_distance];
  int actual_text_width = distance_text.length() * char_width;

  // Calculate centered position for current text
  int text_x = settings_menu_x + (settings_menu_width - actual_text_width) / 2 + 2;
  tft.setTextColor(settings_menu_text_color);
  tft.setCursor(text_x, distance_value_y);
  tft.print(distance_text);
}

bool isTouchInUndoButton(int x, int y) {
  return x >= undo_button_x && x <= undo_button_x + undo_button_width && y >= undo_button_y && y <= undo_button_y + button_height;
}

bool isTouchInSettingsButton(int x, int y) {
  return x >= settings_button_x && x <= settings_button_x + settings_button_width && y >= settings_button_y && y <= settings_button_y + button_height;
}

bool isTouchInStartButton(int x, int y) {
  return x >= start_button_x && x <= start_button_x + start_button_width && y >= start_button_y && y <= start_button_y + button_height;
}

bool isTouchInGrid(int x, int y) {
  return x >= x_offset && x < x_offset + grid_width && y >= y_offset && y < y_offset + grid_height;
}

bool isTouchInBrightnessLeftArrow(int x, int y) {
  return x >= brightness_left_arrow_x - settings_arrow_padding && x <= brightness_left_arrow_x + settings_arrow_width + settings_arrow_padding && y >= brightness_arrow_y - settings_arrow_padding && y <= brightness_arrow_y + settings_arrow_height + settings_arrow_padding;
}

bool isTouchInBrightnessRightArrow(int x, int y) {
  return x >= brightness_right_arrow_x - settings_arrow_padding && x <= brightness_right_arrow_x + settings_arrow_width + settings_arrow_padding && y >= brightness_arrow_y - settings_arrow_padding && y <= brightness_arrow_y + settings_arrow_height + settings_arrow_padding;
}

bool isTouchInSizeLeftArrow(int x, int y) {
  return x >= distance_left_arrow_x - settings_arrow_padding && x <= distance_left_arrow_x + settings_arrow_width + settings_arrow_padding && y >= distance_arrow_y - settings_arrow_padding && y <= distance_arrow_y + settings_arrow_height + settings_arrow_padding;
}

bool isTouchInSizeRightArrow(int x, int y) {
  return x >= distance_right_arrow_x - settings_arrow_padding && x <= distance_right_arrow_x + settings_arrow_width + settings_arrow_padding && y >= distance_arrow_y - settings_arrow_padding && y <= distance_arrow_y + settings_arrow_height + settings_arrow_padding;
}

bool isTouchInSpeedLeftArrow(int x, int y) {
  return x >= speed_left_arrow_x - settings_arrow_padding && x <= speed_left_arrow_x + settings_arrow_width + settings_arrow_padding && y >= speed_arrow_y - settings_arrow_padding && y <= speed_arrow_y + settings_arrow_height + settings_arrow_padding;
}

bool isTouchInSpeedRightArrow(int x, int y) {
  return x >= speed_right_arrow_x - settings_arrow_padding && x <= speed_right_arrow_x + settings_arrow_width + settings_arrow_padding && y >= speed_arrow_y - settings_arrow_padding && y <= speed_arrow_y + settings_arrow_height + settings_arrow_padding;
}

// Function to calculate direction between two points
int getDirection(int start_row, int start_col, int end_row, int end_col) {
  if (end_row < start_row) return 0;  // Up
  if (end_col > start_col) return 1;  // Right
  if (end_row > start_row) return 2;  // Down
  if (end_col < start_col) return 3;  // Left
  return current_direction;           // No change
}

// Function to calculate required turn
int calculateTurn(int current_dir, int target_dir) {
  int diff = target_dir - current_dir;

  // Normalize to -1 (left) or 1 (right)
  if (diff == 3) diff = -1;
  if (diff == -3) diff = 1;

  return diff;
}

// Function to print current cell information
void printCurrentCell(PathCell current, PathCell next) {
}

// Function to execute next movement
void executeMovement() {

  // Get current and next points
  PathCell current = path[current_path_index];
  PathCell next = path[current_path_index + 1];

  // Calculate target direction
  int target_direction = getDirection(current.row, current.col, next.row, next.col);

  switch (drive_state) {

    // Stopped driving state
    case STOPPED:
      {

        // Print current cell being processed
        printCurrentCell(current, next);

        // Redraw current cell
        drawGridCells(current.row, current.row, current.col, current.col);

        // Check if aligned to target diretion
        if (current_direction == target_direction) {

          // Set driving flags
          drive_state = DRIVING;
          move_start_time = millis();

          // Here you would add actual motor control:
          // driveForward();

          Serial.println("Driving");
        }
        // Not aligned to target direction
        else {

          // Set turing flags
          drive_state = TURNING;
          move_start_time = millis();

          // Get turn direction
          int turn = calculateTurn(current_direction, target_direction);

          // Here you would add actual motor control:
          // if (turn < 0) turnLeft();
          // if (turn > 0) turnRight();

          Serial.print("Turning ");
          Serial.println(turn < 0 ? "left" : "right");
        }
        break;
      }

    // Driving state
    case DRIVING:
      {

        // Check if driving movement is complete
        if (millis() - move_start_time >= FORWARD_MOVE_TIME) {

          // Update current path point
          current_path_index++;
          move_start_time = millis();  // Reset timer for next movement

          // Check if current path point is the last
          if (current_path_index >= path_length - 1) {

            // Here you would stop motors:
            // stopMotors();
            Serial.println("Path complete");

            // Redraw current cell
            drawGridCells(next.row, next.row, next.col, next.col);

            // Update UI and driving state
            ui_state = COMPLETE;
            drive_state = STOPPED;
            drawStartButton();
          }

          // Current path point is not the last
          else {

            // Calculate next direction
            current = path[current_path_index];
            next = path[current_path_index + 1];
            int next_direction = getDirection(current.row, current.col, next.row, next.col);

            // Print current cell being processed
            printCurrentCell(current, next);

            // Redraw current cell
            drawGridCells(current.row, current.row, current.col, current.col);

            // If direction changes, stop for turn
            if (next_direction != current_direction) {
              drive_state = STOPPED;
              // Here you would stop motors:
              // stopMotors();
              Serial.println("Stopping for turn");
            }
            // If same direction, continue driving
            else {
              Serial.println("Continuing forward");
              // No need to modify motors, just keep driving
            }
          }
        }
        break;
      }

    // Turning state
    case TURNING:
      {

        // Check if turn is complete
        if (millis() - move_start_time >= TURN_MOVE_TIME) {

          // Update direction and start driving
          current_direction = target_direction;
          drive_state = DRIVING;
          move_start_time = millis();

          // Here you would update motor control:
          // stopTurning();
          // driveForward();

          Serial.println("Turn complete, starting forward movement");
        }
        break;
      }
  }
}

void loop() {

  // Get touch data
  TSPoint p = ts.getPoint();

  // Check if touch occured
  if (p.z > ts.pressureThreshhold) {

    // Calculate touch pixel coordinates
    int pixel_x = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, screen_width);
    int pixel_y = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, screen_height);
    Serial.println(pixel_x);
    Serial.println(pixel_y);
    Serial.println(" ");

    // Check if start button touched while not in settings state
    if (isTouchInStartButton(pixel_x, pixel_y) && ui_state != SETTINGS) {
      Serial.println("Start button touched");

      // Process touch based on current state
      switch (ui_state) {

        // Idle state
        case IDLE:
          {
            // Set to counting state and initialze couting variables
            ui_state = COUNTING;
            countdown_start = millis();
            countdown_number = countdown_duration / 1000;
            break;
          }

        // Counting state
        case COUNTING:
          // Set to idle state
          ui_state = IDLE;
          break;

        // Running state
        case RUNNING:
          // Set to idle state and stopped driving state
          ui_state = IDLE;
          drive_state = STOPPED;
          break;

        // Running state
        case COMPLETE:
          // Set to idle state
          ui_state = IDLE;
          break;
      }

      // Redraw entire grid
      drawGridCells(0, num_rows - 1, 0, num_cols - 1);

      // Draw button
      drawStartButton();

      // Debounce delay
      delay(250);
    }

    // Check if undo button touched while in idle state
    else if (isTouchInUndoButton(pixel_x, pixel_y) && ui_state == IDLE) {
      Serial.println("Undo button touched");

      // Reset grid values and path to default cells
      resetGridValues();
      resetDefaultPath();

      // Redraw entire grid
      drawGridCells(0, num_rows - 1, 0, num_cols - 1);

      // Debounce delay
      delay(50);
    }

    // Check if settings button touched
    else if (isTouchInSettingsButton(pixel_x, pixel_y)) {
      Serial.println("Settings button touched");

      // Check if in idle state
      if (ui_state == IDLE) {

        // Change to settings state and show menu
        ui_state = SETTINGS;
        drawSettingsMenu();
      }

      // Check if in settings state
      else if (ui_state == SETTINGS) {

        // Change back to idle state and redraw main interface
        ui_state = IDLE;
        drawUserInterface();
      }

      // Debounce delay
      delay(250);
    }

    // Check if grid touched while in idle state
    else if (isTouchInGrid(pixel_x, pixel_y) && ui_state == IDLE) {
      Serial.println("Grid button touched");

      // Calculated col/row of touched cell
      int grid_col = (pixel_x - x_offset) / cell_size;
      int grid_row = (pixel_y - y_offset) / cell_size;
      Serial.println(grid_col);
      Serial.println(grid_row);
      Serial.println(" ");

      // If cell is selectable, add to path and redraw surrounding cells
      if (isSelectable(grid_row, grid_col)) {
        pathAdd(grid_row, grid_col);
        drawGridCells(grid_row - 2, grid_row + 2, grid_col - 2, grid_col + 2);
      }
    }

    // Check if in settings state
    else if (ui_state == SETTINGS) {

      // Check if touch is in brightness right arrow
      if (isTouchInBrightnessRightArrow(pixel_x, pixel_y)) {
        // Increase brightness and redraw value
        display_brightness = min(100, display_brightness + 10);
        setBrightness();
        updateBrightnessDisplay();
        delay(250);
      }

      // Check if touch is in brightness left arrow
      else if (isTouchInBrightnessLeftArrow(pixel_x, pixel_y)) {
        // Decrease brightness and redraw value
        display_brightness = max(10, display_brightness - 10);
        setBrightness();
        updateBrightnessDisplay();
        delay(250);
      }

      // Check if touch is in size right arrow
      else if (isTouchInSizeRightArrow(pixel_x, pixel_y)) {
        // Increase drive speed and redraw value
        if (drive_distance < DISTANCE_EXTENDED) {
          drive_distance = (DriveDistance)(drive_distance + 1);
          updateDriveDistanceDisplay();
        }
        delay(250);
      }

      // Check if touch is in size left arrow
      else if (isTouchInSizeLeftArrow(pixel_x, pixel_y)) {
        // Decrease drive distance and redraw value
        if (drive_distance > DISTANCE_COMPACT) {
          drive_distance = (DriveDistance)(drive_distance - 1);
          updateDriveDistanceDisplay();
        }
        delay(250);
      }

      // Check if touch is in speed right arrow
      else if (isTouchInSpeedRightArrow(pixel_x, pixel_y)) {
        // Increase drive speed and redraw value
        if (drive_speed < SPEED_FAST) {
          drive_speed = (DriveSpeed)(drive_speed + 1);
          updateDriveSpeedDisplay();
        }
        delay(250);
      }

      // Check if touch is in speed left arrow
      else if (isTouchInSpeedLeftArrow(pixel_x, pixel_y)) {
        // Decrease drive speed and redraw value
        if (drive_speed > SPEED_SLOW) {
          drive_speed = (DriveSpeed)(drive_speed - 1);
          updateDriveSpeedDisplay();
        }
        delay(250);
      }
    }

    // Debounce delay
    delay(50);
  }

  // Check if in coutning state
  if (ui_state == COUNTING) {
    //
    if (millis() - countdown_start >= countdown_duration) {
      ui_state = RUNNING;
      current_path_index = 0;
      current_direction = 0;
      drawStartButton();
      //
    } else {
      // Calculate current countdown number
      int current_number = (countdown_duration / 1000) - ((millis() - countdown_start) / 1000);

      // Only draw if number has changed
      if (current_number != countdown_number) {
        countdown_number = current_number;
        drawStartButton();
      }
    }
  }

  // Check if in running state
  if (ui_state == RUNNING) {
    // Executute bot movements
    executeMovement();
  }

  delay(50);
}