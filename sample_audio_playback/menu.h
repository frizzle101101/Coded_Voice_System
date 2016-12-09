#pragma once
/*                                Constants                                                */
#define MAX 100
/*                               Prototypes                                             */
void helpmenu(void);
void helpcompose(void);
void helpdisplay(void);
void helpplayback(void);
void helpreceive(void);
void helprecord(void);
void helpselect(void);
void helpsettings(void);
void helpremove(void);
void composeMsg(void);
void displayMsg(void);
void playback(void);
void receive(void);
void record(void);
void select(void);
void settings(void);
void remove(void);
typedef enum { 
	HELP, 
	HELPCOMPOSE,
	HELPDISPLAY,
	HELPPLAYBACK,
	HELPRECEIVE, 
	HELPRECORD, 
	HELPSELECT, 
	HELPTEST, 
	HELPINSERT, 
	HELPSETTINGS,
	HELPSET,
	HELPREMOVE,
	HELPSEND,
	COMPOSE, 
	DISPLAY, 
	PLAYBACK,
	PLAYBACKALL,
	PRIORITY, 
	RECEIVE, 
	RECORD, 
	SELECTR, 
	SELECTS, 
	SELECTRID, 
	SELECTSID, 
	SENDAUDIO,
	SENDBMP,
	SETTINGS,
	SETCMPRS,
	SETHASH,
	SETAUDIOPARAMS,
	SETPRIORITY,
	SETPRIORITYID,
	SETBAUD,
	SETBAUDID,
	SETCOM,
	SETCOMID,
	TESTAUDIO,
	TESTPRIQU,
	TESTPHONEBOOK,
	TESTHEADER,
	TESTBMP,
	REMOVE
}OPTIONS;