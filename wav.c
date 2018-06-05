#include "wav.h"

void wavStructInit(WAV_STRUCT * wavFileEg){
	wavFileEg->numSamples = 0;
	wavFileEg->dataSize = 0;
	wavFileEg->data = NULL;
}

Boolean isDataChunk(WAV_DATASubchunk wd){
	if(wd.subchunk2ID[0] == 'd' && wd.subchunk2ID[1] == 'a' &&
		wd.subchunk2ID[2] == 't' && wd.subchunk2ID[3] == 'a')
		return TRUE;
	else return FALSE;
}
void printWaveFileHeaderInfo(WAV_HEADER wh){
	printf("chunkID: %.4s\n",wh.wavFileRIFFChunk.chunkID);
	printf("chunkSize: %u\n",wh.wavFileRIFFChunk.chunkSize);
	printf("format: %.4s\n",wh.wavFileRIFFChunk.format);
	printf("subchunk1ID: %.4s\n",wh.fmt.subchunk1ID);
	printf("subchunk1Size: %u\n",wh.fmt.subchunk1Size);
	printf("audioFormat: %u\n",wh.fmt.audioFormat);
	printf("numChannels: %u\n",wh.fmt.numChannels);
	printf("sampleRate: %u\n",wh.fmt.sampleRate);
	printf("byteRate: %u\n",wh.fmt.byteRate);
	printf("blockAlign: %u\n",wh.fmt.blockAlign);
	printf("bitsPerSample: %u\n",wh.fmt.bitsPerSample);
	printf("subchunk2ID: %.4s\n",wh.dataHeader.subchunk2ID);
	printf("subchunk2Size: %u\n",wh.dataHeader.subchunk2Size);
}

double * wavFile_execute(char * fileName, WAV_STRUCT * wavFileEg){
	FILE * fin = NULL;
	fin = fopen(fileName,"rb");
	if(!fin){
		printf("%s cannot open this file\n", fileName);
		exit(0);
	}
	else printf("%s opened successfully\n", fileName);
	fread(wavFileEg, sizeof(wavFileEg->wavHeader), 1, fin);
	while(isDataChunk(wavFileEg->wavHeader.dataHeader) == FALSE){
		int cnt = wavFileEg->wavHeader.dataHeader.subchunk2Size;
		while(cnt > 0){
			fgetc(fin);
			--cnt;
		}
		fread(&(wavFileEg->wavHeader.dataHeader),sizeof(wavFileEg->wavHeader.dataHeader),1,fin);
	}
	wavFileEg->dataSize = (int)wavFileEg->wavHeader.dataHeader.subchunk2Size;
	wavFileEg->numSamples = (int)wavFileEg->dataSize*8/wavFileEg->wavHeader.fmt.bitsPerSample/wavFileEg->wavHeader.fmt.numChannels;
	wavFileEg->data = (signed char *)malloc(sizeof(byte)*wavFileEg->dataSize);
	int i = 0;
	while(i < wavFileEg->dataSize && !feof(fin)){
		wavFileEg->data[i] = fgetc(fin);
		++i;
	}
	fclose(fin);
	double *originData = (double *)malloc(sizeof(double)*wavFileEg->numSamples);
	int gap = wavFileEg->wavHeader.fmt.bitsPerSample / 8 * wavFileEg->wavHeader.fmt.numChannels;
	if(wavFileEg->wavHeader.fmt.bitsPerSample == 8){
		i = 0;
		while(i < wavFileEg->numSamples){
			unsigned int tmp = wavFileEg->data[i * gap];
			originData[i] = (double)tmp / 255;
		}
	}
	else {
		i = 0;
		while(i < wavFileEg->numSamples){
			int tmp = (wavFileEg->data[i*gap+1])<<8|(wavFileEg->data[i*gap]);
			if(tmp > 0) originData[i] = (double)tmp/32767;
			else originData[i] = (double)tmp/32768;
			++i;
		}
	}
	return originData;
}