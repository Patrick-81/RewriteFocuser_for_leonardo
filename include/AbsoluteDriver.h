/*
 * Absolute-Relative - Stepper Motor Driver Driver (A4988-compatible)
 * Indexer mode only.
 *
 * Copyright (C)2019 Patrick
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef AbsoluteDriver_H
#define AbsoluteDriver_H
#include <Arduino.h>
#include "DRV8825.h"

class AbsoluteDriver : public DRV8825 {
protected:
    static const uint8_t MS_TABLE[];
    // tWH(STEP) pulse duration, STEP high, min value (1.9us)
    static const int step_high_min = 2;
    // tWL(STEP) pulse duration, STEP low, min value (1.9us)
    static const int step_low_min = 2;
    // tWAKE wakeup time, nSLEEP inactive to STEP (1000us)
    static const int wakeup_time = 1700;
    // also 650ns between ENBL/DIR/MODEx changes and STEP HIGH

    // Get the microstep table
    const uint8_t* getMicrostepTable() override;
    size_t getMicrostepTableSize() override;

    // Get max microsteps supported by the device
    short getMaxMicrostep() override;

    long targetPos;
    long currentPos;

private:
    // microstep range (1, 16, 32 etc)
    static const short MAX_MICROSTEP = 32;

public:
    AbsoluteDriver(short steps, short dir_pin, short step_pin);
    AbsoluteDriver(short steps, short dir_pin, short step_pin, short enable_pin);
    AbsoluteDriver(short steps, short dir_pin, short step_pin, short mode0_pin, short mode1_pin, short mode2_pin);
    AbsoluteDriver(short steps, short dir_pin, short step_pin, short enable_pin, short mode0_pin, short mode1_pin, short mode2_pin);
    void  setCurrentPosition(long Target);
    void  moveTo(long Target);
    void  move(long steps,uint16_t wait);
 //   void  stop();
    void  startMove(long steps, long time=0);
    long  currentPosition();
    long  targetPosition();
    long  distanceToGo();
    long  run();
};
#endif // AbsoluteDriver_H
