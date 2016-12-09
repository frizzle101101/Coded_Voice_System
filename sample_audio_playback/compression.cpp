#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "playback.h"
#include "compression.h"

unsigned char *huffman_cmp_wrapper(void *content, long *cmprsed_size)
{
	unsigned char *compressed;
	long *c_sized;

	compressed = (unsigned char *)malloc(_msize(content) + 384);
	c_sized = (long *)malloc(sizeof(long));

	*c_sized = Huffman_Compress((unsigned char*)content, compressed, _msize(content));

	if (cmprsed_size)
		*cmprsed_size = *c_sized;

	compressed = (unsigned char *)realloc(compressed, *c_sized);
	return compressed;
}

unsigned char *huffman_decmp_wrapper(void *content, long ldatalength)
{
	unsigned char *decompressed;
	decompressed = (unsigned char *)malloc(sizeof(unsigned char) *ldatalength);
	Huffman_Uncompress((unsigned char *)content, decompressed, _msize(content), _msize(decompressed));
	return decompressed;
}