#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "record.h"
#include "playback.h"
#include "console.h"
#include "utilities.h"
#include "RS232.h"
#include "queue.h"
#include "header.h"
#include "prioQueue.h"
#include "bst.h"
#include "rle.h"
#include "menu.h"

#define DEFAULT_RECORD_TIME		2		// seconds to record for
#define DEFAULT_SAMPLES_SEC		8000	// samples per second
#define WAITTIME 40
#define DEFAULT_COMPRESSION_OPTION TRUE_B


int	main(int argc, char *argv[])
{
	char option;
	char *transmitPrio = NULL;
	//short *audio_buff = NULL;
	short *audio_rcv = NULL;
	int sample_sec = DEFAULT_SAMPLES_SEC;
	int record_time = DEFAULT_RECORD_TIME;
	char *stationID = NULL;
	char *targetID = NULL;
	char *msgPriority = NULL;
	long audio_buff_sz = sample_sec * record_time;
	QUEUE *rcvQ;
	NODE *tmp;
	NODE *rcvNode;
	PRIONODE *tmpPQ;
	HEADER *tmpHdr, *rcvHdr;
	void *payload, *rcvPayload;
	FILE *fpw;
	char *audioFile[] = { "prio1-1", "prio4-1", "prio5-1", "prio2-1", "prio3-1", "prio1-2" };
	char prio[6] = { 1, 4, 5, 2, 3, 1 };
	short *tmp_audio_buff;
	unsigned char stations[11] = { 0x00, 0x03, 0x00, 0x04, 0x05, 0x06, 0x07, 0x00, 0x10, 0x13, 0x13 };
	BSTNODE *tmpBstNode;
	BST *phoneBook;
	PRIOQUEUE *rcvPQ;
	FILE *fp;
	int bmpSize;
	char *bmpRaw;
	FILETYPE usrFileType;
	BOOLYN isCompressed = DEFAULT_COMPRESSION_OPTION;
	BOOLYN isHash;
	void *rcvBuf;
	int *commPort = NULL;
	int *baudrate = NULL;
	int rcvByte;
	char input[MAX];
	char *str1 = NULL;
	char helpstat[MAX];
	char *usrText = NULL;
	int record_flag = 0;
	OPTIONS options;
	AUDIO audio_content;

	audio_content.audio_buffer = NULL;
	rcvQ = queue_init();
	tmp = (NODE *)malloc(sizeof(NODE));
	rcvPQ = prioQueue_init();
	phoneBook = bst_init();

	initializeBuffers(sample_sec, record_time, &audio_content, &audio_buff_sz);
	audio_content.sampling_rate = sample_sec;
	audio_content.recording_time = record_time;

	while (1) {
		printf(">");
		gets_s(input);
		for (int i = 0; input[i]; i++) {
			input[i] = tolower(input[i]);
		}
		str1 = strtok(input, " -");
		if (strcmp("help", str1) == 0) {
			options = HELP;
			str1 = strtok(NULL, " -");
			if (str1 == NULL)
				strcpy(helpstat, "help");
			else
				strcpy(helpstat, str1);
		} else if (strcmp("compose", str1) == 0)
			options = COMPOSE;
		else if (strcmp("display", str1) == 0)
			options = DISPLAY;
		else if (strcmp("playback", str1) == 0) {
			str1 = strtok(NULL, " -");
			if (str1 == NULL)
				options = PLAYBACK;
			else if (strcmp("a", str1) == 0) 
				options = PLAYBACKALL;
		} else if (strcmp("receive", str1) == 0)
			options = RECEIVE;
		else if (strcmp("record", str1) == 0) {
			options = RECORD;
		} else if (strcmp("select", str1) == 0) {
			strcpy(helpstat, str1);
			char *tempId = strtok(NULL, " -");
			if (tempId == NULL)
				options = HELP;
			else if (strcmp("r", tempId) == 0)
				options = SELECTR;
			else if (strcmp("s", tempId) == 0)
				options = SELECTS;
			else {
				str1 = strtok(NULL, " -");
				if (str1 == NULL)
					options = HELP;
				else if (strcmp("r", str1) == 0) {
					targetID = tempId;
					options = SELECTRID;
				} else if (strcmp("s", str1) == 0) {
					stationID = tempId;
					options = SELECTSID;
				} else
					options = INVALID;
			}
		} else if (strcmp("send", str1) == 0) {
			strcpy(helpstat, str1);
			str1 = strtok(NULL, " -");
			if (str1 == NULL)
				options = HELP;
			else if (strcmp("a", str1) == 0)
				options = SENDAUDIO;
			else if (strcmp("b", str1) == 0)
				options = SENDBMP;
			else if (strcmp("t", str1) == 0)
				options = SENDTEXT;
			else
				options = INVALID;
		} else if (strcmp("settings", str1) == 0)
			options = SETTINGS;
		else if (strcmp("set", str1) == 0) {
			strcpy(helpstat, str1);
			str1 = strtok(NULL, " -");
			if (str1 == NULL)
				options = INVALID;
			else if (strcmp("cmprs", str1) == 0) {
				str1 = strtok(NULL, " -");
				if (str1 == NULL)
					options = INVALID;
				else if (strcmp("1", str1) == 0)
					isCompressed = TRUE_B;
				else if (strcmp("0", str1) == 0)
					isCompressed = FALSE_B;
				options = SETCMPRS;
			} else if (strcmp("hash", str1) == 0) {
				str1 = strtok(NULL, " -");
				if (str1 == NULL)
					options = HELP;
				else if (strcmp("1", str1) == 0)
					isHash = TRUE_B;
				else if (strcmp("0", str1) == 0)
					isHash = FALSE_B;
				options = SETHASH;
			} else if (strcmp("audioparams", str1) == 0)
				options = SETAUDIOPARAMS;
			else if (strcmp("priority", str1) == 0) {
				char temp = strtol(strtok(NULL, " -"), NULL, 10);
				if (temp == NULL)
					options = SETPRIORITY;
				else {
					transmitPrio = &temp;
					options = SETPRIORITYID;
				}
			} else if (strcmp("baud", str1) == 0) {
				int temp = strtol(strtok(NULL, " -"), NULL, 10);
				if (temp == NULL)
					options = SETBAUD;
				else {
					baudrate = &temp;
					options = SETBAUDID;
				}
			} else if (strcmp("com", str1) == 0) {
				int temp = strtol(strtok(NULL, " -"), NULL, 10);
				if (temp == NULL)
					options = SETCOM;
				else {
					commPort = &temp;
					options = SETCOMID;
				}
			} else
				options = INVALID;
		} else if (strcmp("test", str1) == 0) {
			strcpy(helpstat, str1);
			str1 = strtok(NULL, " -");
			if (str1 == NULL)
				options = HELP;
			else if (strcmp("a", str1) == 0)
				options = TESTAUDIO;
			else if (strcmp("pq", str1) == 0)
				options = TESTPRIQU;
			else if (strcmp("pb", str1) == 0)
				options = TESTPHONEBOOK;
			else if (strcmp("h", str1) == 0)
				options = TESTHEADER;
			else if (strcmp("b", str1) == 0)
				options = TESTBMP;
			else
				options = INVALID;
		}
		else
			options = INVALID;

		switch (options)
		{
		case INVALID:
			printf("Invalid Input/Option. Please Type \"help\" or\n\"help [command]\" for more information\n");
			break;
		case HELP:
			displayHelp(helpstat);
			break;
		case COMPOSE:
			usrText = composeMsg();
			break;
		case DISPLAY:
			if (usrText)
				printf("%s\n", usrText);
			else
				printf("No text entered! Use command (compose) to enter message.\n");
			break;
		case PLAYBACK:
			if (!record_flag) {
				printf("Empty Buffer! Please Record audio before playback\n");
				break;
			} else {
				printf("Playing..wtf\n");
				PlayBuffer(audio_content.audio_buffer, _msize(audio_content.audio_buffer), audio_content.sampling_rate);
				ClosePlayback();
				printf("Press any key to continue..\n");
				fgetc(stdin);
				while (getchar() != '\n');
				break;
			}
		case PLAYBACKALL:
			/* Playback Queue, Priority Queue and Phonebook*/
			printf("Dequeueing Normal Queue\n");
			while ((tmp = dequeue(rcvQ)) != NULL) {
				printf("Deq pointer %p\n", tmp);
				payload_unpack(&rcvHdr, &rcvBuf, tmp->data);

				if (rcvHdr->flags & AUDIO_F)
					PlayBuffer((short *)rcvBuf, rcvHdr->lDataLength, rcvHdr->sampleSec);
			}

			printf("Dequeueing Priority Queue\n");
			while ((tmpPQ = prioDequeue(rcvPQ)) != NULL) {
				while ((tmp = dequeue(tmpPQ->queue)) != NULL) {
					payload_unpack(&rcvHdr, &rcvBuf, tmp->data);

					if (rcvHdr->flags & AUDIO_F)
						PlayBuffer((short *)rcvBuf, rcvHdr->lDataLength, rcvHdr->sampleSec);
				}
			}

			printf("Printing Phonebook\n");
			bst_inorder_traversal(phoneBook->root);
			break;
		case RECEIVE:
			initPort();

			if (rcvByte = inputFromPort(&rcvPayload)) {

				if ((rcvByte != -1) && (rcvByte >= 32)) {
					if (payload_unpack(&rcvHdr, &rcvBuf, rcvPayload)) {
						printf("DETECT ERRONEOUS MESSAGE\n");
						printf("Press any key to continue..\n");
						fgetc(stdin);
						while (getchar() != '\n');
						purgePort();
						CloseHandle(getCom());
						break;
					}

					if (rcvHdr->flags & AUDIO_F) {
						PlayBuffer((short*)rcvBuf, rcvHdr->lDataLength, rcvHdr->sampleSec);
						ClosePlayback();
					}

					if (rcvHdr->flags & BMP_F) {
						FILE *fp;

						fp = fopen("receiveBMP.bmp", "wb");
						fwrite(rcvBuf, sizeof(char), rcvHdr->lDataLength, fp);
						fclose(fp);

						DrawBMP("receiveBMP.bmp", 0, 400);
					}

					if (rcvHdr->flags & TEXT_F) {
						printf("Received Text:\n%s", rcvBuf);
					}

					rcvNode = (NODE *)malloc(sizeof(NODE));
					rcvNode->data = rcvPayload;
					enqueue(rcvQ, rcvNode);

					prioEnqueue(rcvPQ, rcvNode, rcvHdr->priority);

					tmpBstNode = bst_node_init();
					tmpBstNode->data = rcvHdr->senderID;
					bst_insert(&phoneBook->root, tmpBstNode);

					purgePort();
					CloseHandle(getCom());
				}
			}
			else {
				printf("DETECT ERRONEOUS MESSAGE\n");
			}
			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
			break;
		case RECORD:
			initializeBuffers(sample_sec, record_time, &audio_content, &audio_buff_sz);
			RecordBuffer(audio_content.audio_buffer, audio_buff_sz, sample_sec);
			CloseRecording();
			if (!record_flag)
				record_flag = 1;
			break;
		case SELECTR:
			setTargetID(&targetID);
			setGlobalTargetID(targetID);
			break;
		case SELECTS:
			setStationID(&stationID);
			setGlobalStationID(stationID);
			break;
		case SELECTRID:
			setGlobalTargetID(targetID);
			break;
		case SELECTSID:
			setGlobalStationID(stationID);
			break;
		case SENDAUDIO:
			usrFileType = AUDIO_T;
			tmpHdr = header_init(usrFileType, _msize(audio_content.audio_buffer),
				sample_sec, record_time, isCompressed);
			payload = payload_pack(tmpHdr, audio_content.audio_buffer);
			initPort();
			printf("sending %d bytes...\n", _msize(payload));
			outputToPort(payload, _msize(payload));
			purgePort();
			CloseHandle(getCom());
			break;
		case SENDBMP:
			/* Sending BMP*/
			fp = fopen("sample.bmp", "rb");

			if (!fp) {
				printf("Failed to open bmp file\n");
				break;
			}

			fseek(fp, 0, SEEK_END);
			bmpSize = ftell(fp);
			rewind(fp);

			bmpRaw = (char *)malloc(sizeof(char) *bmpSize);

			fread(bmpRaw, sizeof(char), bmpSize, fp);

			usrFileType = BMP_T;
			tmpHdr = header_init(usrFileType, _msize(bmpRaw),
				sample_sec, record_time, isCompressed);

			payload = payload_pack(tmpHdr, bmpRaw);

			initPort();
			printf("sending..%d", _msize(payload));
			outputToPort(payload, _msize(payload));			// Send audio to port
			purgePort();									// Purge the port
			CloseHandle(getCom());							// Closes the handle pointing to the COM port

			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
			break;
		case SENDTEXT:
			if (usrText) {
				usrFileType = TEXT_T;
				tmpHdr = header_init(usrFileType, _msize(usrText),
					sample_sec, record_time, isCompressed);
				payload = payload_pack(tmpHdr, usrText);
				initPort();
				printf("sending %d bytes...\n", _msize(payload));
				outputToPort(payload, _msize(payload));
				purgePort();
				CloseHandle(getCom());
			} else {
				printf("No text entered! Use command (compose) to enter message.\n");
			}
			break;
		case SETTINGS:
			printGlobalSetting();
			printGlobalRS232Param();
			printf("Queue count %d\n", rcvQ->count);
			break;
		case SETCMPRS:
			setGlobalCompression(&isCompressed);
			break;
		case SETHASH:
			setGlobalHash(&isHash);
			break;
		case SETAUDIOPARAMS:
			getNewParam(&sample_sec, &record_time);
			break;
		case SETPRIORITY:
			setPriority(&transmitPrio);
			setGlobalPriority(transmitPrio);
			break;
		case SETPRIORITYID:
			setGlobalPriority(transmitPrio);
			break;
		case SETBAUD:
			setBaudrate(&baudrate);
			setGlobalBaudRate(baudrate);
			break;
		case SETBAUDID:
			setGlobalBaudRate(baudrate);
			break;
		case SETCOM:
			setCommPort(&commPort);
			setGlobalCommPort(*commPort);
			break;
		case SETCOMID:
			setGlobalCommPort(*commPort);
			break;
		case TESTAUDIO:
			/* Audio Packaging Diagnostic */
			printf("Packaging...\n");
			usrFileType = AUDIO_T;
			tmpHdr = header_init(usrFileType, _msize(audio_content.audio_buffer), sample_sec, record_time, isCompressed);
			payload = payload_pack(tmpHdr, audio_content.audio_buffer);

			printf("Unpacking and play...\n");
			if (payload) {
				payload_unpack(&rcvHdr, (void **)&audio_rcv, payload);
				printf("Playing..\n");
				PlayBuffer(audio_rcv, audio_buff_sz, sample_sec);
				ClosePlayback();
			}
			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
			break;
		case TESTPRIQU:
			/* Priority Queue Diagnostic*/
			for (int i = 0; i < 6; i++) {
				printf("%d insert ", i);
				printf("0x%02x\n", prio[i]);
				fpw = fopen(audioFile[i], "rb");
				tmp_audio_buff = (short *)malloc(sizeof(short) *audio_buff_sz);
				fread(tmp_audio_buff, sizeof(short), audio_buff_sz, fpw);
				tmp = (NODE *)malloc(sizeof(NODE));
				printf("main %p\n", tmp);
				tmp->data = tmp_audio_buff;
				printf("audio pointer %p\n", tmp->data);
				PlayBuffer((short *)tmp->data, audio_buff_sz, sample_sec);
				prioEnqueue(rcvPQ, tmp, prio[i]);
				fclose(fpw);
			}
			printPrioQueue(rcvPQ);

			while ((tmpPQ = prioDequeue(rcvPQ)) != NULL) {
				while ((tmp = dequeue(tmpPQ->queue)) != NULL) {
					printf("Deqing %p %p %p\n", tmpPQ->queue, tmp, tmp->data);
					PlayBuffer((short *)tmp->data, audio_buff_sz, sample_sec);
				}
			}
			break;
		case TESTPHONEBOOK:
			/* Phonebook Diagnostic */
			for (int i = 0; i < 11; i++) {
				tmpBstNode = bst_node_init();
				tmpBstNode->data = stations[i];
				bst_insert(&phoneBook->root, tmpBstNode);
			}
			bst_inorder_traversal(phoneBook->root);
			break;
		case TESTHEADER:
			/* Header Diagnostic */
			usrFileType = AUDIO_T;
			tmpHdr = header_init(usrFileType, _msize(audio_content.audio_buffer), sample_sec, record_time, isCompressed);
			print_header(tmpHdr);

			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
			break;
		case TESTBMP:
			/* BMP File Diagnostic*/
			unsigned char *bmpCompressed;
			unsigned char *bmpDecompressed;
			int bmpCompressedSize;
			int bmpDeCompressedSize;
			fp = fopen("sample.bmp", "rb");

			if (!fp) {
				printf("Failed to open bmp file\n");
				break;
			}
			fseek(fp, 0, SEEK_END);
			bmpSize = ftell(fp);
			rewind(fp);

			printf("Original bmp size: %d\n", bmpSize);

			bmpRaw = (char *)malloc(sizeof(char) *bmpSize);

			fread(bmpRaw, sizeof(char), bmpSize, fp);
			fclose(fp);

			RLEcompress((unsigned char *)bmpRaw, _msize(bmpRaw), &bmpCompressed, &bmpCompressedSize);

			printf("Compresssed bmp size: %d\n", bmpCompressedSize);

			RLEdecompress(bmpCompressed, bmpCompressedSize, &bmpDecompressed, &bmpDeCompressedSize);

			fp = fopen("raw.bmp", "wb");
			fwrite(bmpDecompressed, sizeof(char), bmpDeCompressedSize, fp);
			fclose(fp);

			DrawBMP("raw.bmp", 0, 400);

			free(bmpRaw);

			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
			break;
		}
	}
	return(0);
}