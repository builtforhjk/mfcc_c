#ifndef _WAV_H_
#define _WAV_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <stddef.h>
typedef enum Boolean{FALSE = 0, TRUE = 1} Boolean;
typedef  signed char byte;
typedef struct {
	char chunkID[4];
	unsigned int chunkSize;
	char format[4];
}WAV_RIFFChunkDescriptor;
typedef struct {
	char subchunk1ID[4];
	unsigned int subchunk1Size;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned int sampleRate;
	unsigned int byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
}WAV_FMTSubchunk;
typedef struct {
	char subchunk2ID[4];
	unsigned int subchunk2Size;
}WAV_DATASubchunk;
typedef struct {
	WAV_RIFFChunkDescriptor wavFileRIFFChunk;
	WAV_FMTSubchunk fmt;
	WAV_DATASubchunk dataHeader;
}WAV_HEADER;
typedef struct {
	WAV_HEADER wavHeader;
	int numSamples;
	int dataSize;
	byte * data;
}WAV_STRUCT;

void wavStructInit(WAV_STRUCT * wavFileEg);
Boolean isDataChunk(WAV_DATASubchunk);
void printWaveFileHeaderInfo(WAV_HEADER);
double * wavFile_execute(char * fileName, WAV_STRUCT * wavFileEg);
#endif