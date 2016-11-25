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