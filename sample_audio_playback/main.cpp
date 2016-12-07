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
#include "menu.h"

#define DEFAULT_RECORD_TIME		2		// seconds to record for
#define DEFAULT_SAMPLES_SEC		8000	// samples per second
#define WAITTIME 40

int	main(int argc, char *argv[])
{
	char option;
	short *audio_buff = NULL;
	short *audio_rcv = NULL;
	int sample_sec = 0;
	int record_time = 0;
	
	unsigned int rcvStatus = 0;
	QUEUE *rcvQ;
	NODE *tmp;
	HEADER *tmpHdr, *rcvHdr;
	void *payload, *rcvPayload;

	rcvQ = queue_init();
	tmp = (NODE *)malloc(sizeof(NODE));

	
	char input[MAX];
	char *str1;

	OPTIONS options;
	//gets_s(input);
	while (1) {
		printf(">");
		gets_s(input);
		for (int i = 0; input[i]; i++) {
			input[i] = tolower(input[i]);
		}
		str1 = strtok(input, " -");
		if (strcmp("help", str1) == 0)
		{
			str1 = strtok(NULL, " -");
			if (str1 == NULL)
				options = HELP;
			else if (strcmp("compose", str1) == 0)
				options = HELPCOMPOSE;
			else if (strcmp("display", str1) == 0)
				options = HELPDISPLAY;
			else if (strcmp("playback", str1) == 0)
				options = HELPPLAYBACK;
			else if (strcmp("receive", str1) == 0)
				options = HELPRECEIVE;
			else if (strcmp("record", str1) == 0)
				options = HELPRECORD;
			else if (strcmp("select", str1) == 0)
				options = HELPSELECT;
			else if (strcmp("select", str1) == 0)
				options = HELPINSERT;
			else if (strcmp("settings", str1) == 0)
				options = HELPSETTINGS;
			else if (strcmp("remove", str1) == 0)
				options = HELPREMOVE;
		}
		else if (strcmp("compose", str1) == 0)
			options = COMPOSE;
		else if (strcmp("display", str1) == 0)
			options = DISPLAY;
		else if (strcmp("playback", str1) == 0)
			options = PLAYBACK;
		else if (strcmp("receive", str1) == 0)
			options = RECEIVE;
		else if (strcmp("record", str1) == 0)
		{
			options = RECORD;
			char *tempRecTime = strtok(NULL, " -");
			char *tempRecSampl;
			if (tempRecTime == NULL)
			{
				record_time = DEFAULT_RECORD_TIME;
				tempRecSampl = strtok(NULL, " -");
			}
			else
				record_time = atoi(tempRecTime);

			if (tempRecSampl == NULL)
				sample_sec = DEFAULT_SAMPLES_SEC;
			else
				sample_sec = atoi(tempRecTime);
		}
		else if (strcmp("select", str1) == 0)
		{
			char *tempId = strtok(NULL, " -");
			if (tempId == NULL)
				options = SELECT;
			str1 = strtok(NULL, " -");
			if (str1 == NULL)
			{
				char *rName = tempId;
				options = SELECTNAME;
			}
			else if (strcmp("i", str1) == 0)
			{
				int rId = atoi(tempId);
				options = SELECTID;
			}
		}
		else if (strcmp("insert", str1) == 0)
		{
			char *tempId = strtok(NULL, " -");
			if (tempId == NULL)
				options = HELPINSERT;
			else
			{
				str1 = strtok(NULL, " -");
				if (str1 == NULL)
				{
					options = HELPINSERT;
				}
				else if (strcmp("r", str1) == 0)
				{
					options = INSERTRNAME;
				}
			}
		}
		else if (strcmp("settings", str1) == 0)
			options = SETTINGS;
		else if (strcmp("remove", str1) == 0)
			options = REMOVE;
		else
			printf("invlaid input. Defaulting to help\n");

		long audio_buff_sz = sample_sec * record_time;

		switch (options)
		{
		case HELP:
			helpmenu();
			break;
		case HELPCOMPOSE:
			helpcompose();
			break;
		case HELPDISPLAY:
			helpdisplay();
			break;
		case HELPPLAYBACK:
			helpplayback();
			break;
		case HELPRECEIVE:
			helpreceive();
			break;
		case HELPRECORD:
			helprecord();
			break;
		case HELPSELECT:
			helpselect();
			break;
		case HELPINSERT:
			helpsettings();
			break;
		case HELPSETTINGS:
			helpsettings();
			break;
		case HELPREMOVE:
			helpremove();
			break;
		case COMPOSE:
			composeMsg();
			break;
		case DISPLAY:
			displayMsg();
			break;
		case PLAYBACK:
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
			break;
		case RECEIVE:
			receive();
			break;
		case RECORD:
			initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMALLOC);
			RecordBuffer(audio_buff, audio_buff_sz, sample_sec);
			CloseRecording();
			break;
		case SELECT:
			//getphonebook();
			break;
		case SELECTNAME:
			select();
			break;
		case SELECTID:
			select();
			break;
		case INSERTRNAME:
			break;
		case SETTINGS:
			settings();
			break;
		case REMOVE:
			remove();
			break;
		}
	}
	return(0);
}