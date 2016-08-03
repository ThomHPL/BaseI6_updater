// Copyright Thomas Herpoel 2015
//
// This file is part of FSi6_updater
//
// FSi6_updater is free software: you can redistribute it and/or modify
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


#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <conio.h>
#include <sys/stat.h>

#include "app.h"

uint8_t printHelp(void);
uint8_t detect(void);
uint8_t verify(void);
uint8_t upload(void);
uint8_t erase(void);
uint8_t reset(void);

uint8_t readPage(uint8_t page[], uint16_t nbytes,FILE* file,uint16_t offset);

char filePath[512];
BOOL WITH_BTLDR=FALSE;

/*
 * -n __ : com port number
 * -b __ : baudrate
 * -p __ : path to flash image
 * -h    : print help and exit
 * -a __ : action
 * 	  -d : detect
 * 	  -v : verify flash image CRC_CCITT
 *	  -u : flash
 *    -e : erase program memory
 *	  -r : reset 
*/

int main(int argc, char *argv[]) {
	
	uint8_t comPortNb = 1;
	uint32_t baudrate = 115200;
	
	uint8_t (*function)(void);
	function = printHelp;
	
	// parsing command line arguments
	for(int i = 1;i<argc;i++) {
		if(strcmp(argv[i],"-n")==0) {
			comPortNb = (uint8_t)atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-b")==0) {
			baudrate = (uint32_t)atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-p")==0) {
			strcpy(filePath,argv[i+1]);
			//printf("%s\r\n",filePath);
			i++;
		}
		else if(strcmp(argv[i],"-h")==0) {
			function = printHelp;
			return 0;
		}
		else if(strcmp(argv[i],"-a")==0) {
			if(strcmp(argv[i+1],"-d")==0) {
				function = detect;
			}
			else if(strcmp(argv[i+1],"-v")==0) {
				function = verify;
			}
			else if(strcmp(argv[i+1],"-u")==0) {
				function = upload;
			}
			else if(strcmp(argv[i+1],"-e")==0) {
				function = erase;
			}
			else if(strcmp(argv[i+1],"-r")==0) {
				function = reset;
			}
			else {
				printf("Invalid -a option, exit. \r\n");
				return 0;
			}
			i++;
		}
	}
	
	printf("*Opening COM%d at %d bps:\r\n",comPortNb,baudrate);
	if(appInit(comPortNb,baudrate)!=0) {
		printf("*\t\t\tERROR\r\n");
		return 0;
	}
	printf("*\t\t\tSUCCESS\r\n");
	
	// execute function
	(*function)();
	
	printf("*Closing COM%d at %d bps:\r\n",comPortNb,baudrate);
	if(appDeinit()!=0) {
		printf("*\t\t\tERROR\r\n");
		return 0;
	}
	printf("*\t\t\tSUCCESS\r\n");
	
	return 0;
}

uint8_t printHelp(void){
	printf("\r\n");
	printf("_________UPDATER BASEI6 HELP_________\r\n");
	printf("-n __ : com port number\r\n"); 				//ok
	printf("-b __ : com port baudrate\r\n");				//ok
	printf("-p __ : path to flash image\r\n");			//ko
	printf("-h    : print help and quit\r\n");			//ok
	printf("-a __ : action to perform :\r\n");			//ok
	printf("   -d : detect transmitter\r\n");				//ok
	printf("   -v : verify flash image CRC CCITT\r\n");	//ko
	printf("   -u : upload flash image\r\n");				//ko
	printf("   -r : reset transmitter\r\n");				//ok
	printf("_____________________________________\r\n\r\n");
	return 0;
}

uint8_t detect(void) {
	printf("*Detecting i6 transmitter:\r\n");
	if(appDetect()!=0) {
		printf("*\t\t\tERROR\r\n");
		return -1;
	}
	printf("*\t\t\tSUCCESS\r\n");
	return 0;
}

uint8_t verify(void) {
	printf("*Verifying flash image CRC:\r\n");
	printf("*\t\t\tNOT IMPLEMENTED YET\r\n");
	return 0;
}

uint8_t upload(void) {
	if(detect()!=0) {
		return -1;
	}
	if(verify()!=0) {
		return -2;
	}
	printf("*Uploading:\t\t%s\r\n",filePath);
	printf("*\tOpening file\t");
	
	FILE* file = NULL;
	file = fopen(filePath, "rb");
	if(file==NULL)
		printf("[FAIL]\r\n");
	else
		printf("done\r\n");
	
	struct stat st;
	stat(filePath, &st);
	uint16_t fileSize = st.st_size;
	printf("*\tFile size\t0x%X bytes\r\n",fileSize);
	
	
	uint16_t start = 0x1800;
	uint16_t finish = 0x1800+fileSize;
	printf("*\t");
	for(int offset=start;offset<finish;offset+=0x80) {
		if(offset%0x400 == 0) {
			printf("_");
		}
	}
	printf("\r\n*\t");
	
	uint16_t bytesRemaining = fileSize;
	uint8_t data[512];
	uint16_t lastOffset=0x1800;
	for(int offset=start;offset<finish-0x80;offset+=0x80) {
		if(WITH_BTLDR)
			readPage(data,0x80,file,offset);
		else
			readPage(data,0x80,file,offset-0x1800);
		
		if(offset%0x400 == 0) {
			if(appPrepareBlock(offset)!=0) {
				return -1;
			}
			else
				printf("%c",178);
		}
		appWritePage(data,0x80, offset);
		bytesRemaining -= 0x80;
		lastOffset=(uint16_t)offset+0x80;
	}
	printf("\r\n");
	//printf("writing 0x%X bytes at 0x%X\r\n",bytesRemaining,lastOffset);
	if(WITH_BTLDR)
		readPage(data,bytesRemaining,file,lastOffset);
	else
		readPage(data,bytesRemaining,file,lastOffset-0x1800);
	
	appWritePage(data,bytesRemaining,lastOffset);

	printf("*\t\t\tSUCCESS\r\n");
	fclose(file);

	
	reset();
	
	return 0;
}

uint8_t readPage(uint8_t page[], uint16_t nbytes,FILE* file,uint16_t offset)
{
	fseek(file,offset,0);

    for(int i=0;i<nbytes;i++) {
        // printf("index : %x\n",(int)ftell(file));
        page[i]=(uint8_t)fgetc(file);
    }
    return 0;
}


uint8_t erase(void) {
	uint16_t start = 0x1800;
	uint16_t finish = 0xFFFF;
	
	if(detect()!=0) {
		return -1;
	}
	printf("*Erasing:\r\n*\t");
	uint8_t blankPage[0x80];
	for(int i =0;i<sizeof(blankPage);i++) {
		blankPage[i]=0xFF;
	}
	for(int offset=start;offset<finish;offset+=0x80) {
		if(offset%0x400 == 0) {
			printf("_");
		}
	}
	printf("\r\n*\t");
	for(int offset=start;offset<finish;offset+=0x80) {
		if(offset%0x400 == 0) {
			if(appPrepareBlock(offset)!=0) {
				return -1;
			}
			else
				printf("%c",178);
		}
		appWritePage(blankPage,sizeof(blankPage), offset);
	}
	
	printf("\r\n*\t\t\tSUCCESS\r\n");
	return 0;
}

uint8_t reset(void) {
	printf("*Reseting i6 transmitter:\r\n");
	if(appReset()!=0) {
		printf("*\t\t\tERROR\r\n");
		return -1;
	}
	printf("*\t\t\tSUCCESS\r\n");
	return 0;
}


