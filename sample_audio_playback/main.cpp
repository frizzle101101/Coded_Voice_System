#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "record.h"
#include "playback.h"
#include "console.h"

#define RECORD_TIME		6		// seconds to record for
#define SAMPLES_SEC		8000	// samples per second

static short iBigBuf[SAMPLES_SEC * RECORD_TIME];
static long	 lBigBufSize = SAMPLES_SEC * RECORD_TIME;	// in sample

int	main(int argc, char *argv[])
{
	int	iTestPlayRecord = 0;        // THIS VALUE CHANGES TO 1 TO RECORD FROM MIC

	if (toupper(*argv[1]) == 'R') {
		iTestPlayRecord = 1;
	}
	InitializePlayback();

	if (iTestPlayRecord) {
		// Record the special buffer
		InitializeRecording();
		RecordBuffer(iBigBuf, lBigBufSize);
		CloseRecording();
		// play the special buffer
		printf("Playing special %d..\n", 60);
		PlayBuffer(iBigBuf, lBigBufSize);
	}

	ClosePlayback();
	return(0);
}