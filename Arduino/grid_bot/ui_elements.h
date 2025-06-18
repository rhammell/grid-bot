#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include <Adafruit_ILI9341.h>
#include <Arduino.h>

// Base class now only contains what is common to ALL buttons.
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

  // The base draw method is now virtual.
  virtual void draw(Adafruit_ILI9341 &tft) const;
};

class UITextButton : public UIButton {
public:
  String label;
  uint16_t textColor;
  uint8_t textSize;

  UITextButton() : label(""), textColor(0), textSize(2) {}

  void setLabel(const String &l) { label = l; }
  void setTextColor(uint16_t c) { textColor = c; }
  void setTextSize(uint8_t size) { textSize = size; }

  // This is the primary drawing method for this class.
  void draw(Adafruit_ILI9341 &tft) const override;
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

  // This is the primary drawing method for this class.
  void draw(Adafruit_ILI9341 &tft) const override;
};

enum ArrowDirection { ARROW_LEFT, ARROW_RIGHT };

class UIArrow : public UIButton {
public:
  ArrowDirection dir;
  int triangleWidth;   // New attribute for explicit triangle width
  int triangleHeight;  // New attribute for explicit triangle height
  uint16_t triangleColor;  // New attribute for triangle color

  UIArrow() : dir(ARROW_LEFT), triangleWidth(12), triangleHeight(18), triangleColor(ILI9341_WHITE) {}  // Default values based on previous 60% of 20x30

  void setDirection(ArrowDirection d) { dir = d; }
  void setTriangleSize(int width, int height) {  // New setter method
    triangleWidth = width;
    triangleHeight = height;
  }
  void setTriangleColor(uint16_t color) { triangleColor = color; }

  // Override the draw method to draw the arrow
  void draw(Adafruit_ILI9341 &tft) const override;
};

class UISettingsValue {
public:
    int x;
    int y;
    int width;
    uint16_t backgroundColor;
    uint16_t textColor;
    uint8_t textSize;
    int charWidth;  // NEW: character width in pixels
    int charHeight; // NEW: character height in pixels
    String value;

    UISettingsValue() : x(0), y(0), width(0),
                       backgroundColor(ILI9341_DARKGREY),
                       textColor(ILI9341_WHITE),
                       textSize(2),
                       charWidth(6),  // Default font char width
                       charHeight(8), // Default font char height
                       value("") {}

    void setPosition(int bx, int by, int w) {
        x = bx;
        y = by;
        width = w;
    }

    void setColors(uint16_t bg, uint16_t txt) {
        backgroundColor = bg;
        textColor = txt;
    }

    void setTextSize(uint8_t size) { textSize = size; }
    void setValue(const String &val) { value = val; }
    
    // NEW: Set font metrics
    void setFontMetrics(int cw, int ch) {
        charWidth = cw;
        charHeight = ch;
    }

    void draw(Adafruit_ILI9341 &tft) const;
};

#endif
