#pragma once

#include "huffman.h"

unsigned char *huffman_cmp_wrapper(void *content, long *cmprsed_size);
unsigned char *huffman_decmp_wrapper(void *content, long ldatalength);