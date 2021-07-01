#ifndef __CONFIG__
#define __CONFIG__

// DO NOT CHANGE!
#define DIY 0
#define PCB_V1 1
#define PCB_V2 2
//------------------------------------------------------//
//SETTINGS
//------------------------------------------------------//

// Setup the OpenBot version (DIY,PCB_V1,PCB_V2)
#define OPENBOT DIY

// Enable/Disable voltage divider (1,0)
#define HAS_VOLTAGE_DIVIDER 0

// Enable/Disable indicators (1,0)
#define HAS_INDICATORS 0

// Enable/Disable speed sensors (1,0)
#define HAS_SPEED_SENSORS 1

// Enable/Disable sonar (1,0)
#define HAS_SONAR 0

// Enable/Disable median filter for sonar measurements (1,0)
#define USE_MEDIAN 0

// Enable/Disable OLED (1,0)
#define HAS_OLED 0

// Enable/Disable no phone mode (1,0)
// In no phone mode:
// - the motors will turn at 75% speed
// - the speed will be reduced if an obstacle is detected by the sonar sensor
// - the car will turn, if an obstacle is detected within STOP_THRESHOLD
// WARNING: If the sonar sensor is not setup, the car will go full speed forward!
#define NO_PHONE_MODE 1
//
#define SEND_INTERVAL 1000  // How frequently vehicle data is sent (ms).
//
#define PS2_GAMEPAD 0

#endif
