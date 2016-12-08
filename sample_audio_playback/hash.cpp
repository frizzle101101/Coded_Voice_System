#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

/******************************************************************************
This function is a well-known universal string hash function. This function
accepts a string and mod number for calculating string hash. This function
returns an integer hash value ranging from 0 to the input mod value.

    @param    hashStr        Input string which to be hashed by the function.
	          mod            Modular number that indicate the range of the hash
			                 value.

   @return   hash            A hash value calculated from the input parameters.

******************************************************************************/
int uni_str_hash(char *hashStr, int mod)
{
	int hash, hash_multi;
	hash_multi = RANDOM_NUMBER;

	for (hash = 0; *hashStr != '\0'; hashStr++, hash_multi = (hash_multi * RANDOM_MULTIPLIER) % (mod - 1))
		hash = (hash * hash_multi + *hashStr) % mod;

	return hash;
}