// Copyright Thomas Herpoel 2016
//
// This file is part of BaseI6_updater
//
// BaseI6_updater is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FSi6_updater is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "commands.h"

uint8_t initCmds(uint8_t comPortNb, uint32_t baudrate) {
	return initFraming(comPortNb,baudrate);
}
uint8_t deinitCmds() {
	return deinitFraming();
}

uint8_t encData(uint8_t data[], uint16_t dataSize, uint8_t cmdType) {
	
	PACKET packet;
	
	packet.size=dataSize+1;
	packet.data=data;
	packet.cmdType=cmdType;
	
	
	return sendPacket(packet);
}
uint8_t decData(uint8_t *data, uint16_t *dataSize, uint8_t *cmdType) {
	PACKET packet;
	uint8_t code = readPacket(&packet);
	if(code!=0) {
		*dataSize=0;
		*cmdType=0;
		return code;
	}
	
	*dataSize = packet.size - 1;
	*cmdType = packet.cmdType;
	data = realloc(data,*dataSize);
	memcpy(data,packet.data,*dataSize);
	
	return code;
}

uint8_t sendPacket(PACKET packet) {
	uint8_t packetArray[packet.size];
	packetArray[0]=packet.cmdType;
	for(int i = 1;i<packet.size;i++) {
		packetArray[i]=packet.data[i-1];
	}
	encPacket(packetArray,packet.size);
	return 0;
}

uint8_t readPacket(PACKET *packet) {
	
	uint8_t *pkt=malloc(512);
	uint16_t size;
	uint8_t code = decPacket(pkt,&size);
	
	if(code !=0 ) {
		return code;
	}
	
	(*packet).size = size;
	(*packet).cmdType = pkt[0];
	(*packet).data = &(pkt[1]);
	
	// printf("\r\n");
	// printf("received packet size:\t0x%X\r\n",(*packet).size);
	// printf("received packet type:\t0x%X\r\n",(*packet).cmdType);
	// printf("received data:\r\n");
	// printArr((*packet).data,(*packet).size - 1);
	// printf("\r\n");
	
	return code;
}

