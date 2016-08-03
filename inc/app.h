#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"


uint8_t appInit(uint8_t comPortNb, uint32_t baudrate);
uint8_t appDeinit();

uint8_t appReset();
uint8_t appDetect();
uint8_t appPrepareBlock(uint16_t offset);
uint8_t appWritePage(uint8_t *data,uint16_t size, uint32_t offset);