#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "playback.h"
#include "compression.h"

void *huffman_cmp_wrapper(void *content, long *cmprsed_size)
{
	//printf("Pre-compressed test 2\n");
	//PlayBuffer((short *)content, (long)16000, 8000);
	short *compressed;
	unsigned int *c_sized;

	compressed = (short *)malloc(_msize(content) + 384);
	c_sized = (unsigned int *)malloc(sizeof(unsigned char));

	*c_sized = Huffman_Compress((unsigned char*)content, (unsigned char *)compressed, _msize(content));

	if (cmprsed_size)
		*cmprsed_size = *c_sized;

	printf("cmprsed_size:%d, msize compressed: %d\n", *cmprsed_size, _msize(compressed));
	return compressed;
}

void *huffman_decmp_wrapper(void *content, long ldatalength)
{
	short *decompressed;
	decompressed = (short *)malloc(sizeof(short) *ldatalength);

	printf("huffman_decmp_wrapper(). size content %d\n", _msize(content));
	printf("huffman_decmp_wrapper(). size decompressed %d\n", _msize(decompressed));
	Huffman_Uncompress((unsigned char *)content, (unsigned char *)decompressed, _msize(content), _msize(decompressed));

	//PlayBuffer((short *)decompressed, (long)16000, 8000);
	return decompressed;
}