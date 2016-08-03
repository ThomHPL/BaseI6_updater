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

#include "framing.h"

static HANDLE hdlr = NULL;

uint8_t initFraming(uint8_t comPortNb, uint32_t baudrate) {
	return openCom(comPortNb,(DWORD)baudrate,&hdlr);
}
uint8_t deinitFraming() {
	closeCom(&hdlr);
	return 0;
}

uint16_t genChecksum(FRAME frame, uint16_t size) {
	uint8_t tmp[sizeof(frame)];
	memcpy(tmp,&frame,sizeof(frame));
	uint16_t checksum = 0xFFFF;
	checksum -= frame.size & 0xFF;
	checksum -= (frame.size >> 8) & 0xFF;

	for(int i = 0;i < size-4;i++) {
		checksum -= *(frame.packet+i);
	}
	return checksum;
}

uint8_t encPacket(uint8_t *packet, uint16_t size) {

	FRAME frame;
	
	frame.size = size + 4;
	frame.packet = packet;
	frame.checksum = genChecksum(frame,frame.size);
	
	sendFrame(frame);
	return 0;	
}

// 0 if ok, -1 if timeout, -2 if checksum mismatch
uint8_t decPacket(uint8_t *pkt, uint16_t *size) {
	FRAME frame;
	uint8_t code = readFrame(&frame);
	if(code !=0 ) {
		*size=0;
		return code;
	}
	*size = frame.size-5;
	pkt = realloc(pkt,*size);
	memcpy(pkt,frame.packet,*size);
	uint16_t checksum = genChecksum(frame,frame.size-1) - 0x55;
	if(checksum!=frame.checksum)
		return -2;
	else
		return code;
}

uint8_t sendFrame(FRAME frame) {
	// printf("_____Frame Sent______\r\n");
	// printf("size:\t\t0x%x\r\n",frame.size);
	// printf("packet:\t\t");
	// printArr(frame.packet,frame.size - 4);
	// printf("\r\n");
	// printf("checksum:\t0x%x\r\n",frame.checksum);
	// printf("\r\n");
	
	sends(frame.size,&hdlr);
	// for(int i = 0;i<frame.size - 4;i++) {
		// sendb(*(frame.packet+i),&hdlr);
	// }
	senda(frame.packet,frame.size-4,&hdlr);
	sends(frame.checksum,&hdlr);
	
	return 0;
}

// return 0 if frame arrived, -1 if timeout
uint8_t readFrame(FRAME *frame) {
	uint8_t byte = 0x00;
	readb(&byte,&hdlr);
	
	clock_t t1,t2;
	t1 = clock();
	while(byte != 0x55) {
		readb(&byte,&hdlr);
		t2 = clock();
		if((double)((t2 - t1)*1000.0/CLOCKS_PER_SEC) >=(double)TIMEOUT) {
			flushCom(&hdlr);
			return -1; // timeout
		}
	}
	
	reads(&((*frame).size),&hdlr);
	
	// allocation of memory for the packet
	(*frame).packet = malloc((*frame).size-5);
	
	for(int i=0;i<(*frame).size-5;i++) {
		readb(((*frame).packet+i),&hdlr);
	}
	reads(&((*frame).checksum),&hdlr);
	
	// printf("received frame size:\t0x%X\r\n",frame.size);
	// printf("received packet:\r\n");
	// printArr(frame.packet,frame.size - 5);
	// printf("\r\n");
	// printf("received frame checksum:\t0x%X\r\n",frame.checksum);
	
	return 0;
}

void printArr(uint8_t array[],uint16_t size) {
	for(int i = 0;i<size;i++) {
		printf("0x%x ",array[i]);
	}
}



