#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "serial.h"

#define TIMEOUT 1000 //ms

typedef struct _FRAME {
	uint16_t size;
	uint16_t checksum;
	uint8_t *packet;
}FRAME;

uint8_t initFraming(uint8_t comPortNb, uint32_t baudrate);
uint8_t deinitFraming();

void printArr(uint8_t array[],uint16_t size);
uint16_t genChecksum(FRAME frame, uint16_t size);

uint8_t encPacket(uint8_t packet[], uint16_t size);
uint8_t decPacket(uint8_t *pkt, uint16_t *size);

uint8_t sendFrame(FRAME frame);
uint8_t readFrame(FRAME *frame);