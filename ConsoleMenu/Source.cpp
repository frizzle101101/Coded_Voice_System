/*	Damian Scarpone

September 06, 2016

Program: To find the sum and average of three user picked integers

Revisoins:
*/
/*********************************Libraries*************************************************/
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
/*                                Constants                                                */
#define MAX 100
#define TRUE 0
#define HELPW "help"
#define COMPOSEW "compose"
#define DISPLAYW "display"
#define PLAYBACKW "playback"
#define RECEIVEW "receive"
#define RECORDW "record"
#define SELECTW "select"
#define SETTINGSW "settings"
#define REMOVEW "remove"
/*                               Prototypes                                             */
void helpmenu(void);
void composeMsg(void);
void displayMsg(void);
void playback(void);
void receive(void);
void record(void);
void select(void);
void settings(void);
void remove(void);
typedef enum { HELP, COMPOSE, DISPLAY, PLAYBACK, RECEIVE, RECORD, SELECT, SETTINGS, REMOVE }OPTIONS;
/*                              Main Function                                              */
void main()
{
	char input[MAX];
	char *str1;
	char check[MAX];
	int checker;
	OPTIONS options;
	//gets_s(input);

	while (1) {
		gets_s(input);
		str1 = strtok(input, "-");
		if (strcmp(HELPW, str1) == TRUE)
			options = HELP;
		else if (strcmp(COMPOSEW, str1) == TRUE)
			options = COMPOSE;
		else if (strcmp(DISPLAYW, str1) == TRUE)
			options = DISPLAY;
		else if (strcmp(PLAYBACKW, str1) == TRUE)
			options = PLAYBACK;
		else if (strcmp(RECEIVEW, str1) == TRUE)
			options = RECEIVE;
		else if (strcmp(RECORDW, str1) == TRUE)
			options = RECORD;
		else if (strcmp(SELECTW, str1) == TRUE)
			options = SELECT;
		else if (strcmp(SETTINGSW, str1) == TRUE)
			options = SETTINGS;
		else if (strcmp(REMOVEW, str1) == TRUE)
			options = REMOVE;
		else
		{
			printf("invlaid input. Defaulting to help\n");
			options = HELP;
		}

		switch (options)
		{
		case HELP:
			helpmenu();
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
			select();
			break;
		case SETTINGS:
			settings();
			break;
		case REMOVE:
			remove();
			break;
		}
	}
	system("PAUSE");

}




void helpmenu(void)
{
	printf("HELP MENU\n");
	printf("help -");
	printf(" Output a list of valid commands\n");
	printf("compose -");
	printf(" Compose a message\n");
	printf("display -");
	printf(" Display a message\n");
	printf("playback -");
	printf(" Playback a message\n");
	printf("receive -");
	printf(" Receive a message\n");
	printf("record -");
	printf(" Record a message\n");
	printf("select -");
	printf(" Select a message or device\n");
	printf("settings -");
	printf(" Change parameters\n");
	printf("remove -");
	printf(" Remove a message\n");


}

void composeMsg(void)
{
	printf("compose\n");
}

void displayMsg(void)
{
	printf("display\n");
}

void playback(void)
{
	printf("playback\n");
}

void receive(void)
{
	printf("receive\n");
}

void record(void)
{
	printf("record\n");
}

void select(void)
{
	printf("select\n");
}

void settings(void)
{
	printf("settings\n");
}

void remove(void)
{
	printf("remove\n");
}