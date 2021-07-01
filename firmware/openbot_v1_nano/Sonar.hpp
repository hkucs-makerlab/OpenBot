#ifndef __SONAR__
#define __SONAR__

#include "Config.hpp"

class Sonar {
  private:
    int trigPin, echoPin;
    long echo_time; // Time taken to receive echo
    long start_time;  //Time when sending sonar pulse was sent
    const float US_TO_CM = 0.01715; //cm/uS -> (343 * 100 / 1000000) / 2;
    const unsigned long PING_INTERVAL = 100; // How frequently to send out a ping (ms).
    unsigned long ping_timeout = 0;   // After timeout (ms), distance is set to maximum.
    unsigned int distance = UINT_MAX; //cm
    unsigned int distance_estimate = UINT_MAX; //cm
#if USE_MEDIAN
    static const unsigned int distance_array_sz = 3;
    unsigned int distance_array[distance_array_sz] = {};
    unsigned int distance_counter = 0;

    unsigned int get_median(unsigned int a[], unsigned int sz) {
      //bubble sort
      for (unsigned int i = 0; i < (sz - 1); i++) {
        for (unsigned int j = 0; j < (sz - (i + 1)); j++) {
          if (a[j] > a[j + 1]) {
            unsigned int t = a[j];
            a[j] = a[j + 1];
            a[j + 1] = t;
          }
        }
      }
      return a[sz / 2];
    }
#endif

  public:
    Sonar(int trigPin, int echoPin): trigPin(trigPin), echoPin(echoPin) {
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }

    inline void setStartTime(long startTime) {
      this->start_time = startTime;
    }

    inline void setEchoTime() {
      this->echo_time = micros() - start_time;
    }

    void signalTrigger() {
      echo_time = 0;
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
    }

    void measure(void (*send_echo)(void)) {
      //Measure distance every PING_INTERVAL
      if (millis() >= ping_timeout) {
        ping_timeout = ping_timeout + PING_INTERVAL;
        if (echo_time == 0) { //No echo received
          distance = UINT_MAX;
        } else {
          distance = echo_time * US_TO_CM;
        }
#if USE_MEDIAN
        distance_array[distance_counter % distance_array_sz] = distance;
        distance_counter++;
        distance_estimate = get_median(distance_array, distance_array_sz);
#else
        distance_estimate = distance;
#endif
        send_echo();
      }
    }

    unsigned int getDistance() {
      return distance_estimate;
    }
};
#endif //__SONAR__
