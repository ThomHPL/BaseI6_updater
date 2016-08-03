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

#include "serial.h"
#undef _DEBUG


static COMMTIMEOUTS g_cto = {
    MAX_WAIT_READ,  /* ReadIntervalTimeOut          */
    0,              /* ReadTotalTimeOutMultiplier   */
    MAX_WAIT_READ,  /* ReadTotalTimeOutConstant     */
    0,              /* WriteTotalTimeOutMultiplier  */
    0               /* WriteTotalTimeOutConstant    */
};

static DCB g_dcb = {
    sizeof(DCB),        /* DCBlength            */
    115200,             /* BaudRate             */
    TRUE,               /* fBinary              */
    FALSE,              /* fParity              */
    FALSE,              /* fOutxCtsFlow         */
    FALSE,              /* fOutxDsrFlow         */
    DTR_CONTROL_DISABLE, /* fDtrControl          */
    FALSE,              /* fDsrSensitivity      */
    FALSE,              /* fTXContinueOnXoff    */
    FALSE,              /* fOutX                */
    FALSE,              /* fInX                 */
    FALSE,              /* fErrorChar           */
    FALSE,              /* fNull                */
    RTS_CONTROL_DISABLE, /* fRtsControl          */
    FALSE,              /* fAbortOnError        */
    0,                  /* fDummy2              */
    0,                  /* wReserved            */
    0x100,              /* XonLim               */
    0x100,              /* XoffLim              */
    8,                  /* ByteSize             */
    NOPARITY,           /* Parity               */
    ONESTOPBIT,         /* StopBits             */
    0x11,               /* XonChar              */
    0x13,               /* XoffChar             */
    '?',                /* ErrorChar            */
    0x1A,               /* EofChar              */
    0x10                /* EvtChar              */
};

uint8_t openCom(uint32_t comPortNb,DWORD baudrate,HANDLE *hdlr) {

    #ifdef _DEBUG
    printf("[DEBUG] Connecting to COM%d at %d bauds : " ,comPortNb,(int)baudrate);
    #endif

    char comPortName[16];

    sprintf(comPortName, "COM%d", comPortNb);
	
    *hdlr = CreateFile(comPortName,
						GENERIC_READ|GENERIC_WRITE,
						0,
						NULL,
                        OPEN_EXISTING,
						//FILE_ATTRIBUTE_NORMAL,
						0,
						NULL);
	
    if(*hdlr == INVALID_HANDLE_VALUE)
    {
        #ifdef _DEBUG
        printf("ERROR - invalid handle (COM%d)\r\n", comPortNb);
        #endif
        return GetLastError(); // invalid handle error
    }

    // size of RX and TX buffers
    SetupComm(*hdlr, RX_SIZE, TX_SIZE);

    // com port config
    g_dcb.BaudRate=baudrate;
    if(!SetCommTimeouts(*hdlr, &g_cto) || !SetCommState(*hdlr, &g_dcb))
    {
        #ifdef _DEBUG
        printf("ERROR - setting timeouts (COM%d)\r\n", comPortNb);
        #endif
        CloseHandle(*hdlr);
        return -2;	// setting timeouts error
    }

    // empty the buffers
    PurgeComm(*hdlr, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);

    #ifdef _DEBUG
    printf("OK\r\n");
    #endif

    return 0;
}
uint8_t closeCom(HANDLE *hdlr) {
    CloseHandle(*hdlr);
    return 0;
}
uint8_t flushCom(HANDLE *hdlr) {
	// empty the buffers
    PurgeComm(*hdlr, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
	return 0;
}

uint8_t sendb(uint8_t byte, HANDLE *hdlr) {
    PDWORD nBytesWritten=0;
    if(!WriteFile(*hdlr, &byte, 1, nBytesWritten, NULL))
    {
        return -1;	// error
    }
	else {
		//printf("byte sent:\t0x%X\r\n",byte);
		return 0;	// ok
	}
}
uint8_t sends(uint16_t shrt, HANDLE *hdlr) {
	sendb((uint8_t)(shrt & 0xFF),hdlr);
	sendb((uint8_t)((shrt>>8) & 0xFF),hdlr);
	return 0;
}
uint8_t sendi(uint32_t val, HANDLE *hdlr) {
	sendb((uint8_t)(val & 0xFF),hdlr);
	sendb((uint8_t)((val>>8) & 0xFF),hdlr);
	sendb((uint8_t)((val>>16) & 0xFF),hdlr);
	sendb((uint8_t)((val>>24) & 0xFF),hdlr);
	return 0;
}

uint8_t senda(uint8_t *array, uint16_t size, HANDLE *hdlr) {
	PDWORD nBytesWritten=0;
    if(!WriteFile(*hdlr, array, size, nBytesWritten, NULL)) {
        return -1;	// error
    }
	else {
		return 0;	// ok
	}
}

uint8_t readb(uint8_t *byte,HANDLE *hdlr) {
    PDWORD nBytesRead=0;
    if(!ReadFile(*hdlr, byte, 1, nBytesRead, NULL)) {
		return -1;	// error
	}
	else
		return 0;	// ok
}
uint8_t reads(uint16_t *shrt, HANDLE *hdlr) {
	*shrt = 0x0000;
	uint8_t byte;
	if(readb(&byte,hdlr)!=0)
		return -1;
	*shrt += byte;
	if(readb(&byte,hdlr)!=0)
		return -1;
	*shrt += (uint16_t)byte<<8;
	return 0;
}

