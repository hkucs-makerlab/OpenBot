#include "Config.hpp"

#include <PinChangeInterrupt.h> // https://www.arduinolibraries.info/libraries/pin-change-interrupt

class SpeedSensor {
  private:
    static const unsigned int DISK_HOLES = 20;
    volatile int counter;
    float rpm;

  public:
    SpeedSensor(int pin, void (*isr)(void)) {
      pinMode(pin, INPUT);
      // isr calls inc() and dec()
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(pin), isr, FALLING);
    }

    inline void inc() {
      counter++;
    }

    inline void dec() {
      counter--;
    }

    inline float getRPM() {
      int ticks = counter;
      counter = 0;
      rpm = ticks * 60.0 * (1000.0 / SEND_INTERVAL) / (DISK_HOLES);
      return rpm;
    }

    inline int getCounter() {
      return counter;
    }
};
