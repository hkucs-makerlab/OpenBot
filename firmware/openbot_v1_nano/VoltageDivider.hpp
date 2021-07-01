#ifndef __VOLTAGE_DIVIDER__
#define __VOLTAGE_DIVIDER__
#include "Config.hpp"

class VoltageDivider {
  private:
    const unsigned int ADC_MAX = 1023;
    const unsigned int VREF = 5;
    //The voltage divider factor is computed as (R1+R2)/R2
#if (OPENBOT == PCB_V1)
    const float VOLTAGE_DIVIDER_FACTOR = (100 + 33) / 33;
#else //DIY and PCB_V2
    const float VOLTAGE_DIVIDER_FACTOR = (20 + 10) / 10;
#endif
    static const unsigned int VIN_ARR_SZ = 10;
    unsigned int vin_counter = 0;
    unsigned int vin_array[VIN_ARR_SZ];
    int pin;
  public:
    VoltageDivider(int pin):vin_array{0},pin(pin) {
      
    }
    
    void checkVoltage() {
      vin_array[vin_counter % VIN_ARR_SZ] = analogRead(pin);
      vin_counter++;
    }

    float getVoltage () {
      unsigned long array_sum = 0;
      unsigned int array_size = min(VIN_ARR_SZ, vin_counter);
      for (unsigned int index = 0; index < array_size; index++)
      {
        array_sum += vin_array[index];
      }
      return float(array_sum) / array_size / ADC_MAX * VREF * VOLTAGE_DIVIDER_FACTOR;
    }
};
#endif
