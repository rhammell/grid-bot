#include "ui_elements.h"

void UIButton::drawFilled(Adafruit_ILI9341 &tft, uint16_t color) const {
  tft.fillRect(x, y, width, height, color);
}

void UIButton::drawIcon(Adafruit_ILI9341 &tft,
                        const uint16_t *icon,
                        int iconWidth,
                        int iconHeight,
                        uint16_t bgColor) const {
  drawFilled(tft, bgColor);
  int iconX = x + (width - iconWidth) / 2;
  int iconY = y + (height - iconHeight) / 2;
  tft.drawRGBBitmap(iconX, iconY, icon, iconWidth, iconHeight);
}

void UIButton::drawLabel(Adafruit_ILI9341 &tft,
                         const String &label,
                         uint16_t bgColor,
                         uint16_t textColor) const {
  drawFilled(tft, bgColor);
  tft.setTextColor(textColor);
  tft.setTextSize(2);
  int charWidth = 6 * 2;  // Character width at text size 2
  int textWidth = label.length() * charWidth;
  int textX = x + (width - textWidth) / 2;
  int textY = y + (height - 16) / 2;
  tft.setCursor(textX, textY);
  tft.print(label);
}

void UITextButton::draw(Adafruit_ILI9341 &tft) const {
  drawLabel(tft, label, bgColor, textColor);
}

void UIIconButton::draw(Adafruit_ILI9341 &tft) const {
  drawIcon(tft, icon, iconWidth, iconHeight, bgColor);
}

void UIArrow::draw(Adafruit_ILI9341 &tft, uint16_t color) const {
  if (dir == ARROW_LEFT) {
    tft.fillTriangle(
        x, y + height / 2,
        x + width, y + height / 4,
        x + width, y + (3 * height) / 4,
        color);
  } else {
    tft.fillTriangle(
        x + width, y + height / 2,
        x, y + height / 4,
        x, y + (3 * height) / 4,
        color);
  }
}
