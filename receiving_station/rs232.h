#pragma once

#include <Windows.h>    // Includes the functions for serial communication via RS232
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Prototype the functions to be used
void initPort();
void purgePort();
void inputFromPort(LPVOID buf, DWORD szBuf);
void free_port();

// Sub functions
void createPortFile();
static int SetComParms();