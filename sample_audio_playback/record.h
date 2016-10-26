#pragma once

int InitializeRecording(short *iBigBuf, long lBigBufSize, int sample_sec);
int	RecordBuffer(short *piBuf, long lBufSize, int sample_sec);
void CloseRecording(void);
