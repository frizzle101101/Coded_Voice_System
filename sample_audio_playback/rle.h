typedef enum {
	B_FALSE,
	B_TRUE
}BOOLYNS;

int RLEcompress(unsigned char *inBuf, int iBufLen, unsigned char **compressedBuf, int *compressedByte);
int RLEdecompress(unsigned char *inBuf, unsigned int inBufSize, unsigned char **outBuf, int *uncompressedByte);