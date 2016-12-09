#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include "utilities.h"

#define MAX_RECORD_SEC 64
#define MAX_TMP_BUFF 8
void menu(int sample_sec, int record_time, int qCount) 
{
	printf("Welcome to Audio Playback Demo!\n");
	printf("Please enter the follow option:\n");
	printf("1 - Record Audio\n");
	printf("2 - Play Audio\n");
	printf("3 - Transmit Audio\n");
	printf("4 - Recieve Mode\n");
	printf("5 - Change Parameters\n");
	printf("7 - Packaging Diagnostic\n");
	printf("8 - Exit\n");
	printf("\n\nParameters: sample second = %d, record second = %d\n", sample_sec, record_time);
	printf("Number of message in Queue %d\n", qCount);
}

void cp_menu() 
{
	printf("Set Parameters\n");
	printf("1 - Change SAMPLE SECOND\n");
	printf("2 - Change Record Time\n");
	printf("3 - Change COM Port\n");
	printf("4 - Change Baud Rate\n");
	printf("5 - Back to Main Menu\n");
}

void getNewParam(int *sample_sec, int *record_time)
{
	char tmp[MAX_TMP_BUFF];
	char *ptr;
	char c;

	do {
		printf("Please enter sample second\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		*sample_sec = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((*sample_sec <= 0) && (*sample_sec > 64));

	do {
		printf("Please enter record time\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		*record_time = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((*record_time <= 0) && (*record_time > 64));
}

void setPriority(char **prio)
{
	char tmp[MAX_TMP_BUFF];
	char *ptr;

	if (!*prio) {
		*prio = (char *)malloc(sizeof(char));
	}
	do {
		printf("Please enter transmit priority from 0 - 255 (0 - highest priorty)\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		**prio = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((**prio <= 0) && (**prio > 255));
}

void setStationID(char **stationID)
{
	char tmp[MAX_TMP_BUFF];
	char *ptr;

	if (!*stationID) {
		*stationID = (char *)malloc(sizeof(char));
	}

	do {
		printf("Please enter stationID from 0 - 255\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		**stationID = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((**stationID <= 0) && (**stationID > 255));
}


void setTargetID(char **targetID)
{
	char tmp[MAX_TMP_BUFF];
	char *ptr;

	if (!*targetID) {
		*targetID = (char *)malloc(sizeof(char));
	}

	do {
		printf("Please enter targetID from 0 - 255\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		**targetID = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((**targetID <= 0) && (**targetID > 255));
}

char setCompression(void)
{
	char tmp;
	do {
		printf("Set Huffman Compression (y/n)\n");
		tmp = fgetc(stdin);
		system("CLS");
	} while ((tmp != 'y') && (tmp!= 'n'));

	while (getchar() != '\n');
	return tmp;
}

void setBaudrate(int **baudrate)
{
	char tmp[MAX_TMP_BUFF];
	char *ptr;

	if (!*baudrate) {
		*baudrate = (int *)malloc(sizeof(int));
	}
	do {
		printf("Please enter Baudrate (Use standard baudrate)\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		**baudrate = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((**baudrate != 300) && (**baudrate != 600) && (**baudrate != 1200) && (**baudrate != 2400) && (**baudrate != 4800) 
		     && (**baudrate != 9600) && (**baudrate != 19200) && (**baudrate != 19200) && (**baudrate != 38400)
		     && (**baudrate != 57600) && (**baudrate != 115200));
}

void setCommPort(int **commPort)
{
	char tmp[MAX_TMP_BUFF];
	char *ptr;

	if (!*commPort) {
		*commPort = (int *)malloc(sizeof(int));
	}
	do {
		printf("Please enter CommPort (10/11/13)\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		**commPort = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((**commPort != 11) && (**commPort != 10) && (**commPort != 13));
}

void initializeBuffers (int sample_sec, int record_time, short **audio_buff, long *audio_buff_sz, ALLOC_TYPE type)
{
	(*audio_buff_sz) = sample_sec * record_time;

	if (type == MEMALLOC)
		*audio_buff = (short *)malloc(sizeof(short) * (*audio_buff_sz));
	else 
		*audio_buff = (short *)realloc(*audio_buff, sizeof(short) * (*audio_buff_sz));
}
