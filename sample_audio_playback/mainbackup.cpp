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

#define DEFAULT_RECORD_TIME		1		// seconds to record for
#define DEFAULT_SAMPLES_SEC		8000	// samples per second
#define WAITTIME 40
#define DEFAULT_COMPRESSION_OPTION TRUE_B
#undef ISMAIN
#ifdef ISMAIN
int	main(int argc, char *argv[])
{
	char option;
	char *transmitPrio = NULL;
	short *audio_buff = NULL;
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

	rcvQ = queue_init();
	tmp = (NODE *)malloc(sizeof(NODE));
	rcvPQ = prioQueue_init();
	phoneBook = bst_init();

	initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMALLOC);
	int i = 0;

	while (1) {
		menu(sample_sec, record_time, rcvQ->count);

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
			}
			else {
				printf("Playing..\n");
				PlayBuffer(audio_buff, _msize(audio_buff), sample_sec);
				printf("%d %d\n", audio_buff_sz, sample_sec);
				ClosePlayback();
				printf("Press any key to continue..\n");
				fgetc(stdin);
				while (getchar() != '\n');
				break;
			}
		case '3':
			usrFileType = AUDIO_T;
			tmpHdr = header_init(usrFileType, _msize(audio_buff),
				sample_sec, record_time, isCompressed);
			payload = payload_pack(tmpHdr, audio_buff);
			initPort();
			printf("sending %d bytes...", _msize(payload));
			outputToPort(payload, _msize(payload));
			purgePort();
			CloseHandle(getCom());
			break;
		case '4':
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
		case '5':
			if (setCompression() == 'n')
				isCompressed = FALSE_B;
			else
				isCompressed = TRUE_B;
			setGlobalCompression(&isCompressed);
			getNewParam(&sample_sec, &record_time);
			initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMREALLOC);
			setPriority(&transmitPrio);
			setGlobalPriority(transmitPrio);
			setStationID(&stationID);
			setGlobalStationID(stationID);
			setTargetID(&targetID);
			setGlobalTargetID(targetID);
			setBaudrate(&baudrate);
			setGlobalBaudRate(baudrate);
			setCommPort(&commPort);
			setGlobalCommPort(*commPort);

			if (setHashCheck() == 'n')
				isHash = FALSE_B;
			else
				isHash = TRUE_B;

			setGlobalHash(&isHash);
			break;
		case '6':
			/* Audio Packaging Diagnostic */
			printf("Packaging...\n");
			usrFileType = AUDIO_T;
			tmpHdr = header_init(usrFileType, _msize(audio_buff), sample_sec, record_time, isCompressed);
			payload = payload_pack(tmpHdr, audio_buff);

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
		case '7':
			/* Display Queue, Priority Queue and Phonebook*/
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
		case '8':
			/* Priority Queue Diagnostic*/
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
		case '9':
			/* Phonebook Diagnostic */
			for (i = 0; i < 11; i++) {
				tmpBstNode = bst_node_init();
				tmpBstNode->data = stations[i];
				bst_insert(&phoneBook->root, tmpBstNode);
			}
			bst_inorder_traversal(phoneBook->root);
			break;
		case 'a':
			/* Header Diagnostic */
			usrFileType = AUDIO_T;
			tmpHdr = header_init(usrFileType, _msize(audio_buff), sample_sec, record_time, isCompressed);
			print_header(tmpHdr);

			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
			break;
		case 'b':
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
		case 'c':
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
		case 'd':
			printGlobalSetting();
			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
			break;
		default:
			printf("Please Enter a valid option 1-4!\n");
			printf("Press any key to continue..\n");
			fgetc(stdin);
			while (getchar() != '\n');
		}

		system("CLS");
	}
	return(0);
}
#endif