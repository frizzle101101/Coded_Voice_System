#include "record.h"
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>


#define RECORD_TIME		6		// seconds to record for
#define SAMPLES_SEC		8000	// samples per second

static	int		g_nSamplesPerSec = SAMPLES_SEC;
static	int		g_nBitsPerSample = 16;
static	WAVEFORMATEX WaveFormat;
static  HWAVEIN		HWaveIn;
static  WAVEHDR WaveHeaderIn;

static short iBigBuf[SAMPLES_SEC * RECORD_TIME];
static long	 lBigBufSize = SAMPLES_SEC * RECORD_TIME;	// in samples


static void SetupFormat(WAVEFORMATEX *wf)
{
	wf->wFormatTag = WAVE_FORMAT_PCM;
	wf->nChannels = 1;
	wf->nSamplesPerSec = g_nSamplesPerSec;
	wf->wBitsPerSample = g_nBitsPerSample;
	wf->nBlockAlign = wf->nChannels * (wf->wBitsPerSample / 8);
	wf->nAvgBytesPerSec = wf->nSamplesPerSec * wf->nBlockAlign;
	wf->cbSize = 0;
	return;
}

static int WaitOnHeader(WAVEHDR *wh, char cDit)
{
	long	lTime = 0;

	// wait for whatever is being played, to finish. Quit after 10 seconds.
	for (; ; ) {
		if (wh->dwFlags & WHDR_DONE) return(1);
		// idle for a bit so as to free CPU
		Sleep(100L);
		lTime += 100;
		if (lTime >= 10000) return(0);  // timeout
		if (cDit) printf("%c", cDit);
	}
}

static int Errorp(const char *szFormat, ...)
{
	char		szBuf[300];
	char		*ptr = szBuf;
	va_list		arg_ptr;
	int         i;
	int			iMode = MB_OK | MB_SETFOREGROUND;

	/* build a buffer with the message */
	va_start(arg_ptr, szFormat);
	i = vsprintf(szBuf, szFormat, arg_ptr);
	va_end(arg_ptr);
	// allow the first character to customize the box 
	if (*ptr == '!') {
		iMode |= MB_ICONEXCLAMATION;
		++ptr;
	}
	else
		if (*ptr == '@') {
			iMode |= MB_ICONINFORMATION;
			++ptr;
		}
		else
			if (*ptr == '?') {
				iMode |= MB_ICONQUESTION;
				++ptr;
			}
			else
				if (*ptr == '#') {
					iMode |= MB_ICONSTOP;
					++ptr;
				}

	// display the box
	MessageBox(NULL, ptr, "NOTE", iMode);
	return(i);
}


int InitializeRecording(void)
{
	MMRESULT rc;

	// set up the format structure, needed for recording.
	SetupFormat(&WaveFormat);

	// open the recording device
	rc = waveInOpen(&HWaveIn, WAVE_MAPPER, &WaveFormat, (DWORD)NULL, 0, CALLBACK_NULL);
	if (rc) {
		Errorp("Unable to open Input sound Device! Error %x.", rc);
		return(0);
	}

	// prepare the buffer header for use later on
	WaveHeaderIn.lpData = (char *)iBigBuf;
	WaveHeaderIn.dwBufferLength = lBigBufSize*sizeof(short);
	rc = waveInPrepareHeader(HWaveIn, &WaveHeaderIn, sizeof(WAVEHDR));
	if (rc) {
		Errorp("Failed preparing input WAVEHDR, error 0x%x.", rc);
		return(0);
	}

	return(1);

}

/* RecordBuffer() : Fill in the buffer from input device.
* Returns 0 for failure.
*/
int	RecordBuffer(short *piBuf, long lBufSize)
{
	static	WAVEFORMATEX WaveFormat;	/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
	static  WAVEHDR	WaveHeader;			/* WAVEHDR structure for this buffer */
	MMRESULT	mmErr;
	int		rc;

	printf("Recording now.....");

	// stop previous recording (just in case)
	waveInReset(HWaveIn);   // is this good?

	// get the header ready for recording.  This should not be needed here AND in init.
	WaveHeader.lpData = (char *)piBuf;
	WaveHeader.dwBufferLength = lBufSize*sizeof(short);
	rc = waveInPrepareHeader(HWaveIn, &WaveHeader, sizeof(WAVEHDR));
	if (rc) {
		Errorp("Failed preparing WAVEHDR, error 0x%x.", rc);
		return(0);
	}
	WaveHeader.dwFlags &= ~(WHDR_BEGINLOOP | WHDR_ENDLOOP);

	// Give the buffer to the recording device to fill.
	mmErr = waveInAddBuffer(HWaveIn, &WaveHeader, sizeof(WAVEHDR));
	if (mmErr != MMSYSERR_NOERROR) {
		char	szErrBuf[150];
		waveOutGetErrorText(mmErr, szErrBuf, sizeof(szErrBuf));
		Errorp("Error 0x%x writing to playback device. %s", mmErr, szErrBuf);
		return(0);
	}

	// Record the buffer. This is NON-blocking.
	mmErr = waveInStart(HWaveIn);
	if (mmErr != MMSYSERR_NOERROR) {
		char	szErrBuf[150];
		waveOutGetErrorText(mmErr, szErrBuf, sizeof(szErrBuf));
		Errorp("Error 0x%x writing to playback device. %s", mmErr, szErrBuf);
		return(0);
	}

	// wait for completion
	rc = WaitOnHeader(&WaveHeader, '.');

	// probably not necessary
	waveInStop(HWaveIn);

	return(rc);
}

void CloseRecording(void)
{
	waveInUnprepareHeader(HWaveIn, &WaveHeaderIn, sizeof(WAVEHDR));

	// close the playback device
	waveInClose(HWaveIn);

	return;
}

