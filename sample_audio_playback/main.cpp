#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "record.h"
#include "playback.h"
#include "console.h"
#include "demo.h"

#define DEFAULT_RECORD_TIME		2		// seconds to record for
#define DEFAULT_SAMPLES_SEC		8000	// samples per second

int	main(int argc, char *argv[])
{
	char option;
	short *audio_buff = NULL;
	int sample_sec = DEFAULT_SAMPLES_SEC;
	int record_time = DEFAULT_RECORD_TIME;
	long audio_buff_sz = sample_sec * record_time;

	initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMALLOC);

	while (1) {
		menu(sample_sec, record_time);

		option = fgetc(stdin);
		// Flushing \n character in stdin
		while (getchar() != '\n');

		system("CLS");
		
		switch (option) {
			case '1':
				RecordBuffer(audio_buff, audio_buff_sz, sample_sec);
				CloseRecording();
				break;
			case '2':
				if (!audio_buff) {
					Errorp("Empty Buffer! Please Record audio before playback\n");
					break;
				} else {
					printf("Playing..\n");
					PlayBuffer(audio_buff, audio_buff_sz, sample_sec);
					ClosePlayback();
					break;
				}
			case '3':
				getNewParam(&sample_sec, &record_time);
				initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMREALLOC);
				break;
			case '4':
				break;
			default:
				printf("Please Enter a valid option 1-4!\n");
				_sleep(2000);
		}

		if (option == '4')
			break;

		system("CLS");
	}
	return(0);
}