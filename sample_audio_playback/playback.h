/* playback.h
 */

int InitializePlayBacking(int sample_sec);
void ClosePlayback(void);
int PlayBuffer(short *piBuf, long lSamples, int sample_sec);