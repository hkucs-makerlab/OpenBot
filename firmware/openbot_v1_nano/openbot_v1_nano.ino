// ---------------------------------------------------------------------------
// This Arduino sketch accompanies the OpenBot Android application.
// By Matthias Mueller, Intelligent Systems Lab, 2020
//
// The sketch has the following functinonalities:
//  - receive control commands from Android application (USB serial)
//. - produce low-level controls (PWM) for the vehicle
//  - toggle left and right indicator signals
//  - wheel odometry based on optical speed sensors
//  - estimate battery voltage via voltage divider
//  - estimate distance based on sonar sensor
//  - send sensor readings to Android application (USB serial)
//  - display vehicle status on OLED
//
// Dependencies: Install via "Tools --> Manage Libraries" (type library name in the search field)
//  - Interrupts: PinChangeInterrupt by Nico Hood (read speed sensors and sonar)
//  - OLED: Adafruit_SSD1306 & Adafruit_GFX (display vehicle status)
// Contributors:
//  - October 2020: OLED display support by Ingmar Stapel
// ---------------------------------------------------------------------------
#include "Config.hpp"

// PIN_PWM_L1,PIN_PWM_L2,PIN_PWM_R1,PIN_PWM_R2  Low-level control of left DC motors via PWM
// PIN_SPEED_L, PIN_SPEED_R                     Measure left and right wheel speed
// PIN_VIN                                      Measure battery voltage via voltage divider
// PIN_TRIGGER                                  Arduino pin tied to trigger pin on ultrasonic sensor.
// PIN_ECHO                                     Arduino pin tied to echo pin on ultrasonic sensor.
// PIN_LED_LB, PIN_LED_RB                       Toggle left and right rear LEDs (indicator signals)
//------------------------------------------------------//
// PINOUT
//------------------------------------------------------//
//Setup the pin definitions
#if (OPENBOT == DIY)
#include "Buzzer.hpp"
#define PIN_SPEED_L 2
#define PIN_SPEED_R 3
#define PIN_BUZZER  4
#define PIN_PWM_L1  5
#define PIN_PWM_L2  6
#define PIN_LED_LB  7
#define PIN_LED_RB  8
#define PIN_PWM_R1  9
#define PIN_PWM_R2  10
#define PIN_ECHO    11
#define PIN_TRIGGER 12
#if PS2_GAMEPAD
#define PS2_CLK A0  // sck
#define PS2_ATT A1  // cs
#define PS2_CMD A2  // mosi
#define PS2_DAT A3  // miso
#endif // PS2_GAMEPAD
#define PIN_VIN     A7
#elif (OPENBOT == PCB_V1)
#define PIN_PWM_L1 9
#define PIN_PWM_L2 10
#define PIN_PWM_R1 5
#define PIN_PWM_R2 6
#define PIN_SPEED_L 2
#define PIN_SPEED_R 4
#define PIN_VIN A7
#define PIN_TRIGGER 3
#define PIN_ECHO 3
#define PIN_LED_LB 7
#define PIN_LED_RB 8
#elif (OPENBOT == PCB_V2)
#define PIN_PWM_L1 9
#define PIN_PWM_L2 10
#define PIN_PWM_R1 5
#define PIN_PWM_R2 6
#define PIN_SPEED_L 2
#define PIN_SPEED_R 3
#define PIN_VIN A7
#define PIN_TRIGGER 4
#define PIN_ECHO 4
#define PIN_LED_LB 7
#define PIN_LED_RB 8
#endif
//------------------------------------------------------//
// INITIALIZATION
//------------------------------------------------------//
#include "Mobile.hpp"
#include "SpeedSensor.hpp"
#include <limits.h>
const unsigned int STOP_THRESHOLD = 32; //cm

#if NO_PHONE_MODE
int turn_direction = 0; // right
const unsigned long TURN_DIRECTION_INTERVAL = 2000; // How frequently to change turn direction (ms).
unsigned long turn_direction_timeout = 0;   // After timeout (ms), random turn direction is updated.
#endif

#if HAS_SONAR
#include "Sonar.hpp"
Sonar sonar(PIN_TRIGGER, PIN_ECHO);
#endif

#if HAS_OLED
#include "Display.hpp"
Display oledDisplay;
#endif

#if HAS_VOLTAGE_DIVIDER
#include "VoltageDivider.hpp"
VoltageDivider voltageDivider(PIN_VIN);
#endif

#if (OPENBOT == DIY)
#include "Buzzer.hpp"
Buzzer buzzer(PIN_BUZZER);
#endif

#if HAS_INDICATORS
//Indicator Signal
#include "Indicator.hpp"
Indicator indicator(PIN_LED_LB, PIN_LED_RB);
#endif

Motor rightMotors(PIN_PWM_R1, PIN_PWM_R2);
Motor leftMotors(PIN_PWM_L1, PIN_PWM_L2);
Mobile mobile(rightMotors, leftMotors);

//Vehicle Control
const int max_speed = 192;
const int min_speed = 64;
int ctrl_left_speed = 0;
int ctrl_right_speed = 0;

#if HAS_SPEED_SENSORS
//Speed sensor
extern void update_speed_left_isr();
extern void update_speed_right_isr();
SpeedSensor leftSpeedSensor(PIN_SPEED_L, update_speed_left_isr);
SpeedSensor rightSppedSensor(PIN_SPEED_R, update_speed_right_isr);
#endif

//Serial communication
unsigned long send_timeout = 0;
String inString = "";
//
unsigned int distance_estimate = UINT_MAX; //cm

//------------------------------------------------------//
// SETUP
//------------------------------------------------------//
void setup()
{
  Serial.begin(115200, SERIAL_8N1); //8 data bits, no parity, 1 stop bit
  send_timeout = millis() + SEND_INTERVAL; //wait for one interval to get readings

#if HAS_OLED
  oledDisplay.begin();
#endif

  //Test sequence for indicator LEDs
#if HAS_INDICATORS
  indicator.flash();
#endif

#if (OPENBOT == DIY)
  buzzer.beep();
#endif
}

//------------------------------------------------------//
// MAIN LOOP
//------------------------------------------------------//

void loop() {

#if HAS_VOLTAGE_DIVIDER
  //Measure voltage
  voltageDivider.checkVoltage();
#endif

#if HAS_SONAR == 1
  sonar.measure(send_ping);
#endif

#if HAS_INDICATORS
  indicator.signalling();
#endif

  // Send vehicle measurments to serial every SEND_INTERVAL
  if (millis() >= send_timeout) {
    send_vehicle_data();
    send_timeout = millis() + SEND_INTERVAL;
  }

#if (NO_PHONE_MODE)
  if (millis() > turn_direction_timeout) {
    turn_direction_timeout = millis() + TURN_DIRECTION_INTERVAL;
    turn_direction = random(2); //Generate random number in the range [0,1]
  }
  // drive forward
  if (distance_estimate > 4 * STOP_THRESHOLD) {
    ctrl_left_speed = distance_estimate;
    ctrl_right_speed = ctrl_left_speed;
#if HAS_INDICATORS
    indicator.off();
#endif
  }
  // turn slightly
  else if (distance_estimate > 2 * STOP_THRESHOLD) {
    ctrl_left_speed = distance_estimate;
    ctrl_right_speed = ctrl_left_speed - 3 * STOP_THRESHOLD;
  }
  // turn strongly
  else if (distance_estimate > STOP_THRESHOLD) {
    ctrl_left_speed = max_speed;
    ctrl_right_speed = -max_speed;
  }
  // drive backward slowly
  else {
    ctrl_left_speed = -min_speed;
    ctrl_right_speed = -min_speed;
#if HAS_INDICATORS
    indicator.on();
#endif
  }
  // flip controls if needed and set indicator light
  if (ctrl_left_speed != ctrl_right_speed) {
    if (turn_direction > 0) {
      int temp = ctrl_left_speed;
      ctrl_left_speed = ctrl_right_speed;
      ctrl_right_speed = temp;
#if HAS_INDICATORS
      indicator.left();
#endif
    }
    else {
#if HAS_INDICATORS
      indicator.right();
#endif
    }
  }
  // enforce limits
  ctrl_left_speed = ctrl_left_speed > 0 ? max(min_speed, min(ctrl_left_speed, max_speed)) : min(-min_speed, max(ctrl_left_speed, -max_speed));
  ctrl_right_speed = ctrl_right_speed > 0 ? max(min_speed, min(ctrl_right_speed, max_speed)) : min(-min_speed, max(ctrl_right_speed, -max_speed));

#else // Wait for messages from the phone
  if (Serial.available() > 0) {
    read_msg();
  }
  if (distance_estimate < STOP_THRESHOLD) {
    if (ctrl_left_speed > 0) ctrl_left_speed = 0;
    if (ctrl_right_speed > 0) ctrl_right_speed = 0;
  }
#endif
  mobile.updateSpeed(ctrl_right_speed, ctrl_left_speed);
}


//------------------------------------------------------//
// FUNCTIONS
//------------------------------------------------------//

void read_msg() {
  if (Serial.available()) {
    char inChar = Serial.read();
    switch (inChar) {
      case 'c':
        ctrl_left_speed = Serial.readStringUntil(',').toInt();
        ctrl_right_speed = Serial.readStringUntil('\n').toInt();
        break;
      case 'i': {
          //indicator_val = Serial.readStringUntil('\n').toInt();
          int val = Serial.readStringUntil('\n').toInt();
#if HAS_INDICATORS == 1
          indicator.setValue(val);
#endif
        }
        break;
      default:
        break;
    }
  }
}

void send_vehicle_data() {
  float rpm_left  = 0;
  float rpm_right = 0;
#if HAS_VOLTAGE_DIVIDER
  float voltage_value = voltageDivider.getVoltage();
#else
  float voltage_value = -1;
#endif

#if HAS_SONAR
  distance_estimate = sonar.getDistance();
#endif

#if (NO_PHONE_MODE || HAS_OLED)
#if HAS_SPEED_SENSORS
  rpm_left = leftSpeedSensor.getRPM();
  rpm_right = rightSppedSensor.getRPM();
#endif
#if HAS_OLED
  oledDisplay.setRPM(rpm_left, rpm_right);
#endif
#endif

#if (NO_PHONE_MODE)
  if (voltage_value >= 0) {
    Serial.print("Voltage: "); Serial.println(voltage_value, 2);
  } else {
    Serial.println("Voltage: unknown");
  }
  Serial.print("Left RPM: "); Serial.println(rpm_left, 0);
  Serial.print("Right RPM: "); Serial.println(rpm_right, 0);
  Serial.print("Distance: "); Serial.println(distance_estimate);
  Serial.println("------------------");
#else
  // send data to phone
  Serial.print(voltage_value);
  Serial.print(",");
  Serial.print(leftSpeedSensor.getCounter());
  Serial.print(",");
  Serial.print(rightSppedSensor.getCounter());
  Serial.print(",");
  Serial.print(distance_estimate);
  Serial.println();
#endif

#if HAS_OLED
  // Set display information
  oledDisplay.setDistance(distance_estimate);
  oledDisplay.setVoltage(voltage_value);
  oledDisplay.drawString();
#endif
}

//------------------------------------------------------//
// INTERRUPT SERVICE ROUTINES (ISR)
//------------------------------------------------------//

#if HAS_SPEED_SENSORS
// ISR: Increment speed sensor counter (right)
void update_speed_left_isr() {
  if (ctrl_left_speed < 0) {
    leftSpeedSensor.dec();
  }
  else if (ctrl_left_speed > 0) {
    leftSpeedSensor.inc();
  }
}

// ISR: Increment speed sensor counter (right)
void update_speed_right_isr() {
  if (ctrl_right_speed < 0) {
    rightSppedSensor.dec();
  }
  else if (ctrl_right_speed > 0) {
    rightSppedSensor.inc();
  }
}
#endif

#if HAS_SONAR
#include <PinChangeInterrupt.h> // https://www.arduinolibraries.info/libraries/pin-change-interrupt
// Send pulse by toggling trigger pin
void send_ping() {
  detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_ECHO));
  sonar.signalTrigger();
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_ECHO), start_timer, RISING);
}
// ISR: Start timer to measure the time it takes for the pulse to return
void start_timer() {
  sonar.setStartTime(micros());
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_ECHO), stop_timer, FALLING);
}
// ISR: Stop timer and record the time
void stop_timer() {
  sonar.setEchoTime();
}
#endif
