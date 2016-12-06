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
#include "prioQueue.h"

#define DEFAULT_RECORD_TIME		2		// seconds to record for
#define DEFAULT_SAMPLES_SEC		8000	// samples per second
#define WAITTIME 40

int	main(int argc, char *argv[])
{
	char option;
	char transmitPrio = 0;
	short *audio_buff = NULL;
	short *audio_rcv = NULL;
	int sample_sec = DEFAULT_SAMPLES_SEC;
	int record_time = DEFAULT_RECORD_TIME;
	char *stationID = NULL;
	long audio_buff_sz = sample_sec * record_time;
	QUEUE *rcvQ;
	NODE *tmp;
	NODE *rcvNode;
	PRIONODE *tmpPQ;
	HEADER *tmpHdr, *rcvHdr;
	void *payload, *rcvPayload;
	FILE *fpw;
	char *audioFile[] = { "prio1-1", "prio4-1", "prio5-1", "prio2-1", "prio3-1", "prio1-2"};
	char prio[6] = { 1, 4, 5, 2, 3, 1};
	short *tmp_audio_buff;


	PRIOQUEUE *rcvPQ;

	rcvQ = queue_init();
	tmp = (NODE *)malloc(sizeof(NODE));
	rcvPQ = prioQueue_init();


	initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMALLOC);
	int i = 0;

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
				/*
				fpw = fopen(audioFile[i], "w+b");
				fwrite(audio_buff, sizeof(short), audio_buff_sz, fpw);
				fclose(fpw);
				i++;*/
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
				tmpHdr = header_init(&transmitPrio);
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

				inputFromPort(&rcvPayload);

				if (payload_unpack(&rcvHdr, &audio_rcv, rcvPayload)) {
					printf("DETECT ERRONEOUS MESSAGE\n");
					_sleep(3000);
					purgePort();									
					CloseHandle(getCom());							
					break;
				}

				PlayBuffer(audio_rcv, audio_buff_sz, sample_sec);
				ClosePlayback();

				rcvNode = (NODE *)malloc(sizeof(NODE));
				rcvNode->data = audio_rcv;
				enqueue(rcvQ, rcvNode);

				purgePort();
				CloseHandle(getCom());
				break;
			case '5':
				getNewParam(&sample_sec, &record_time);
				setPriority(&transmitPrio);
				initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMREALLOC);
				break;
			case '6':
				printf("Packaging...\n");
				tmpHdr = header_init(&transmitPrio);
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
			case '7':
				printf("Dequeueing\n");
				while ((tmp = dequeue(rcvQ)) != NULL)
					PlayBuffer((short *)tmp->data, audio_buff_sz, sample_sec);
				break;
			case '8':
				for (i = 0; i < 6; i++) {
					printf("%d insert ", i);
					printf("0x%02x\n", prio[i]);
					fpw = fopen(audioFile[i], "rb");
					tmp_audio_buff = (short *)malloc(sizeof(short) *audio_buff_sz);
					fread(tmp_audio_buff, sizeof(short), audio_buff_sz, fpw);
					tmp = (NODE *)malloc(sizeof(NODE));
					printf("main %p\n", tmp);
					tmp->data = tmp_audio_buff;
					printf("audio pointer %p\n", tmp->data);
					//PlayBuffer((short *)tmp->data, audio_buff_sz, sample_sec);
					prioEnqueue(rcvPQ, tmp, prio[i]);
					fclose(fpw);
				}
				printPrioQueue(rcvPQ);

				while ((tmpPQ = prioDequeue(rcvPQ)) != NULL) {
					printf("hello\n");
					while ((tmp = dequeue(tmpPQ->queue)) != NULL) {
						printf("Deqing %p %p %p\n", tmpPQ->queue, tmp, tmp->data);
						PlayBuffer((short *)tmp->data, audio_buff_sz, sample_sec);
					}
				}
				break;
			default:
				printf("Please Enter a valid option 1-4!\n");
				_sleep(2000);
		}

		system("CLS");
	}
	return(0);
}