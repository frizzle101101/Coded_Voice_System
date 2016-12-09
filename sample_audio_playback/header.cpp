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

void *payload_pack(HEADER *usrHeader, void *contentBuf)
{
	//HEADER *tmpHdr;
	void *tmp;
	void *compressedContent;
	short *pkgedCompressedContent;
	void *decompressedContent;
	printf("header size %d", _msize(usrHeader));
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

	if (usrHeader->flags & AUDIO_F) {
		if (usrHeader->flags & HUFFMAN_F) {
			compressedContent = huffman_cmp_wrapper(contentBuf, &usrHeader->clDataLength);
			pkgedCompressedContent = (short *)malloc(sizeof(short) *usrHeader->clDataLength);
			memmove_s(pkgedCompressedContent, _msize(pkgedCompressedContent), compressedContent, usrHeader->clDataLength);
			printf("msize pkgedCompressedContent:%d cldatalength:%d\n", _msize(pkgedCompressedContent), usrHeader->clDataLength);
			printf("Post-compressed test\n");
			PlayBuffer((short *)huffman_decmp_wrapper(pkgedCompressedContent, usrHeader->lDataLength), usrHeader->lDataLength, usrHeader->sampleSec);


			//printf("%d\n", usrHeader->clDataLength);
			tmp = malloc(sizeof(HEADER) + _msize(pkgedCompressedContent));
			memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
			memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp), pkgedCompressedContent, _msize(pkgedCompressedContent));
			printf("Post-packpaged Pre-send test\n");
			printf("msize tmp:%d\n", _msize(tmp));
			//payload_unpack(NULL, NULL, tmp);

		}
		else {
			tmp = malloc(sizeof(HEADER) + _msize(contentBuf));
			memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
			memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp), contentBuf, _msize(contentBuf));
		}
	} else if (usrHeader->flags & BMP_F) {
		printf("\n%d\n", usrHeader->lDataLength);
		tmp = malloc(sizeof(HEADER) + _msize(contentBuf));
		memcpy_s(tmp, _msize(tmp), usrHeader, _msize(usrHeader));
		memcpy_s((char *)tmp + sizeof(HEADER), _msize(tmp), contentBuf, _msize(contentBuf));
	}

	return tmp;
}
int payload_unpack(HEADER **usrHeader, void **audioBuf, void *payload)
{
	int rc = 0;
	HEADER *tmpHdr;
	short *rcvAudio;
	short *rcvAudio_u;
	short *compressed_content;
	char *rcvBMP;

	printf("%d size of payload", _msize(payload));
	tmpHdr = (HEADER *)malloc(sizeof(HEADER));

	memmove_s(tmpHdr, sizeof(HEADER), payload, sizeof(HEADER));
	
	rc = header_check(tmpHdr);

	if (rc)
		return rc;

	if(usrHeader)
		*usrHeader = tmpHdr;

	if (tmpHdr->flags & AUDIO_F) {
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

		}
		else {
			rcvAudio_u = (short *)malloc(sizeof(short) *tmpHdr->lDataLength);
			memmove_s(rcvAudio_u, _msize(rcvAudio_u), (char *)payload + sizeof(HEADER), tmpHdr->lDataLength);
			*audioBuf = rcvAudio_u;

			rc = hash_check(tmpHdr, rcvAudio_u);

			if (rc)
				return rc;
		}
	} else if (tmpHdr->flags & BMP_F) {
		FILE *fp;

		rcvBMP = (char *)malloc(sizeof(char) * tmpHdr->lDataLength);
		memmove_s(rcvBMP, tmpHdr->lDataLength, (char *)payload + sizeof(HEADER), tmpHdr->lDataLength);
		
		fp = fopen("receiveBMP.bmp", "wb");
		fwrite(rcvBMP, sizeof(char), tmpHdr->lDataLength, fp);
		fclose(fp);

		DrawBMP("receiveBMP.bmp", 0, 400);
	}
	
	return 0;
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
