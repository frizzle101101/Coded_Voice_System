#pragma once

typedef struct {
	long lSignature;
	char rcverID;
	char senderID;
	char priority;
	char rcverID_rp1;
	long lDataLength;
	long clDataLength;
	char rcverID_rp2;
	char bVersion;
	short sampleSec;
	int contentHash;
	char rcverID_rp3;
	char flags;
	short recordTime;
	char bPattern[4];
} HEADER;

#define DEADBEEF 0xDEADBEEF
#define PATTERN  "\x55\xaa\x55\xaa"
#define AUDIO_F     (1 << 0)
#define TEXT_F      (1 << 1)
#define HUFFMAN_F   (1 << 2)
#define RLE_F       (1 << 3)
#define BMP         (1 << 4)
#define BRAODCAST   (1 << 5)

#define DEFAULT_STATION_ID       0x01
#define DEFAULT_TARGET_ID       0x01
#define DEFAULT_TRANSMIT_PRIO   0x00
#define HEADERSIZE  32

static char stationID = DEFAULT_STATION_ID;
static char targetID = DEFAULT_TARGET_ID;
static char transmitPriority = DEFAULT_TRANSMIT_PRIO;

HEADER *header_init(char *prio, char *inputStationID);
void *payload_pack(HEADER *usrHeader, void *contentBuf);
int payload_unpack(HEADER **usrHeader, short **audioBuf, void *payload);
void print_header(HEADER *usrHeader);