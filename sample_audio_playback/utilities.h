#pragma once

typedef enum {
	MEMALLOC,
	MEMREALLOC
} ALLOC_TYPE;

void initializeBuffers(int sample_sec, int record_time, short **audio_buff, long *audio_buff_sz, ALLOC_TYPE type);
void getNewParam(int *sample_sec, int *record_time);
void setPriority(char **prio);
void setStationID(char **stationID);
void setTargetID(char **targetID);
char setCompression(void);
void setCommPort(int **commPort);
void setBaudrate(int **baudrate);
void menu(int sample_sec, int record_time, int qCount);
void cp_menu();