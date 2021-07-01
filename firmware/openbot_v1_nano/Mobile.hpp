#ifndef __MOBILE__
#define __MOBILE__
class Motor {
  protected:
    int pin1;
    int pin2;
    int pwmPin;

  public:
    Motor(int pin1, int pin2): pin1(pin1), pin2(pin2) {
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
    }
    
    void update(int speed) {
      if (speed < 0) {
        analogWrite(pin1, -speed);
        analogWrite(pin2, 0);
      } else if (speed > 0) {
        analogWrite(pin1, 0);
        analogWrite(pin2, speed);
      } else { //Motor brake
        analogWrite(pin1, 255);
        analogWrite(pin1, 255);
      }
    }
    void stop() {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
    }
};

class Mobile {
  private:
    Motor& rightMotors;
    Motor& leftMotors;

  public:
    Mobile(Motor &rightMotors, Motor& leftMotors):
      rightMotors(rightMotors),
      leftMotors(leftMotors) {
    }

    void updateSpeed(int right, int left) {
//      right=0;
//      left=right;
      rightMotors.update(right);
      leftMotors.update(left);
//      static long prev = 0;
//      if (millis() - 1000 > prev) {
//        prev = millis();
//        Serial.println("[Mobile]right speed: " + String(right) + ",left speed: " + String(left));
//      }
    }
};
#endif
