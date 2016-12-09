#pragma once

// Prototype the functions to be used

void initPort(); 
void purgePort();
void outputToPort(LPCVOID buf, DWORD szBuf);
int inputFromPort(LPVOID *rcvPayload);

// Sub functions
void createPortFile();
static int SetComParms();
HANDLE getCom(void);
void setGlobalCommPort(int inputComm);
void setGlobalBaudRate(int *inputBaud);
void printGlobalRS232Param(void);
static long get_nanos(void);