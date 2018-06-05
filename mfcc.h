#ifndef _MFCC_H_
#define _MFCC_H_
#include "wav.h"
#define PI 3.1415926
#define _DEFAULT_DETECTION_CONTINUOUS_FRAME_NUM_ 5
#define _DEFAULT_DETECION_THRESHOLD_START_ 20
#define _DEFAULT_DETECION_THRESHOLD_END_ 10
#define _FLEXIBLE_RANGED_ 5
typedef struct {
	double * data;
	int dataSize;
}AUDIODATA;
typedef struct {
	int frameSize;
	int frameOverlap;
	int frameNum;
	int * framePos;
}FrameInfo;
typedef struct {
	double * frameData;
	double * spectrum;
	int id;
} Frame;
typedef struct {
	int sampleRate;
	int low;
	int high;
	int ndft;
	int filterSize;
	int *pos_vector;
}MelBankInfo;
typedef struct {
	double * filter;
	int filterSize;
	int dimension;
	double *mfcc_vector;
}MFCCInfo;
typedef struct {
	int dimension;
	int frameNum;
	double ** coeff;
}MFCC_Matrix;
void audioDataInit(AUDIODATA * au);
void setFrameInfo(FrameInfo *frameInfo, int frameSize, int frameOverlap, int frameNum);
double getBaseNoisePower(double * in, int size, int openingFrameCnt, FrameInfo frameInfo);
void voiceActiveDetection(int * vadStartPos, int * vadEndPos, double * in, int size, int openingFrameCnt, FrameInfo frameInfo);
void preEmphasize(AUDIODATA * source, double * in, int voiceStartPos, int voiceEndPos, double a);
void frameInit(Frame * frame);
void updateFrameData(Frame *frame, FrameInfo frameInfo, double * in);
void hammingWindow(double * frame, int frameSize, double a);
void enFrame(FrameInfo * frameInfo, int size);
void dft_execute(Frame * frame, FrameInfo frameInfo);
double frequencyToMelScale(double f);
double melScaleToFrequency(double m);
int frequencyToPos(double f, int fs, int ndft);
void triangularWindow(int left, int middle, int right, double *out, double * in);
void setMelBankInfo(MelBankInfo * melBank, int fs, int low, int high, int ndft, int num);
void melFilter(MelBankInfo melBank, MFCCInfo * mfcc_std, Frame frame);
void setMfccDimension(MFCCInfo * mfcc_std, int dim);
void dct_execute(MFCCInfo * mfcc_std);
void melBankDestroy(MelBankInfo * melBank);
void mallocMFCCMatrix(MFCC_Matrix * mfcc_res, int dim, int frameNum);
void freeMatrix(double ** in);
void updateFrameMfccInfo(MFCC_Matrix * mfcc_std, MFCCInfo frame, int frameIndex);
void mfcc_execute(MFCC_Matrix * mfcc_std, double * originalData, int size, int sampleRate, int dim, int frameSize, int frame_overlap, int melFilterNum);
void mfcc_diff_execute(MFCC_Matrix * mfcc_std, MFCC_Matrix * mfcc_diff_1);
void dump(Frame * frame, MFCCInfo * mfcc);
#endif