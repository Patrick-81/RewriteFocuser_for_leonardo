#ifndef Config_h
#define Config_h

// ----- General config -----
// Status LED pin
#define LED 13
#define SERIAL_SPEED 9600
#define ENABLE_SW_RS false

// ----- Stepper driver configuration -----
// Motor driver type: BasicStepperDriver (0), DRV8825 (1), A4988 (2) or DRV8834 (3)
#define STEPPER_TYPE 1
// Steps per revolution
#define STEPS_REV 200
// gears reduction
#define GEARS_RED 3
// Stepper resolution
#define SINGLE_STEP 1
// Ignored for BasicStepperDriver. Can be 1, 2, 4, 8, 16 or 32
#define FULL_STEP 16
// Ignored for BasicStepperDriver. Can be 1, 2, 4, 8, 16 or 32
#define HALF_STEP 32
// Reverses the direction of the motor
#define REVERSE_DIR true
// RPM. Decreasing this value should decrease telescope shaking.
#define MOTOR_RPM 50
#define MIN_RPM 10
#define MAX_RPM 100
// Acceleration
#define MOTOR_ACCEL 1000
// Motor pulses per second (pps). See full formula here: https://electronics.stackexchange.com/a/232679
// #define MOTOR_PPS (MOTOR_RPM * STEPS_REV / 60)
// ----- Driver pins -----
#if defined(ARDUINO_AVR_NANO)
    // Driver DIR pin
    #define DRIVER_DIR 2
    // Driver STEP pin
    #define DRIVER_STEP 3

#elif defined (ARDUINO_AVR_LEONARDO)
    // Driver DIR pin
    #define DRIVER_DIR 4
    // Driver STEP pin
    #define DRIVER_STEP 5
#endif
// Driver ENABLE pin. Comment to disable functionality
#define DRIVER_EN 6
// M0. Only for microstepping-enabled drivers
#define M0 4
// M1. Only for microstepping-enabled drivers
#define M1 5
// M2. Only for microstepping-enabled drivers
#define M2 6
// Mouvement manuel in
#define INWARD 7
// Mouvement manuel out
#define OUTWARD 8
// I2C Port
#define I2CPORT 0x02

// ----- Polar finder light -----
#define ENABLE_POLAR_LIGHT false
#define POT_ANALOG_PIN 0
#define POLAR_LIGHT_LED 10

// ----- Customizable pins -----
// List of pins that can be modified by the serial port
//#define CUSTOMIZABLE_PINS {7, 8, 9}
// Backlash use
// right shift value
// The backlash value is defined by
// Backlash = StepMode * STEPS_REV * GEARS_RED
// Backlash = (Backlash >> BacklashLevel);
//
#define BACKLASHHIGH 2
#define BACKLASHMED  4
#define BACKLASHLOW  6
#
#define __DEBUG__    true

#endif