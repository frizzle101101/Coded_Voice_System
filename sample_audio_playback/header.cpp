#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "header.h"
#include "compression.h"
#include "playback.h"

#define DEADBEEF 0xDEADBEEF
#define PATTERN  "\x55\xaa\x55\xaa"
#define AUDIO_F     (1 << 0)
#define TEXT_F      (1 << 1)
#define HUFFMAN_F   (1 << 2)
#define RLE_F       (1 << 3)

static void header_check(HEADER *usrHeader)
{
	printf("Header Checks\n");
	printf("Header Size:%d bytes\n", _msize(usrHeader));
	printf("lSignature: %04x\n", usrHeader->lSignature);
	printf("bVersion: %d\n", usrHeader->bVersion);
	printf("bPattern: %04x\n", usrHeader->bPattern);
	printf("ldatalength: %d\n", usrHeader->lDataLength);
	printf("cldatalength: %d\n", usrHeader->clDataLength);
}

HEADER *header_init()
{
	HEADER *tmp;
	long deadbeef = 0xDEADBEEF;
	tmp = (HEADER *)malloc(sizeof(HEADER));
	memcpy(tmp->bPattern, PATTERN, strlen(PATTERN));
	tmp->bVersion = 1;
	tmp->lSignature = DEADBEEF;
	tmp->lDataLength = 32000;
	tmp->clDataLength = 0;
	tmp->flags = 0;
	tmp->flags |= AUDIO_F;
	tmp->flags |= HUFFMAN_F;
	return tmp;
}

void *payload_pack(HEADER *usrHeader, void *contentBuf)
{
	//HEADER *tmpHdr;
	void *tmp;
	void *compressedContent;
	short *pkgedCompressedContent;
	void *decompressedContent;
	if ((!usrHeader) || (_msize(usrHeader) != 32)) {
		printf("Invalid Header!\n");
		errno = EINVAL;
		return NULL;
	}

	if (!contentBuf) {
		printf("Invalid audio input\n");
		errno = EINVAL;
		return NULL;
	}

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
	HEADER *tmpHdr;
	short *rcvAudio;
	short *rcvAudio_u;
	short *compressed_content;

	printf("%d size of payload", _msize(payload));
	tmpHdr = (HEADER *)malloc(sizeof(HEADER));

	memmove_s(tmpHdr, sizeof(HEADER), payload, sizeof(HEADER));
	
	header_check(tmpHdr);

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
	} else {
		rcvAudio_u = (short *)malloc(sizeof(short) *tmpHdr->lDataLength);
		memmove_s(rcvAudio_u, _msize(rcvAudio_u), (char *)payload + sizeof(HEADER), tmpHdr->lDataLength);
		*audioBuf = rcvAudio_u;
	}

	
	return 0;
}