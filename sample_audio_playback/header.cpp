#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "header.h"
#include "compression.h"
#include "playback.h"
#include "hash.h"
#include "console.h"
#include "huffman.h"


static int repetition_vote(HEADER *usrHeader)
{
	int voteCount = 0;

	printf("0x%02x 0x%02x\n", stationID, usrHeader->rcverID);
	if (usrHeader->rcverID == stationID) 
		voteCount++;

	if (usrHeader->rcverID_rp1 == stationID)
		voteCount++;

	if (usrHeader->rcverID_rp2 == stationID)
		voteCount++;

	if (usrHeader->rcverID_rp3 == stationID)
		voteCount++;
	
	if (voteCount >= 3)
		return 0;
	else
		return -EINVAL;
}

static int header_check(HEADER *usrHeader)
{
	int rc = 0;

	printf("Header Checks\n");
	rc = repetition_vote(usrHeader);

	if (rc) {
		return rc;
	}

	return rc;
}

static int hash_check(HEADER *usrHeader, void *content)
{
	int rc;
	(usrHeader->contentHash == uni_str_hash((char *)content, DEFAULT_MOD)) ? rc = 0 : rc = -1;

	return rc;
}
static int populateIDs(HEADER *usrHeader)
{
	int rc = 0;

	if (!usrHeader) {
		rc = -EINVAL;
	} else {
		usrHeader->senderID = stationID;
		usrHeader->rcverID = targetID;
		usrHeader->rcverID_rp1 = targetID;
		usrHeader->rcverID_rp2 = targetID;
		usrHeader->rcverID_rp3 = targetID;
	}
	return rc;
}

HEADER *header_init(FILETYPE usrFileType, long sizeBuf, 
	                short sampleSec, short recordSec, BOOLYN isCompressed)
{
	int rc;
	HEADER *tmp;
	long deadbeef = 0xDEADBEEF;
	printf("sizeof header%d\n", sizeof(HEADER));
	tmp = (HEADER *)malloc(sizeof(HEADER));
	memcpy(tmp->bPattern, PATTERN, strlen(PATTERN));
	tmp->bVersion = 1;
	tmp->lSignature = DEADBEEF;
	tmp->lDataLength = sizeBuf;
	tmp->clDataLength = 0;
	populateIDs(tmp);
	tmp->priority = transmitPriority;
	tmp->flags = 0;

	switch (usrFileType) {
		case AUDIO_T:
			tmp->flags |= AUDIO_F;
			tmp->sampleSec = sampleSec;
			tmp->recordTime = recordSec;
			break;

		case BMP_T:
			tmp->flags |= BMP_F;
			break;

		case TEXT_T:
			tmp->flags |= TEXT_F;
			break;
			
	}

	if (isCompressed == TRUE_B)
		tmp->flags |= HUFFMAN_F;

	return tmp;
}

int setGlobalStationID(char *inputID)
{
	int rc = 0;

	if (inputID)
		stationID = *inputID;
	else
		rc = -EINVAL;

	return rc;
}

int setGlobalTargetID(char *inputID)
{
	int rc = 0;

	if (inputID)
		targetID = *inputID;
	else
		rc = -EINVAL;

	return rc;
}

int setGlobalPriority(char *inputPrio)
{
	int rc = 0;

	if (inputPrio)
		 transmitPriority = *inputPrio;
	else
		rc = -EINVAL;

	return rc;
}

int setGlobalCompression(BOOLYN *option)
{
	int rc = 0;

	if (option)
		isCompression = *option;
	else
		rc = -EINVAL;

	return rc;
}

int setGlobalHash(BOOLYN *option)
{
	int rc = 0;

	if (option)
		isHash = *option;
	else
		rc = -EINVAL;

	return rc;
}

void printGlobalSetting(void)
{
	printf("StationID: %02x\n", stationID);
	printf("targetID: %02x\n", targetID);
	printf("Current Priority: %d\n", transmitPriority);
	if (isCompression == TRUE_B)
		printf("Compression: ON\n");
	else
		printf("Compression: OFF\n");
}

void *payload_pack(HEADER *usrHeader, void* contentBuf)
{
	void *tmp;
	unsigned char *compressedContent;

	printf("header size %d\n", _msize(usrHeader));
	if ((!usrHeader) || (_msize(usrHeader) != HEADERSIZE)) {
		printf("Invalid Header!\n");
		errno = EINVAL;
		return NULL;
	}

	if (!contentBuf) {
		printf("Invalid input buffer\n");
		errno = EINVAL;
		return NULL;
	}

	usrHeader->contentHash = uni_str_hash((char *)contentBuf, DEFAULT_MOD);

	if (usrHeader->flags & HUFFMAN_F) {
		compressedContent = huffman_cmp_wrapper(contentBuf, &usrHeader->clDataLength);
		tmp = malloc(sizeof(HEADER) + _msize(compressedContent));
		memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
		memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp), compressedContent, _msize(compressedContent));
	}
	else {
		tmp = malloc(sizeof(HEADER) + _msize(contentBuf));
		memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
		memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp), contentBuf, _msize(contentBuf));
	}

	return tmp;
}

int payload_unpack(HEADER **usrHeader, void **outBuf, void *payload)
{
	int rc = 0;
	HEADER *tmpHdr;
	unsigned char *rcvContent;
	unsigned char *compressed_content;

	printf("%d size of payload", _msize(payload));
	tmpHdr = (HEADER *)malloc(sizeof(HEADER));

	memmove_s(tmpHdr, sizeof(HEADER), payload, sizeof(HEADER));
	
	rc = header_check(tmpHdr);

	if (rc)
		return rc;

	if(usrHeader)
		*usrHeader = tmpHdr;

	rcvContent = (unsigned char *)malloc(sizeof(unsigned char) *tmpHdr->lDataLength);

	if (tmpHdr->flags & HUFFMAN_F) {
		compressed_content = (unsigned char*)malloc(sizeof(unsigned char) * tmpHdr->clDataLength);
		memmove_s(compressed_content, _msize(compressed_content), (char *)payload + sizeof(HEADER), tmpHdr->clDataLength);
		rcvContent = huffman_decmp_wrapper(compressed_content, tmpHdr->lDataLength);

		if (rc)
			return rc;

	} else {
		memmove_s(rcvContent, _msize(rcvContent), (char *)payload + sizeof(HEADER), tmpHdr->lDataLength);
	}

	*outBuf = rcvContent;

	if (isHash == TRUE_B)
		rc = hash_check(tmpHdr, rcvContent);
	else
		printf("No Hash Check\n");

	if (rc)
		return rc;

	return rc;
}

void print_header(HEADER *usrHeader)
{
	if (usrHeader) {
		printf("Header Size:%d bytes\n", _msize(usrHeader));
		printf("lSignature: %04x\n", usrHeader->lSignature);
		printf("cldatalength: %d\n", usrHeader->clDataLength);
		printf("Receiver ID %02x\n", usrHeader->rcverID);
		printf("Sender ID %02x\n", usrHeader->senderID);
		printf("Priority %02x\n", usrHeader->priority);
		printf("Receiver ID Repeat 1 %02x\n", usrHeader->rcverID_rp1);
		printf("ldatalength: %d\n", usrHeader->lDataLength);
		printf("Receiver ID Repeat 2 %02x\n", usrHeader->rcverID_rp2);
		printf("bVersion: %d\n", usrHeader->bVersion);
		printf("Sample Second: %hu\n", usrHeader->sampleSec);
		printf("Hash %d\n", usrHeader->contentHash);
		printf("Receiver ID Repeat 3 %02x\n", usrHeader->rcverID_rp3);
		printf("Flags: %02x\n", usrHeader->flags);
		printf("Record Time: %hu sec\n", usrHeader->recordTime);
		printf("bPattern: %04x\n", usrHeader->bPattern);
	}
}
