#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "framing.h"

#define CMD_DETECT	0xC0
#define CMD_RESET	0xC1
#define CMD_PREP	0xC2
#define CMD_WRITE	0xC3



typedef struct _PACKET {
	uint16_t size; // off the whole packet
	uint16_t cmdType;
	uint8_t *data;
}PACKET;


uint8_t initCmds(uint8_t comPortNb, uint32_t baudrate);
uint8_t deinitCmds();

uint8_t encData(uint8_t data[], uint16_t dataSize, uint8_t cmdType);
uint8_t decData(uint8_t *data, uint16_t *dataSize, uint8_t *cmdType);

uint8_t sendPacket(PACKET packet);
uint8_t readPacket(PACKET *packet);