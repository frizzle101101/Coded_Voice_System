#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "header.h"

#define DEADBEEF 0xDEADBEEF
#define PATTERN  "\x55\xaa\x55\xaa"

static void header_check(HEADER *usrHeader)
{
	printf("Header Checks\n");
	printf("Header Size:%d bytes\n", _msize(usrHeader));
	printf("lSignature: %04x\n", usrHeader->lSignature);
	printf("bVersion: %d\n", usrHeader->bVersion);
	printf("bPattern: %04x\n", usrHeader->bPattern);

}

HEADER *header_init()
{
	HEADER *tmp;
	long deadbeef = 0xDEADBEEF;
	tmp = (HEADER *)malloc(sizeof(HEADER));
	//memcpy(tmp->bPattern, PATTERN, strlen(PATTERN));
	tmp->bVersion = 1;
	tmp->lSignature = DEADBEEF;
	tmp->lDataLength = 32000;
	return tmp;
}

void *payload_pack(HEADER *usrHeader, short *audioBuf)
{
	//HEADER *tmpHdr;
	void *tmp;

	if ((!usrHeader) || (_msize(usrHeader) != 32)) {
		printf("Invalid Header!\n");
		errno = EINVAL;
		return NULL;
	}

	if (!audioBuf) {
		printf("Invalid audio input\n");
		errno = EINVAL;
		return NULL;
	}

	tmp = malloc(sizeof(HEADER) + _msize(audioBuf));
	memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
	memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp),  audioBuf, _msize(audioBuf));

	return tmp;
}
int payload_unpack(HEADER **usrHeader, short **audioBuf, void *payload)
{
	HEADER *tmpHdr;
	short *rcvAudio;

	tmpHdr = (HEADER *)malloc(sizeof(HEADER));

	memmove_s(tmpHdr, sizeof(HEADER), payload, sizeof(HEADER));
	
	header_check(tmpHdr);

	*usrHeader = tmpHdr;

	rcvAudio = (short *)malloc(sizeof(short) *tmpHdr->lDataLength);

	memmove_s(rcvAudio, _msize(rcvAudio), (char *)payload + sizeof(HEADER), tmpHdr->lDataLength);
	
	*audioBuf = rcvAudio;
	return 0;
}