#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "seekaudio_afc.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif


#define FRAME_SIZE 160


static int afc_Test()
{
  FILE *fin = NULL;
  FILE *fout = NULL;
  int count = 0;
  short in[FRAME_SIZE];
  short out[FRAME_SIZE];
  
  fin = fopen("near.pcm", "rb");
  fout = fopen("out.pcm", "wb");

  if (!fin)
	  return 0;
  if (!fout)
	  return 0;

  printf("afctest£¬start\n");
  
  AFCHandle*denoise = SeekAudioAFC_Create();
  //SeekAudioAFC_OpenLog(denoise, NULL);
  SeekAudioAFC_Init(denoise, 16000);
  //SeekAudioAFC_Set_AI_Engine_Power(denoise, 25);
  
  while (1) {
		short in[FRAME_SIZE];
		fread(in, sizeof(short), FRAME_SIZE, fin);
		if (feof(fin)) break;

		count++;

		SeekAudioAFC_Process(denoise, in, out);

		if(fout)
			fwrite(out, sizeof(short), FRAME_SIZE, fout);
	}
	
	SeekAudioAFC_Free(denoise);

	if (fin)
		fclose(fin);
	if (fout)
		fclose(fout);
	printf("afctest£¬end\n");
  return 0;
}

int main()
{
	afc_Test();
}