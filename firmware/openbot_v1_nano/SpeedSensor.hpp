#include "Config.hpp"

#include <PinChangeInterrupt.h> // https://www.arduinolibraries.info/libraries/pin-change-interrupt

class SpeedSensor {
  private:
    static const unsigned int DISK_HOLES = 20;
    volatile int counter;
    int ticks;
    int pin;

  public:
    SpeedSensor(int pin): pin(pin), counter(0) {
      pinMode(pin, INPUT);
    }

    void begin(void (*isr)(void)) {
      // isr calls inc() and dec()
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(pin), isr, FALLING);
    }

    void inc() {
      counter++;
    }

    void dec() {
      counter--;
    }

    float getRPM() {
      getTicks();
      return ticks * 60.0 * (1000.0 / SEND_INTERVAL) / (DISK_HOLES);
    }

    int getTicks() {
      ticks = counter;
      counter = 0;
      return ticks;
    }
};
