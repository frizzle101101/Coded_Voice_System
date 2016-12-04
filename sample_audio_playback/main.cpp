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
#include "queue.h"
#include "header.h"

#define DEFAULT_RECORD_TIME		2		// seconds to record for
#define DEFAULT_SAMPLES_SEC		8000	// samples per second
#define WAITTIME 40

int	main(int argc, char *argv[])
{
	char option;
	short *audio_buff = NULL;
	short *audio_rcv = NULL;
	int sample_sec = DEFAULT_SAMPLES_SEC;
	int record_time = DEFAULT_RECORD_TIME;
	char *stationID = NULL;
	long audio_buff_sz = sample_sec * record_time;
	unsigned int rcvStatus = 0;
	QUEUE *rcvQ;
	NODE *tmp;
	HEADER *tmpHdr, *rcvHdr;
	void *payload, *rcvPayload;

	rcvQ = queue_init();
	tmp = (NODE *)malloc(sizeof(NODE));

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
				tmpHdr = header_init();
				payload = payload_pack(tmpHdr, audio_buff);
				//payload_unpack(&rcvHdr, &audio_rcv, payload);
				//PlayBuffer(audio_rcv, audio_buff_sz, sample_sec);
				initPort();
				printf("sending..%d", _msize(payload));
				outputToPort(payload, _msize(payload));			// Send audio to port
				purgePort();									// Purge the port
				CloseHandle(getCom());							// Closes the handle pointing to the COM port
				break;
			case '4':
				initPort();
				printf("Press ESC to return to menu\n");
				while ((rcvStatus = inputFromPort(&rcvPayload)) == 0) {}					// Receive string from port


				if (payload_unpack(&rcvHdr, &audio_rcv, rcvPayload)) {
					printf("DETECT ERRONEOUS MESSAGE\n");
					_sleep(3000);
					break;
				}

				PlayBuffer(audio_rcv, audio_buff_sz, sample_sec);
				ClosePlayback();

				if (rcvStatus) {
					tmp = (NODE *)malloc(sizeof(NODE));
					tmp->data = audio_buff;
					enqueue(rcvQ, tmp);
				}
				purgePort();									// Purge the port
				CloseHandle(getCom());							// Closes the handle pointing to the COM port
				break;
			case '5':
				getNewParam(&sample_sec, &record_time);
				initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMREALLOC);
				break;
			case '6':
				printf("Packaging...\n");
				tmpHdr = header_init();
				payload = payload_pack(tmpHdr, audio_buff);

				printf("Unpacking and play...\n");
				if (payload) {
					payload_unpack(&rcvHdr, &audio_rcv, payload);
					printf("Playing..\n");
					PlayBuffer(audio_rcv, audio_buff_sz, sample_sec);
					ClosePlayback();
				}
				_sleep(2000);
				break;
			default:
				printf("Please Enter a valid option 1-4!\n");
				_sleep(2000);
		}

		system("CLS");
	}
	return(0);
}