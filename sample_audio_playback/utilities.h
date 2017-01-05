#pragma once

typedef struct audio_adt{
	short *audio_buffer;
	int sampling_rate;
	int recording_time;
}AUDIO;

int initializeBuffers(int sample_sec, int record_time, AUDIO *curr_audio, long *audio_buff_sz);
void getNewParam(int *sample_sec, int *record_time);
void setPriority(char **prio);
void setStationID(char **stationID);
void setTargetID(char **targetID);
char setCompression(void);
void setCommPort(int **commPort);
void setBaudrate(int **baudrate);
char setHashCheck(void);
void menu(int sample_sec, int record_time, int qCount);
void cp_menu();
void displayHelp(char *fileName);
char *composeMsg(void);