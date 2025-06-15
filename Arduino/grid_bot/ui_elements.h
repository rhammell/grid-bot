#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include <Adafruit_ILI9341.h>
#include <Arduino.h>

class UIButton {
public:
  int x;
  int y;
  int width;
  int height;
  uint16_t bgColor;

  UIButton() : x(0), y(0), width(0), height(0), bgColor(0) {}

  void setBounds(int bx, int by, int w, int h) {
    x = bx;
    y = by;
    width = w;
    height = h;
  }

  bool contains(int px, int py) const {
    return px >= x && px <= x + width && py >= y && py <= y + height;
  }

  void setBgColor(uint16_t color) { bgColor = color; }

  void drawFilled(Adafruit_ILI9341 &tft, uint16_t color) const;

  void drawIcon(Adafruit_ILI9341 &tft,
                const uint16_t *icon,
                int iconWidth,
                int iconHeight,
                uint16_t bgColor) const;

  void drawLabel(Adafruit_ILI9341 &tft,
                 const String &label,
                 uint16_t bgColor,
                 uint16_t textColor) const;
};

class UITextButton : public UIButton {
public:
  String label;
  uint16_t textColor;

  UITextButton() : label(""), textColor(0) {}

  void setLabel(const String &l) { label = l; }
  void setTextColor(uint16_t c) { textColor = c; }

  void draw(Adafruit_ILI9341 &tft) const;
};

class UIIconButton : public UIButton {
public:
  const uint16_t *icon;
  int iconWidth;
  int iconHeight;

  UIIconButton() : icon(nullptr), iconWidth(0), iconHeight(0) {}

  void setIcon(const uint16_t *i, int w, int h) {
    icon = i;
    iconWidth = w;
    iconHeight = h;
  }

  void draw(Adafruit_ILI9341 &tft) const;
};

enum ArrowDirection { ARROW_LEFT, ARROW_RIGHT };

class UIArrow {
public:
  int x;
  int y;
  int width;
  int height;
  ArrowDirection dir;

  UIArrow() : x(0), y(0), width(0), height(0), dir(ARROW_LEFT) {}

  void set(ArrowDirection d, int bx, int by, int w, int h) {
    dir = d;
    x = bx;
    y = by;
    width = w;
    height = h;
  }

  bool contains(int px, int py, int padding = 0) const {
    return px >= x - padding && px <= x + width + padding &&
           py >= y - padding && py <= y + height + padding;
  }

  void draw(Adafruit_ILI9341 &tft, uint16_t color) const;
};

#endif
