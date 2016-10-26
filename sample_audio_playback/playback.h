/* playback.h
 */

int InitializePlayback(void);
void ClosePlayback(void);
int PlayBuffer( short *piBuf, long lSamples );