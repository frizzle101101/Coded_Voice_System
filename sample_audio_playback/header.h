#pragma once

typedef struct {
	long lSignature;
	char bReceiverAddr;
	char bVersion;
	long lDataLength;
	long clDataLength;
	char flags;
	char TBD[10];
	char bPattern[4];
} HEADER;

HEADER *header_init();
void *payload_pack(HEADER *usrHeader, void *contentBuf);
int payload_unpack(HEADER **usrHeader, short **audioBuf, void *payload);