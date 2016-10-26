#pragma once

typedef enum {
	MEMALLOC,
	MEMREALLOC
} ALLOC_TYPE;

void initializeBuffers(int sample_sec, int record_time, short **audio_buff, long *audio_buff_sz, ALLOC_TYPE type);
void getNewParam(int *sample_sec, int *record_time);
void menu(int sample_sec, int record_time);
void cp_menu();