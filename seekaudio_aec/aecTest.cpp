#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "seekaudio_aec.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif


#define FRAME_SIZE 160

static int aec_Test()
{
	FILE *fnear = NULL;
	FILE *ffar = NULL;
	FILE *fout = NULL;

	int count = 0;
	short nearbuf[FRAME_SIZE];
	short farbuf[FRAME_SIZE];
	short outbuf[FRAME_SIZE];
	AECHandle*aecInst = NULL;

	aecInst = SeekAudioAEC_Create();

	fnear = fopen("near.pcm", "rb");
	ffar = fopen("far.pcm", "rb");
	fout = fopen("aecout.pcm", "wb");


	if (!fnear)
		return 0;
	if (!ffar)
		return 0;
	if (!fout)
		return 0;

	printf("aecTest.start\n");

	//SeekAudioAEC_OpenLog(aecInst, "D:/test/");
	SeekAudioAEC_Init(aecInst, 16000);
	//SeekAudioAEC_Set_AI_Engine_Power_For_Howl(aecInst, 10);
	//SeekAudioAEC_Set_AI_Engine_Power_For_Echo(aecInst, 5);

	while (1) {
		fread(nearbuf, sizeof(short), FRAME_SIZE, fnear);
		if (feof(fnear)) break;

		fread(farbuf, sizeof(short), FRAME_SIZE, ffar);
		if (feof(ffar)) break;

		count++;

		SeekAudioAEC_buffer_farend(aecInst, farbuf, FRAME_SIZE);
		SeekAudioAEC_Process(aecInst, nearbuf, outbuf, FRAME_SIZE);

		if (fout)
			fwrite(outbuf, sizeof(short), FRAME_SIZE, fout);
	}
	
	if (fnear)
		fclose(fnear);
	if (ffar)
		fclose(ffar);
	if (fout)
		fclose(fout);
	if (aecInst)
		SeekAudioAEC_Free(aecInst);

	printf("aecTest.end\n");
	return 0;
}

int main()
{
	aec_Test();
}