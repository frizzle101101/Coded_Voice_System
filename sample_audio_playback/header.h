#pragma once

typedef struct {
	char senderID;
	char rcverID;
	char rcverID_rp1;
	long lSignature;
	char bVersion;
	char rcverID_rp2;
	long lDataLength;
	long clDataLength;
	char flags;
	char rcverID_rp3;
	char bPattern[4];
	int contentHash;
} HEADER;

#define DEADBEEF 0xDEADBEEF
#define PATTERN  "\x55\xaa\x55\xaa"
#define AUDIO_F     (1 << 0)
#define TEXT_F      (1 << 1)
#define HUFFMAN_F   (1 << 2)
#define RLE_F       (1 << 3)
#define DEFAULT_STATION_ID       0x01
#define DEFAULT_TARGET_ID       0x01
#define HEADERSIZE  32

static char stationID = DEFAULT_STATION_ID;
static char targetID = DEFAULT_TARGET_ID;

HEADER *header_init();
void *payload_pack(HEADER *usrHeader, void *contentBuf);
int payload_unpack(HEADER **usrHeader, short **audioBuf, void *payload);