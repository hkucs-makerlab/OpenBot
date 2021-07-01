#ifndef __INDICATOR__
#define __INDICATOR__

class Indicator {
  private:
    const unsigned long INDICATOR_INTERVAL = 500; //Blinking rate of the indicator signal (ms).
    unsigned long indicator_timeout = 0;
    int indicator_val = 0;
    int pin1;
    int pin2;

    void update() {
      if (indicator_val < 0) {
        digitalWrite(pin1, !digitalRead(pin1));
        digitalWrite(pin2, 0);
      }
      else if (indicator_val > 0) {
        digitalWrite(pin1, 0);
        digitalWrite(pin2, !digitalRead(pin2));
      }      else {
        digitalWrite(pin1, 0);
        digitalWrite(PIN_LED_RB, 0);
      }
    }

  public:
    Indicator(int pin1, int pin2): pin1(pin1), pin2(pin2) {
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
    }
    void flash() {
      //
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      delay(500);
      digitalWrite(pin1, HIGH);
      delay(500);
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      delay(500);
      digitalWrite(pin2, LOW);
    }
    void off() {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
    }

    void on() {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, HIGH);
    }

    void left() {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
    }

    void right() {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
    }
    
    void setValue(int value) {
      indicator_val = value;
    }
    
    void signalling() {
      // Check indicator signal every INDICATOR_INTERVAL
      if (millis() >= indicator_timeout) {
        update();
        indicator_timeout = millis() + INDICATOR_INTERVAL;
      }
    }
};
#endif
