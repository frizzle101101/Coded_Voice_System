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
#include "phonebook.h"

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
	long audio_buff_sz = sample_sec * record_time;
	unsigned int rcvStatus = 0;
	QUEUE *rcvQ;
	NODE *tmp;
	HEADER *tmpHdr, *rcvHdr;
	void *payload, *rcvPayload;

	rcvQ = queue_init();
	tmp = (NODE *)malloc(sizeof(NODE));

	initializeBuffers(sample_sec, record_time, &audio_buff, &audio_buff_sz, MEMALLOC);
	char input[MAX];
	char *str1;
	char check[MAX];
	int checker;
	OPTIONS options;
	//gets_s(input);
	char *rName;
	SRUser newuser;
	link left = NULL;
	link right = NULL;

	while (1) {
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
			options = RECORD;
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
					strcpy(rName, tempId);
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
		options = HELP;
	}

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
		playback();
		break;
	case RECEIVE:
		receive();
		break;
	case RECORD:
		record();
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
		newuser.mode = 'r';
		strcpy(newuser.name, rName);
		NEW(newuser, left, right);
		InsertR(newuser);
		break;
	case SETTINGS:
		settings();
		break;
	case REMOVE:
		remove();
		break;
	}
	return(0);
}