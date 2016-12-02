#pragma once

#include "huffman.h"

void *huffman_cmp_wrapper(void *content, long *cmprsed_size);
void *huffman_decmp_wrapper(void *content, long ldatalength);