#ifndef __DISPLAY__
#define __DISPLAY__
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>   // https://github.com/adafruit/Adafruit_SSD1306

class Display {

  private:
    // OLED Display SSD1306
    const unsigned int SCREEN_WIDTH = 128; // OLED display width, in pixels
    const unsigned int SCREEN_HEIGHT = 32; // OLED display height, in pixels
    const int OLED_RESET = -1; // not used
    Adafruit_SSD1306 display;
    float rpm_left;
    float rpm_right;
    float voltage_value;
    unsigned int distance_estimate;

    unsigned int distance;
  public:
    Display(): display(OLED_RESET) {

    }

    void begin() {
      //Initialize with the I2C addr 0x3C
      display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    }

    void setRPM(float rpm_left, float rpm_right) {
      this->rpm_left = rpm_left;
      this->rpm_right = rpm_left;
    }

    void setDistance(unsigned int d) {
      distance_estimate = d;
    }

    void setVoltage(float v) {
      voltage_value = v;
    }

    void drawString() {
      display.clearDisplay();
      // set text color
      display.setTextColor(WHITE);
      // set text size
      display.setTextSize(1);
      // set text cursor position
      display.setCursor(1, 0);
      // show text
      if (voltage_value >= 0)
        display.println("Voltage:    " + String(voltage_value, 2));
      else
        display.println("Voltage:    unknown");
      display.setCursor(1, 8);
      // show text
      display.println("Left RPM:  " + String(rpm_left, 0));
      display.setCursor(1, 16);
      // show text
      display.println("Right RPM: " + String(rpm_right, 0));
      display.setCursor(1, 24);
      // show text
      display.println("Distance:   " + String(distance_estimate));
      display.display();
    }
};

#endif
