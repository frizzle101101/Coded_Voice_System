#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "header.h"
#include "compression.h"
#include "playback.h"
#include "hash.h"

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

	printf("Station Sender ID %04x\n", usrHeader->senderID);
	printf("Header Size:%d bytes\n", _msize(usrHeader));
	printf("lSignature: %04x\n", usrHeader->lSignature);
	printf("bVersion: %d\n", usrHeader->bVersion);
	printf("bPattern: %04x\n", usrHeader->bPattern);
	printf("ldatalength: %d\n", usrHeader->lDataLength);
	printf("cldatalength: %d\n", usrHeader->clDataLength);

	return rc;
}

static int hash_check(HEADER *usrHeader, void *content)
{
	int rc;
	(usrHeader->contentHash == uni_str_hash((char *)content, DEFAULT_MOD)) ? rc = 0 : rc = -1;

	printf("hash rc %d", rc);

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
		printf("0x%02x 0x%02x\n", stationID, usrHeader->rcverID);
	}
	return rc;
}

int setStationID(char *inputID)
{
	int rc = 0;

	if (inputID)
		stationID = *inputID;
	else
		rc = -EINVAL;

	return rc;
}

int setTargetID(char *inputID)
{
	int rc = 0;

	if (inputID)
		targetID = *inputID;
	else
		rc = -EINVAL;

	return rc;
}

HEADER *header_init()
{
	int rc;
	HEADER *tmp;
	long deadbeef = 0xDEADBEEF;
	printf("sizeof header%d\n", sizeof(HEADER));
	tmp = (HEADER *)malloc(sizeof(HEADER));
	memcpy(tmp->bPattern, PATTERN, strlen(PATTERN));
	tmp->bVersion = 1;
	tmp->lSignature = DEADBEEF;
	tmp->lDataLength = 32000;
	tmp->clDataLength = 0;
	tmp->flags = 0;
	tmp->flags |= AUDIO_F;
	tmp->flags |= HUFFMAN_F;
	populateIDs(tmp);
	return tmp;
}

void *payload_pack(HEADER *usrHeader, void *contentBuf)
{
	//HEADER *tmpHdr;
	void *tmp;
	void *compressedContent;
	short *pkgedCompressedContent;
	void *decompressedContent;
	//printf("header size %d", _msize(usrHeader));
	if ((!usrHeader) || (_msize(usrHeader) != HEADERSIZE)) {
		printf("Invalid Header!\n");
		errno = EINVAL;
		return NULL;
	}

	if (!contentBuf) {
		printf("Invalid audio input\n");
		errno = EINVAL;
		return NULL;
	}

	usrHeader->contentHash = uni_str_hash((char *)contentBuf, DEFAULT_MOD);

	//printf("Pre-compressed test\n");
	//PlayBuffer((short *)contentBuf, (long)16000, 8000);

	
	if (usrHeader->flags & HUFFMAN_F) {
		compressedContent = huffman_cmp_wrapper(contentBuf, &usrHeader->clDataLength);
		pkgedCompressedContent = (short *)malloc(sizeof(short) *usrHeader->clDataLength);
		memmove_s(pkgedCompressedContent, _msize(pkgedCompressedContent), compressedContent, usrHeader->clDataLength);
		printf("msize pkgedCompressedContent:%d cldatalength:%d\n", _msize(pkgedCompressedContent), usrHeader->clDataLength);
		printf("Post-compressed test\n");
		PlayBuffer((short *)huffman_decmp_wrapper(pkgedCompressedContent, usrHeader->lDataLength), 16000, 8000);

		
		//printf("%d\n", usrHeader->clDataLength);
		tmp = malloc(sizeof(HEADER) + _msize(pkgedCompressedContent));
		memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
		memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp), pkgedCompressedContent, _msize(pkgedCompressedContent));
		printf("Post-packpaged Pre-send test\n");
		printf("msize tmp:%d\n", _msize(tmp));
		//payload_unpack(NULL, NULL, tmp);

	} else {
		tmp = malloc(sizeof(HEADER) + _msize(contentBuf));
		memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
		memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp), contentBuf, _msize(contentBuf));
	}
	return tmp;
}
int payload_unpack(HEADER **usrHeader, short **audioBuf, void *payload)
{
	int rc = 0;
	HEADER *tmpHdr;
	short *rcvAudio;
	short *rcvAudio_u;
	short *compressed_content;

	printf("%d size of payload", _msize(payload));
	tmpHdr = (HEADER *)malloc(sizeof(HEADER));

	memmove_s(tmpHdr, sizeof(HEADER), payload, sizeof(HEADER));
	
	rc = header_check(tmpHdr);

	if (rc)
		return rc;

	if(usrHeader)
		*usrHeader = tmpHdr;



	if (tmpHdr->flags & HUFFMAN_F) {
		printf("unpack(): cldatalength: %d\n", tmpHdr->clDataLength);
		compressed_content = (short *)malloc(sizeof(short) * tmpHdr->clDataLength);
		rcvAudio = (short *)malloc(sizeof(short) *tmpHdr->lDataLength);
		memmove_s(compressed_content, _msize(compressed_content), (char *)payload + sizeof(HEADER), tmpHdr->clDataLength);
		printf("Decompressed test\n");
		//PlayBuffer((short *)huffman_decmp_wrapper(compressed_content, tmpHdr->lDataLength), (long)16000, 8000);
		rcvAudio = (short *)huffman_decmp_wrapper(compressed_content, tmpHdr->lDataLength);
		//PlayBuffer(rcvAudio, (long)16000, 8000);
		*audioBuf = rcvAudio;

		rc = hash_check(tmpHdr, rcvAudio);

		if (rc)
			return rc;

	} else {
		rcvAudio_u = (short *)malloc(sizeof(short) *tmpHdr->lDataLength);
		memmove_s(rcvAudio_u, _msize(rcvAudio_u), (char *)payload + sizeof(HEADER), tmpHdr->lDataLength);
		*audioBuf = rcvAudio_u;

		rc = hash_check(tmpHdr, rcvAudio_u);

		if (rc)
			return rc;
	}
	
	return 0;
}