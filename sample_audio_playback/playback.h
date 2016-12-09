/* playback.h
 */

int InitializePlayBacking(int sample_sec);
void ClosePlayback(void);
int PlayBuffer(short *piBuf, long bufferSize, int sample_sec);