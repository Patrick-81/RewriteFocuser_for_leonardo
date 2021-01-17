/*
 * AbsoluteDriver - Stepper Motor Driver Driver
 * Indexer mode only.

 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "AbsoluteDriver.h"

bool forward = true;
long dep = 0;
/*
 * Microstepping resolution truth table (Page 13 of AbsoluteDriver pdf)
 * 0bMODE2,MODE1,MODE0 for 1,2,4,8,16,32 microsteps
 */
const uint8_t AbsoluteDriver::MS_TABLE[] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b111};

AbsoluteDriver::AbsoluteDriver(short steps, short dir_pin, short step_pin)
:DRV8825(steps, dir_pin, step_pin)
{}

AbsoluteDriver::AbsoluteDriver(short steps, short dir_pin, short step_pin, short enable_pin)
:DRV8825(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * A4988-AbsoluteDriver Compatibility map: MS1-MODE0, MS2-MODE1, MS3-MODE2
 */
AbsoluteDriver::AbsoluteDriver(short steps, short dir_pin, short step_pin, short mode0_pin, short mode1_pin, short mode2_pin)
:DRV8825(steps, dir_pin, step_pin, mode0_pin, mode1_pin, mode2_pin)
{}

AbsoluteDriver::AbsoluteDriver(short steps, short dir_pin, short step_pin, short enable_pin, short mode0_pin, short mode1_pin, short mode2_pin)
:DRV8825(steps, dir_pin, step_pin, enable_pin, mode0_pin, mode1_pin, mode2_pin)
{}

const uint8_t* AbsoluteDriver::getMicrostepTable()
{
    return (uint8_t*)AbsoluteDriver::MS_TABLE;
}

size_t AbsoluteDriver::getMicrostepTableSize()
{
    return sizeof(AbsoluteDriver::MS_TABLE);
}

short AbsoluteDriver::getMaxMicrostep(){
    return AbsoluteDriver::MAX_MICROSTEP;
}

void AbsoluteDriver::setCurrentPosition(long Target)
{
  currentPos = Target;
}
void AbsoluteDriver::moveTo(long Target)
{
  targetPos = Target;
  forward = (targetPos > currentPos ? true : false);
  dep = (targetPos-currentPos);
  startMove(dep);
}
long AbsoluteDriver::currentPosition()
{
  return (forward ? (targetPos - getStepsRemaining()) : (targetPos + getStepsRemaining()));
}
long AbsoluteDriver::targetPosition()
{
  return targetPos;
}
long AbsoluteDriver::distanceToGo()
{
  return getStepsRemaining();
}
long AbsoluteDriver::run()
{
  long remaining = nextAction();
  currentPos = (forward ? (targetPos - getStepsRemaining()) : (targetPos + getStepsRemaining()));
  return currentPos;
}
void AbsoluteDriver::startMove(long steps, long time=0)
{
  BasicStepperDriver::startMove(steps, time);
}
/*
void AbsoluteDriver::stop()
{
  BasicStepperDriver::stop();
  targetPos = currentPos;
}
*/
void AbsoluteDriver::move(long steps, uint16_t wait)
{
  BasicStepperDriver::move(steps);
  delayMicroseconds(wait);
}
