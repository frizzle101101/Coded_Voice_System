#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "rle.h"

#define MAX_CHAR 256
#define THREE_BYTE_ENCODE 3
#define MAX_BUFF_SIZE 1000000


/******************************************************************************
This function return the least occur character in the incoming buffer and
the character will be choosen as the special control character for RLE.


    @param    inBuf         A pointer to a unsigned char buffer.
	          iBufLen       An integer indicates the size of the buffer.  

    @return   least_char    A character of the least occur character in the
	                        input buffer.

******************************************************************************/
static unsigned char retSym(unsigned char *inBuf, int iBufLen)
{
	int i;
	char least_char;
	unsigned long long least_count = _UI64_MAX ;
	unsigned int list[MAX_CHAR] = { 0 };

	for (i = 0; i < iBufLen; i++)
		list[inBuf[i]] += 1;

	for (i = 0; i < MAX_CHAR; i++) {
		if ((list[i] > 0) && (list[i] <= least_count)) {
				least_count = list[i];
				least_char = i;
		}
	}

	return least_char;
}

/******************************************************************************
This function fill in the RLE repeats character or RLE control character
RLE format group into the compression buffer. 

    @param   byte             A character or either RLE repeat chacater or
	                          speical control character.
             outBuf           A pointer to the compressed buffer.
             outPos           An integer indicates the current spot which the
                              compression buffer can be filled at.
             byteCount        A total occurence of character in "byte".
             isSym            A enum type variable indicates if character in
                              "byte" is a control character or RLE repeat
                              character.
             max_buff_size    An integer indicates the maximum size of the
                              compression buffer.

   @return   rc               0 - No error.
             -EOVERFLOW       Comrpession has excceed the maximum buffer size.

******************************************************************************/
static int fillRunLength(unsigned char byte, unsigned char *outBuf, unsigned int *outPos, 
	                      unsigned int byteCount, BOOLYNS isSym, unsigned int max_buff_size)
{
	unsigned int i, j, sub;
	int rc = 0;

	if (isSym == B_FALSE) {
		j = byteCount;
		while (j > 0) {
			if (*outPos + 3 > max_buff_size)
				return -EOVERFLOW;
			else {
				!(j % MAX_CHAR) ? sub = MAX_CHAR : sub = (j % MAX_CHAR);
				j -= sub;
				outBuf[*outPos] = outBuf[0];
				outBuf[*outPos + 1] = byte;
				outBuf[*outPos + 2] = sub;
				*outPos += 3;
			}
		}
	} else {
		j = byteCount;

		while (j > 0) {
			if (*outPos + 3 > max_buff_size)
				return -EOVERFLOW;
			else {
				!(j % MAX_CHAR) ? sub = MAX_CHAR : sub = (j % MAX_CHAR);
				j -= sub;
				outBuf[*outPos] = outBuf[0];
				outBuf[*outPos + 1] = sub;
				outBuf[*outPos + 2] = 0x00;
				*outPos += 3;
			}
		}
	}

	return rc;
}

/******************************************************************************
This function accepts an input buffer and compressed the content in the
buffer and populates the address to a pointer that holds the compression buffer
and the size of the compression buffer.

    @param    inBuf            A pointer to a unsigned char buffer.
	          iBufLen          An integer indicates the size of the buffer.
              compressedBuf    An address a pointer variable that holds the
                               compression buffer.
              compressedByte   A pointer to integer that indicates the total
                               size of the compression buffer.
                               

    @return   rc               0 - No error.
              -EOVERFLOW       Comrpession has excceed the maximum buffer size.

******************************************************************************/
int RLEcompress(unsigned char *inBuf, int iBufLen, 
	            unsigned char **compressedBuf, int *compressedByte)
{
	int i, j;
	int rc = 0; 
	unsigned int count = 1;
	unsigned int outPos = 0;
	unsigned char outBuf[MAX_BUFF_SIZE];
	unsigned char *tmp_compressed;


	if ((!inBuf) || (iBufLen > MAX_BUFF_SIZE)) {
		return -EINVAL;
	}

	outBuf[0] = retSym(inBuf, iBufLen);
	outPos++;

	for (i = 0; i < iBufLen; i++) {
		if (inBuf[i] != inBuf[i + 1]) {

			if (inBuf[i] == outBuf[0]) {
				rc = fillRunLength(inBuf[i], outBuf, &outPos, count, B_TRUE, MAX_BUFF_SIZE);
				if (rc)
					return -EOVERFLOW;
			} else {
				if (count > 2) {
					rc = fillRunLength(inBuf[i], outBuf, &outPos, count, B_FALSE, MAX_BUFF_SIZE);
					if (rc)
						return -EOVERFLOW;
				} else {
					for (j = 0; j < count; j++) {
						if (outPos > MAX_BUFF_SIZE)
							return -EOVERFLOW;
						else {
							outBuf[outPos] = inBuf[i];
							outPos++;
						}
					}
				}
			}
			count = 1;
		} else {
			count++;
		}
	}

	if (!rc) {
		tmp_compressed = (unsigned char *)malloc(outPos);
		memcpy(tmp_compressed, outBuf, outPos);
		*compressedBuf = tmp_compressed;
		if(compressedByte)
			*compressedByte = outPos;
	}
	return rc;
}

/******************************************************************************
This function is a utility function that fill the decomrpession buffer with
RLE repeats character.

    @param    inBuf            A pointer to a unsigned char buffer.
              pos              An integer indicates the current spot which the
                               decompression buffer can be filled at.
              byte             A character or either RLE repeat chacater or
	                           speical control character.
              num              Number of repeat of the character in "byte".


   @return    N/A

******************************************************************************/
static void rle_sequence_expand(unsigned char *inBuf, unsigned int *pos,
	                            unsigned char byte, unsigned int num)
{
	unsigned int i;

	for (i = 0; i < num; i++) {
		inBuf[*pos] = byte;
		*pos += 1;
	}
}

/******************************************************************************
This function accepts an input buffer and decompressed the content in the
buffer and populates the address to a pointer that holds the decompression
buffer and the size of the decompression buffer.

    @param    inBuf            A pointer to a unsigned char buffer.
	          iBufLen          An integer indicates the size of the buffer.
              outBuf           An address a pointer variable that holds the
                               decompression buffer.
              decompressedByte A pointer to integer that indicates the total
                               size of the compression buffer.

    @return   rc               0 - No error.
              -EOVERFLOW       Comrpession has excceed the maximum buffer size.

******************************************************************************/
int RLEdecompress(unsigned char *inBuf, unsigned int inBufSize, 
	              unsigned char **outBuf, int *decompressedByte)
{
	int i = 1;
	unsigned int curPos = 0;
	unsigned char symChar;
	unsigned char tmp_uncompressed[MAX_BUFF_SIZE];
	unsigned char *tmp_out;
	int rleCCcount = 0;
	int rleRegCount = 0;

	symChar = inBuf[0];

	while (i < inBufSize) {
		if (inBuf[i] != symChar) {
			if (curPos > MAX_BUFF_SIZE)
				return -EOVERFLOW;
			else {
				tmp_uncompressed[curPos] = inBuf[i];
				curPos++;
				i++;
			}
		} else {
			if (inBuf[i + 2] == 0x00) {
				if ((curPos + inBuf[i + 1]) > MAX_BUFF_SIZE)
					return -EOVERFLOW;
				else {
					rleCCcount++;
					rle_sequence_expand(tmp_uncompressed, &curPos, inBuf[i], inBuf[i + 1]);
				}
			} else {
				if ((curPos + inBuf[i + 2]) > MAX_BUFF_SIZE)
					return -EOVERFLOW;
				else {
					rleRegCount++;
					rle_sequence_expand(tmp_uncompressed, &curPos, inBuf[i + 1], inBuf[i + 2]);
				}
			}
			i += 3;
		}
	}

	tmp_out = (unsigned char *)malloc(curPos);
	memcpy(tmp_out, tmp_uncompressed, curPos);
	*outBuf = tmp_out;

	if (decompressedByte)
		*decompressedByte = curPos;
	return 0;
}