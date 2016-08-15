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



#include "app.h"

uint8_t appInit(uint8_t comPortNb, uint32_t baudrate) {
	return initCmds(comPortNb, baudrate);
}
uint8_t appDeinit() {
	return deinitCmds();
}

/*	reset the tx	*/
uint8_t appReset() {
	uint8_t data[] = {0x00}; //reset
	encData(data,sizeof(data),CMD_RESET);
	return 0;
}

/*	detect the tx	*/
uint8_t appDetect() {
	uint8_t code = 0xFF;
	//int i = 0;
	//while(code!=0 && i<10) {
		printf("*\tDetection ...\t");
		uint8_t data[] = {}; // detect
		encData(data,sizeof(data),CMD_DETECT);
		
		// receiving ack
		uint8_t *receivedData=malloc(256);
		uint16_t dataSize;
		uint8_t cmdType;
		code=decData(receivedData,&dataSize,&cmdType);
		if(code==0 && cmdType==CMD_DETECT) {
			printf("Firmware version detected: %d.%d\r\n",receivedData[2],receivedData[0]);
		}
		else {
			switch((char)code){
				case -1:
					printf("[FAIL] timeout\r\n");
					break;
				case -2:
					printf("[FAIL] checksum mismatch\r\n");
					break;
			}
		}
			
		//i++;
	//}
	return code;
}

uint8_t appPrepareBlock(uint16_t offset) {
	uint8_t data[12]={	offset & 0xFF,
						(offset>>8) & 0xFF,
						0x00,
						0x09,
						0x00,
						0x00,
						0x00,
						0x00,
						0x00,
						0x00,
						0x00,
						0x00};
	
	// receiving ack
	uint8_t *receivedData=malloc(256);
	uint16_t dataSize;
	uint8_t cmdType = 0x00;
	uint8_t code = -1;
	
	while(code!=0 || cmdType!=CMD_PREP) {
		//sending the cmd
		// printf("*\tPreparing 1KB block at offset 0x%X ...\t",offset);
		encData(data,sizeof(data),CMD_PREP);
		//receiving
		code = decData(receivedData,&dataSize,&cmdType);
		switch((char)code){
			case 0:
				if(cmdType != CMD_PREP)
					printf("[FAIL] bad command type received\r\n");
				else
					// printf("done\r\n");
				break;
			case -1:
				printf("[FAIL] timeout\r\n");
				break;
			case -2:
				printf("[FAIL] checksum mismatch\r\n");
				break;
			default:
				printf("[FAIL]\r\n");
				break;
		}
		// printf("*\t\treceived:\t");
		// printArr(receivedData,dataSize);
		// printf("\r\n");
	}
	// ok
	
	return 0;
}

uint8_t appWritePage(uint8_t *data,uint16_t size, uint32_t offset) {
	
	uint8_t buf[size+6];
	// receiving ack
	uint8_t *receivedData=malloc(256);
	uint16_t dataSize;
	uint8_t cmdType=0x00;
	uint8_t code=0xFF;
	
	buf[0]=offset & 0xFF;
	buf[1]=(offset>>8) & 0xFF;
	buf[2]=(offset>>16) & 0xFF;
	buf[3]=(offset>>24) & 0xFF;
	buf[4]=size & 0xFF;
	buf[5]=(size>>8) & 0xFF;
	memcpy(buf+6,data,size);
	
	while(code!=0 || cmdType!=CMD_WRITE) {
		//sending the cmd
		// printf("*\tWriting 0x%X bytes at offset 0x%X ...\t",size,offset);
		encData(buf,sizeof(buf),CMD_WRITE);
		//receiving
		code = decData(receivedData,&dataSize,&cmdType);
		switch((char)code){
			case 0:
				if(cmdType != CMD_WRITE)
					printf("[FAIL] bad command type received\r\n");
				else
					// printf("done\r\n");
				break;
			case -1:
				printf("[FAIL] timeout\r\n");
				break;
			case -2:
				printf("[FAIL] checksum mismatch\r\n");
				break;
			default:
				printf("[FAIL]\r\n");
				break;
		}
		// printf("*\t\treceived:\t");
		// printArr(receivedData,dataSize);
		// printf("\r\n");
	}
	return 0;
}
