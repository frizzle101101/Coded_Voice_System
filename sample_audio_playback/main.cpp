#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "record.h"
#include "playback.h"
#include "console.h"
#include "demo.h"
#include "RS232.h"

#define DEFAULT_RECORD_TIME		2		// seconds to record for
#define DEFAULT_SAMPLES_SEC		8000	// samples per second
#define WAITTIME 40

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
				RecordBuffer(audio_buff, audio_buff_sz, sample_sec);
				CloseRecording();
				initPort();
				outputToPort(audio_buff, audio_buff_sz);			// Send audio to port
				purgePort();									// Purge the port
				CloseHandle(getCom());							// Closes the handle pointing to the COM port
				break;
			case '4':
				initPort();
				//get current time for timeout reference
				SYSTEMTIME timeoutWatch;
				GetSystemTime(&timeoutWatch);
				while (inputFromPort(audio_buff, audio_buff_sz) == 0)					// Receive string from port
				{
					SYSTEMTIME currTime;
					GetSystemTime(&currTime);
					if (currTime.wMinute > timeoutWatch.wMinute)
					{
						if (currTime.wSecond + 60 - WAITTIME > timeoutWatch.wSecond)
						{
							printf("\nTimeout(%d)\n", WAITTIME);
							break;
						}
					}
					else
					{
						if (currTime.wSecond - WAITTIME > timeoutWatch.wSecond)
						{
							printf("\nTimeout(%d)\n", WAITTIME);
							break;
						}
					}
				}
				if (!audio_buff) {
					Errorp("Empty Buffer! Please Record audio before playback\n");
					break;
				}
				else {
					printf("Playing..\n");
					PlayBuffer(audio_buff, audio_buff_sz, sample_sec);
					ClosePlayback();
					break;
				}
				purgePort();									// Purge the port
				CloseHandle(getCom());							// Closes the handle pointing to the COM port
			case '5':
				getNewParam(&sample_sec, &record_time);
				initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMREALLOC);
				break;
			case '6':
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