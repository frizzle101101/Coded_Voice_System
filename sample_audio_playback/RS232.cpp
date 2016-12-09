#include <Windows.h>    // Includes the functions for serial communication via RS232
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "RS232.h"


#define EX_FATAL 1
#define BUFSIZE 140
#define SUCESSSEQ "DEADBEEF"
#define FTLERR "FTLERR"
#define WAITTIME 40
#define DEFAULT_COMMPORT "\\\\.\\COM11"
#define COMMPORT10 "\\\\.\\COM10"
#define COMMPORT13 "\\\\.\\COM13"
#define MAX_STACK_SIZE 1000000
#define COMBYTESIZE  8									// Number of bits per frame
#define DEFAULT_BAUDRATE 38400
static HANDLE hCom;										// Pointer to a COM port
COMMTIMEOUTS timeout;								// A commtimout struct variable
static char *port = NULL;
static int baudrate = DEFAULT_BAUDRATE;

HANDLE getCom()
{
	return hCom;
}

// Initializes the port and sets the communication parameters
void initPort() {

	if (!port) {
		port = (char *)malloc(sizeof(char) * strlen(DEFAULT_COMMPORT) + 1);
		strcpy(port, DEFAULT_COMMPORT);
	}
	createPortFile();								// Initializes hCom to point to PORT4 (port 4 is used by USB-Serial adapter on my laptop)
	purgePort();									// Purges the COM port
	SetComParms();									// Uses the DCB structure to set up the COM port
	purgePort(); 
}

// Purge any outstanding requests on the serial port (initialize)
void purgePort() {
	PurgeComm(hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}

void outputToPort(LPCVOID buf1, DWORD szBuf) {
	int i=0;
	DWORD NumberofBytesTransmitted;
	LPDWORD lpErrors=0;
	LPCOMSTAT lpStat=0; 

	i = WriteFile(
		hCom,										// Write handle pointing to COM port
		buf1,										// Buffer size
		szBuf,										// Size of buffer
		&NumberofBytesTransmitted,					// Written number of bytes
		NULL
	);

	printf("%d\n", NumberofBytesTransmitted);
	_sleep(2000);
	// Handle the timeout error
	if (i == 0) {
		printf("\nWrite Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.	
	}
	else
		printf("\nSuccessful transmission, there were %ld bytes transmitted\n", NumberofBytesTransmitted);
}

int inputFromPort(LPVOID *rcvPayload) {
	int i = 0;
	DWORD NumberofBytesRead;
	DWORD dwCommEvent;
	LPDWORD lpErrors = 0;
	LPCOMSTAT lpStat = 0;
	char tmp;
	char serial[MAX_STACK_SIZE];
	LPVOID payload;
	int flag = 0;
	long startTime = 0;
	long endTime = 0;
	long resultTime = 0;

	if (!SetCommMask(hCom, EV_RXCHAR))
		printf("\SetCommMask Error: 0x%x\n", GetLastError());

	if (WaitCommEvent(hCom,&dwCommEvent,NULL))
	{
		if (!flag)
		{
			flag = 1;
			startTime = get_nanos();
		}
		printf("Incoming message receiving in process...\n");
		do
		{
			ReadFile(hCom,              //Handle of the Serial port
				&tmp,                   //Temporary character
				sizeof(tmp),           //Size of TempChar
				&NumberofBytesRead,    //Number of bytes read
				NULL);

			serial[i] = tmp;
			i++;
			if (!(i % 500))
				printf(".");
		} while (NumberofBytesRead > 0);

		printf("\nTotal bytes received!: %d\n", i);

		if (flag)
		{
			endTime = get_nanos();
			resultTime = endTime - startTime;
		}

		printf("\nTotal Transmission Time: %d\n", resultTime);

		payload = malloc(sizeof(char) * i);
		memcpy(payload, serial, i);
		*rcvPayload = payload;
		return i;
	} else {
		printf("\n>No Reception from Partner Com Client!\n");
		return -1;
	}
	
		
}

// Sub functions called by above functions
/**************************************************************************************/

// Set the hCom HANDLE to point to a COM port, initialize for reading and writing, open the port and set securities
void createPortFile() {
	// Call the CreateFile() function 
	hCom = CreateFile(
		port,										// COM port number
		GENERIC_READ | GENERIC_WRITE,				// Open for read and write
		NULL,										// No sharing allowed
		NULL,										// No security
		OPEN_EXISTING,								// Opens the existing com port
		FILE_ATTRIBUTE_NORMAL,						// Do not set any file attributes --> Use synchronous operation
		NULL										// No template
	);
	if (hCom == INVALID_HANDLE_VALUE) {
		printf("\nFatal Error 0x%x: Unable to open\n", GetLastError());
	}
	else {
		printf("\n%s is now open\n", port);
	}
}

static int SetComParms() {
	DCB dcb;										// Windows device control block
	// Clear DCB to start out clean, then get current settings
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(hCom, &dcb))
	{
		printf("\nFatal Error 0x%x: Unable to GetCommState for hCom\n", GetLastError());
		return(0);
	}

	// Set our own parameters from Globals
	dcb.BaudRate = baudrate;						// Baud (bit) rate
	dcb.ByteSize = COMBYTESIZE;					// Number of bits(8)
	dcb.Parity = NOPARITY;									// No parity	
	dcb.StopBits = ONESTOPBIT;						// One stop bit
	if (!SetCommState(hCom, &dcb))
	{
		printf("\nFatal Error 0x%x: Unable to SetCommState for hCom\n", GetLastError());
		return(0);
	}

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	memset((void *)&timeout, 0, sizeof(timeout));
	timeout.ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	timeout.ReadTotalTimeoutMultiplier = 1;			// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	timeout.ReadTotalTimeoutConstant = 1000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	SetCommTimeouts(hCom, &timeout);
	return(1);
}

void setGlobalBaudRate(int *inputBaud)
{
	baudrate = *inputBaud;
}

void setGlobalCommPort(int inputComm)
{
	port = (char *)malloc(sizeof(char) * strlen(DEFAULT_COMMPORT) + 1);

	switch (inputComm) {
		case 11:
			strcpy(port, DEFAULT_COMMPORT);
			break;
		case 10:
			strcpy(port, COMMPORT10);
			break;
		case 13:
			strcpy(port, COMMPORT13);
			break;
		default:
			printf("Invalid Comm port\n");
	}
}

void printGlobalRS232Param(void)
{
	printf("BaudRate %d\n", baudrate);
	printf("Comm port: %s\n", port);
}

static long get_nanos(void) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}