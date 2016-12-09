#pragma once

typedef struct {
	long lSignature;
	long clDataLength;
	char rcverID;
	char senderID;
	char priority;
	char rcverID_rp1;
	long lDataLength;
	char rcverID_rp2;
	char bVersion;
	short sampleSec;
	int contentHash;
	char rcverID_rp3;
	char flags;
	short recordTime;
	char bPattern[4];
} HEADER;

typedef enum {
	AUDIO_T,
	TEXT_T,
	BMP_T
}FILETYPE;

typedef enum {
	FALSE_B,
	TRUE_B
} BOOLYN;

#define DEADBEEF 0xDEADBEEF
#define PATTERN  "\x55\xaa\x55\xaa"
#define AUDIO_F       (1 << 0)
#define TEXT_F        (1 << 1)
#define HUFFMAN_F     (1 << 2)
#define BMP_F         (1 << 3)
#define BRAODCAST_F   (1 << 4)

#define DEFAULT_STATION_ID       0x01
#define DEFAULT_TARGET_ID       0x01
#define DEFAULT_TRANSMIT_PRIO   0x00
#define HEADERSIZE  32
#define DEFAULT_COMPRESSION_CHOICE  TRUE_B;
#define DEFAULT_HASH_CHOICE TRUE_B;

static char stationID = DEFAULT_STATION_ID;
static char targetID = DEFAULT_TARGET_ID;
static char transmitPriority = DEFAULT_TRANSMIT_PRIO;
static BOOLYN isCompression = DEFAULT_COMPRESSION_CHOICE;
static BOOLYN isHash = DEFAULT_HASH_CHOICE;

HEADER *header_init(FILETYPE usrFileType, long sizeBuf,
	                short sampleSec, short recordSec, BOOLYN isCompressed);
void *payload_pack(HEADER *usrHeader, void* contentBuf);
int payload_unpack(HEADER **usrHeader, void **audioBuf, void *payload);
void print_header(HEADER *usrHeader);
int setGlobalStationID(char *inputID);
int setGlobalTargetID(char *inputID);
int setGlobalPriority(char *inputPrio);
int setGlobalCompression(BOOLYN *option);
void printGlobalSetting(void);
int setGlobalHash(BOOLYN *option);