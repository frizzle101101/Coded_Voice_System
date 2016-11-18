#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include "demo.h"

#define MAX_RECORD_SEC 64
#define MAX_TMP_BUFF 8
void menu(int sample_sec, int record_time) 
{
	printf("Welcome to Audio Playback Demo!\n");
	printf("Please enter the follow option:\n");
	printf("1 - Record Audio\n");
	printf("2 - Play Audio\n");
	printf("3 - Record and Transmit Audio\n");
	printf("4 - Recieve and Play Audio\n");
	printf("5 - Change Parameters\n");
	printf("6 - Exit\n");
	printf("\n\nParameters: sample second = %d, record second = %d\n", sample_sec, record_time);
}

void cp_menu() 
{
	printf("1 - Change SAMPLE SECOND\n");
	printf("2 - Change Record Time\n");
	printf("3 - Back to Main Menu\n");
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
		printf("Please enter sample second\n");
		fgets(tmp, MAX_TMP_BUFF, stdin);
		*record_time = (int)strtol(tmp, &ptr, 10);
		system("CLS");
	} while ((*record_time <= 0) && (*record_time > 64));
}


void initializeBuffers (int sample_sec, int record_time, short **audio_buff, long *audio_buff_sz, ALLOC_TYPE type)
{
	(*audio_buff_sz) = sample_sec * record_time;

	if (type == MEMALLOC)
		*audio_buff = (short *)malloc(sizeof(short) * (*audio_buff_sz));
	else 
		*audio_buff = (short *)realloc(*audio_buff, sizeof(short) * (*audio_buff_sz));
}
