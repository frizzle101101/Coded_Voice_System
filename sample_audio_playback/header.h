#pragma once

typedef struct {
	long lSignature;
	char bReceiverAddr;
	char bVersion;
	long lDataLength;
	char flags[15];
	char bPattern[4];
} HEADER;

HEADER *header_init();
void *payload_pack(HEADER *usrHeader, short *audioBuf);
int payload_unpack(HEADER **usrHeader, short **audioBuf, void *payload);