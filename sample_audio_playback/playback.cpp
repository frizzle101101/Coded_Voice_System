/* Playback.cpp
 * By: Jack Cole, Modified by Michael Galle (October 22, 2016)
 * Functions for playback of audio data, to be adapted for S2, Project 2. 
 * This version creates buffers for all notes from C0 to D#8.
 *
 * A testing main() is included that can be used with the following command line arguments:
 * #1 #2  : play a half second of note number #1 then #2 etc (up to 10), 1 - NFREQUENCIES
 * A-Ga-g : play a half second of the notes A-G or a-g (up to 10)
 * r : record 6 seconds (amount of time may be edited below) and then play it back. 
 * 
 */

/*
 * Solution (Right Click) -> Properties -> character set -> "Use multibyte character set"
 */

#pragma comment(lib, "Ws2_32.lib")	   // Make sure we are linking against the Ws2_32.lib library
#pragma comment(lib, "Winmm.lib")      // Make sure we are linking against the Winmm.lib library - some functions/symbols from this library (Windows sound API) are used

#define TESTING

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>

#include "console.h"				// Used for Graphical User Interface (optional)
#include "playback.h"    			// Used for audio recording and playback


// output and input channel parameters
static	int		g_nBitsPerSample = 16;
static	HWAVEOUT	HWaveOut;				/* Handle of opened WAVE Out and In device */
static	WAVEFORMATEX WaveFormat;			/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
static  WAVEHDR	WaveHeader;			/* WAVEHDR structure for this buffer */

static int WaitOnHeader( WAVEHDR *wh, char cDit );

/* SetupFormat() initializes a WAVEFORMATEX structure to the required 
 *				 parameters (sample rate, bits per sample, etc)
 */
static void SetupFormat( WAVEFORMATEX *wf, int sample_sec )
{	
	wf->wFormatTag = WAVE_FORMAT_PCM;
	wf->nChannels = 1;
	wf->nSamplesPerSec = sample_sec;
	wf->wBitsPerSample = g_nBitsPerSample;
	wf->nBlockAlign = wf->nChannels * (wf->wBitsPerSample/8);
	wf->nAvgBytesPerSec = wf->nSamplesPerSec * wf->nBlockAlign;
	wf->cbSize = 0;
	return;
}

/* InitializePlayback()
 */

int InitializePlayBacking(int sample_sec)
{
	int		rc;

	// set up the format structure, needed for playback (and recording)
	SetupFormat( &WaveFormat, sample_sec);

	// open the playback device
	rc = waveOutOpen(&HWaveOut, WAVE_MAPPER, &WaveFormat, (DWORD)NULL, 0, CALLBACK_NULL);
	if ( rc ) {
		Errorp("Unable to open Output sound Device! Error %x.", rc);
		return(0);
	}

	return(1);
}

/* PlayBuffer()
 */
int PlayBuffer( short *piBuf, long lSamples, int sample_sec)
{
	MMRESULT	mmErr;
	int		rc;

	InitializePlayBacking(sample_sec);
	// stop previous note (just in case)
	waveOutReset(HWaveOut);   // is this good?

	// get the header ready for playback
	WaveHeader.lpData = (char *)piBuf;
	WaveHeader.dwBufferLength = lSamples*sizeof(short);
	rc = waveOutPrepareHeader(HWaveOut, &WaveHeader, sizeof(WAVEHDR));
	if ( rc ) {
		Errorp( "Failed preparing WAVEHDR, error 0x%x.", rc);
		return(0);
	}
	WaveHeader.dwFlags &= ~(WHDR_BEGINLOOP|WHDR_ENDLOOP);

	// play the buffer. This is NON-blocking.
	mmErr = waveOutWrite(HWaveOut, &WaveHeader, sizeof(WAVEHDR));
	if ( mmErr != MMSYSERR_NOERROR ) {
		char	szErrBuf[150];
		waveOutGetErrorText(mmErr, szErrBuf, sizeof(szErrBuf) );
		Errorp("Error 0x%x writing to playback device. %s", mmErr, szErrBuf );
		return(0);
	}

	// wait for completion
	rc = WaitOnHeader(&WaveHeader, 0);

	// give back resources
	waveOutUnprepareHeader(HWaveOut, &WaveHeader, sizeof(WAVEHDR));

	return(rc);
}

static int WaitOnHeader( WAVEHDR *wh, char cDit )
{
	long	lTime = 0;

	// wait for whatever is being played, to finish. Quit after 10 seconds.
	for ( ; ; ) {
		if ( wh->dwFlags & WHDR_DONE ) return(1);
		// idle for a bit so as to free CPU
		Sleep(100L);
		lTime += 100; 
		if ( lTime >= 10000 ) return(0);  // timeout
		if ( cDit ) printf("%c", cDit);
	}
}

void ClosePlayback(void)
{
	// close the playback device
	waveOutClose(HWaveOut);
	
	return;
}