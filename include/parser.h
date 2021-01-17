#ifndef __PARSER_H__
#define __PARSER_H__
#include "AbsoluteDriver.h"
#include "Config.h"
#include <WSWire.h>
#define FORWARD   1
#define BACKWARD -1
void initialize(int initPos);
void process_data(const char *input);
void move();
void stop();
void inithandpad();
void processIncomingByte (const byte inByte);
#endif