#include "mfcc.h"

void audioDataInit(AUDIODATA * au){
	au->data = NULL;
	au->dataSize = 0;
}

void frameInit(Frame * frame){
	frame->frameData = NULL;
	frame->spectrum = NULL;
	frame->id = 0;
}
void setFrameInfo(FrameInfo * frameInfo, int frameSize, int frameOverlap, int frameNum){
	frameInfo->frameSize = frameSize;
	frameInfo->frameOverlap = frameOverlap;
	frameInfo->frameNum = frameNum;
	frameInfo->framePos = NULL;
}
double getFramePowerRMS(int frameStartPos, int frameEndPos, double * in){
	double sum = 0.0;
	int i = frameStartPos;
	while(i <= frameEndPos){
		sum += (in[i]*in[i]);
		++i;
	}
	return (double)sqrt(sum/(frameEndPos - frameStartPos + 1));
}
double getBaseNoisePower(double * in, int size, int openingFrameCnt, FrameInfo frameInfo){
	double res = 0.0;
	int i;
	for(i = 0; i < openingFrameCnt; ++i){
		res += getFramePowerRMS(frameInfo.framePos[i*2], frameInfo.framePos[i*2+1], in);
	}
	return res / openingFrameCnt;
}
void voiceActiveDetection(int * vadStartPos, int * vadEndPos, double * in, int size, int openingFrameCnt, FrameInfo frameInfo){
	double baseline = getBaseNoisePower(in, size, openingFrameCnt, frameInfo);
	int cnt = openingFrameCnt+1;
	Boolean terminate = FALSE, indicator = FALSE;
	int quota = 0;
	for(; ;++cnt){
		if(cnt > frameInfo.frameNum){
			printf("error: cannot find valid voice\n");
			terminate = TRUE;
			break;
		}
		double unit = getFramePowerRMS(frameInfo.framePos[(cnt-1)*2], frameInfo.framePos[(cnt-1)*2+1], in);
		if(unit > (baseline * _DEFAULT_DETECION_THRESHOLD_START_)){
			if(indicator == FALSE){
				quota = 1;
				indicator = TRUE;
			}
			else ++quota;
		}
		else if(indicator == TRUE){
			indicator = FALSE;
			quota = 0;
		}
		if(quota == _DEFAULT_DETECTION_CONTINUOUS_FRAME_NUM_){
			*vadStartPos = cnt;
			break;
		}
	}
	if(terminate == TRUE) return;
	cnt = frameInfo.frameNum;
	indicator = FALSE;
	quota = 0;
	for(; cnt > (*vadStartPos); --cnt){
		double unit = getFramePowerRMS(frameInfo.framePos[(cnt-1)*2], frameInfo.framePos[(cnt-1)*2+1], in);
		if(unit > (baseline * _DEFAULT_DETECION_THRESHOLD_END_)){
			if(indicator == FALSE){
				quota = 1;
				indicator = TRUE;
			}
			else ++quota;
		}
		else if(indicator == TRUE){
			indicator = FALSE;
			quota = 0;
		}
		if(quota == _DEFAULT_DETECTION_CONTINUOUS_FRAME_NUM_){
			*vadEndPos = cnt;
			break;
		}
	}
}
void preEmphasize(AUDIODATA * source, double * in, int voiceStartPos, int voiceEndPos, double a){
	int size = voiceEndPos - voiceStartPos+1;
	//printf("%d\n",size);
	source->data = (double *)malloc(sizeof(double)*size);
	int i = 1;
	for(; i < size; ++i){
		source->data[i] = in[i+voiceStartPos] - a*in[voiceStartPos + i-1];
	}
	source->data[0] = in[voiceStartPos];
	source->dataSize = size;
}
void hammingWindow(double * frame, int frameSize, double a){
	int i;
	for(i = 0; i < frameSize-1; ++i){
		frame[i] = ((1.0-a)-a*cos(2.0*PI*i/(frameSize-1)))*frame[i];
	}
}
void enFrame(FrameInfo * frameInfo, int size){
	int frameStartPos = 0, frameEndPos = 0;
	int i = 0;
	frameInfo->framePos = (int *)malloc(sizeof(int) * (frameInfo->frameNum * 2));
	while(1){
		if(frameEndPos == 0){
			frameStartPos = 0;
			frameEndPos = frameInfo->frameSize - 1;
		} 
		else{
			frameStartPos = frameEndPos - frameInfo->frameOverlap + 1;
			frameEndPos = frameStartPos + frameInfo->frameSize - 1; 
		}
		if(frameEndPos > size) {
			break;
		}
		frameInfo->framePos[i] = frameStartPos;
		frameInfo->framePos[i+1] = frameEndPos;
		i += 2;
	}
}
void updateFrameData(Frame *frame, FrameInfo frameInfo, double * in){
	int i;
	frame->frameData = (double *)malloc(sizeof(double)*frameInfo.frameSize);
	for(i = 0; i < frameInfo.frameSize; ++i){
		frame->frameData[i] = in[frameInfo.framePos[(frame->id-1)*2]+i];
	}
}
void dft_execute(Frame * frame, FrameInfo frameInfo){
	frame->spectrum = (double *)malloc(sizeof(double)*frameInfo.frameSize);
	int k,n;
	for(k = 0; k < frameInfo.frameSize/2+1; ++k){
		double real = 0.0, image = 0.0;
		for(n = 0; n < frameInfo.frameSize; ++n){
			real += frame->frameData[n]*cos(2*PI*k*n/frameInfo.frameSize);
			image += -frame->frameData[n]*sin(2*PI*k*n/frameInfo.frameSize);
		}
		frame->spectrum[k] = real*real+image*image;
	}
	for(k = frameInfo.frameSize-1; k >= frameInfo.frameSize/2+1; --k){
		frame->spectrum[k] = frame->spectrum[frameInfo.frameSize-k];
	}
}
double frequencyToMelScale(double f){
	return 1127*log(1.0+f/700.0);
}
double MelScaleToFrequency(double m){
	return 700*(exp(m/1127)-1);
}
int frequencyToPos(double f, int fs, int ndft){
	return f/fs*ndft+1;
}
void triangularWindow(int left, int middle, int right, double *out, double * in){
	double sum = 0.0;
	int i;
	for(i = left; i <= right; ++i){
		if(i < middle) sum += in[i-1]*(i-left)/(middle-left);
		else sum += in[i-1]*(right-i)/(right-middle);
	}
	*out = sum;
}
void setMelBankInfo(MelBankInfo * melBank, int fs, int low, int high, int ndft, int num){
	melBank->sampleRate = fs;
	melBank->low = low;
	melBank->high = high;
	melBank->ndft = ndft;
	melBank->filterSize = num;
	double low_mel = frequencyToMelScale(low), high_mel = frequencyToMelScale(high);
	double gap = (high_mel - low_mel)/(num+1);
	melBank->pos_vector = (int *)malloc(sizeof(int)*(num+2));
	int i;
	for(i = 0; i < (num+2); ++i){
		double tmp_hz = MelScaleToFrequency(low_mel + gap*i);
		melBank->pos_vector[i] = frequencyToPos(tmp_hz, fs, ndft);
	}
}
void melFilter(MelBankInfo melBank, MFCCInfo * mfcc_std, Frame frame){
	int i;
	mfcc_std->filterSize = melBank.filterSize;
	mfcc_std->filter = (double *)malloc(sizeof(double)*mfcc_std->filterSize);
	for(i = 0; i < melBank.filterSize; ++i){
		triangularWindow(melBank.pos_vector[i], melBank.pos_vector[i+1], melBank.pos_vector[i+2], mfcc_std->filter+i, frame.spectrum);
	}
}
void setMfccDimension(MFCCInfo * mfcc_std, int dim){
	mfcc_std->dimension = dim;
}
void dct_execute(MFCCInfo * mfcc_std){
	mfcc_std->mfcc_vector = (double *)malloc(sizeof(double)*mfcc_std->dimension);
	int i;
	for(i = 0; i < mfcc_std->dimension; ++i){
		int j;
		double sum = 0.0;
		for(j = 0; j < mfcc_std->filterSize; ++j){
			sum += log10(mfcc_std->filter[j]) * cos(PI*i*(2*j+1)/(2*mfcc_std->filterSize));
		}
		mfcc_std->mfcc_vector[i] = sum;
	}
}

void melBankDestroy(MelBankInfo * melbank){
	if(melbank->pos_vector != NULL){
		free(melbank->pos_vector);
		melbank->pos_vector = NULL;
	}
}
void mallocMFCCMatrix(MFCC_Matrix * mfcc_res, int dim, int frameNum){
	mfcc_res->dimension =  dim;
	mfcc_res->frameNum = frameNum;
	mfcc_res->coeff = (double **)malloc(sizeof(double *)*frameNum);
	double * inData = (double *)malloc(sizeof(double)*frameNum*dim);
	int i;
	for(i = 0; i < frameNum; ++i){
		mfcc_res->coeff[i] = &(inData[i*dim]);
	}
}
void updateFrameMfccInfo(MFCC_Matrix * mfcc_std, MFCCInfo frame, int frameIndex){
	if(frameIndex > mfcc_std->frameNum) {
		printf("error: wrong with frameNumber\n");
		return;
	}
	int i;
	for(i = 0; i < mfcc_std->dimension; ++i){
		mfcc_std->coeff[frameIndex-1][i] = frame.mfcc_vector[i];
	}
}
void mfcc_diff_execute(MFCC_Matrix * mfcc_std, MFCC_Matrix * mfcc_diff_1){
	mallocMFCCMatrix(mfcc_diff_1, mfcc_std->dimension, mfcc_std->frameNum);
	int i;
	for(i = 2; i < (mfcc_std->frameNum-2); ++i){
		int j;
		for(j = 0; j < mfcc_std->dimension; ++j){
			int cnt;
			for(cnt = 1; cnt < 3; ++cnt){
				mfcc_diff_1->coeff[i][j] += 0.1*(mfcc_std->coeff[i + cnt][j] - mfcc_std->coeff[i - cnt][j])*cnt;
			}
		}
	}
	for(i = 0; i < mfcc_std->dimension; ++i){
		mfcc_diff_1->coeff[0][i] = mfcc_std->coeff[0][i];
	}

	for(i = 0; i < mfcc_std->dimension; ++i){
		mfcc_diff_1->coeff[1][i] = mfcc_std->coeff[1][i];
	}

	for(i = 0; i < mfcc_std->dimension; ++i){
		mfcc_diff_1->coeff[mfcc_std->frameNum-2][i] = mfcc_std->coeff[mfcc_std->frameNum-2][i];
	}

	for(i = 0; i < mfcc_std->dimension; ++i){
		mfcc_diff_1->coeff[mfcc_std->frameNum-1][i] = mfcc_std->coeff[mfcc_std->frameNum-1][i];
	}
}

void dump(Frame * frame, MFCCInfo * mfcc){
	if(frame->frameData != NULL){
		free(frame->frameData);
		frame->frameData = NULL;
	}

	if(frame->spectrum != NULL){
		free(frame->spectrum);
		frame->spectrum = NULL;
	}

	if(mfcc->filter != NULL){
		free(mfcc->filter);
		mfcc->filter = NULL;
	}

	if(mfcc->mfcc_vector != NULL){
		free(mfcc->mfcc_vector);
		mfcc->mfcc_vector = NULL;
	}
}
void mfcc_execute(MFCC_Matrix * mfcc_std, double * originalData, int size, int sampleRate, int dim, int frame_overlap, int frameSize, int melFilterNum){
	int frameNum = (size-frameSize)/(frameSize - frame_overlap)+1;
	//printf("%d\n",frameNum);
	FrameInfo frameInfo;
	setFrameInfo(&frameInfo, frameSize, frame_overlap, frameNum);
	enFrame(&frameInfo, size);
	int vadStartPos = 0, vadEndPos = 0;
	voiceActiveDetection(&vadStartPos, &vadEndPos, originalData, size, 12, frameInfo);
	vadStartPos = vadStartPos - _FLEXIBLE_RANGED_;
	vadEndPos = vadEndPos + _FLEXIBLE_RANGED_;
	frameInfo.frameNum = vadEndPos - vadStartPos + 1;
	//printf("%d %d\n", vadStartPos, vadEndPos);
	//printf("%d %d\n", frameInfo.framePos[(vadStartPos*2)], frameInfo.framePos[(vadEndPos*2+1)]);
	AUDIODATA source;
	audioDataInit(&source);
	preEmphasize(&source, originalData, frameInfo.framePos[(vadStartPos*2)], frameInfo.framePos[(vadEndPos*2+1)], 0.9375);
	enFrame(&frameInfo, source.dataSize);
	mallocMFCCMatrix(mfcc_std, dim, frameInfo.frameNum);
	MelBankInfo melBank;
	melBank.pos_vector = NULL;
	setMelBankInfo(&melBank, sampleRate, 0, sampleRate/2, frameInfo.frameSize, melFilterNum);
	int cnt = 0;
	//printf("%d\n", frameInfo.frameNum);
	while(cnt < frameInfo.frameNum){
		++cnt;
		Frame unitFrame;
		unitFrame.frameData = NULL;
		unitFrame.spectrum = NULL;
		frameInit(&unitFrame);
		unitFrame.id = cnt;
		//printf("%d\n",cnt);
		updateFrameData(&unitFrame, frameInfo, source.data);
		hammingWindow(unitFrame.frameData, frameInfo.frameSize, 0.46);
		dft_execute(&unitFrame, frameInfo);
		MFCCInfo frameMFCC;
		frameMFCC.filter = NULL;
		frameMFCC.mfcc_vector = NULL;
		melFilter(melBank, &frameMFCC, unitFrame);
		setMfccDimension(&frameMFCC, dim);
		dct_execute(&frameMFCC);
		updateFrameMfccInfo(mfcc_std, frameMFCC, unitFrame.id);
		dump(&unitFrame, &frameMFCC);
	}
	melBankDestroy(&melBank);
}

void freeMatrix(double **in){
	double * data = *in;
	if(data != NULL){
		free(data);
		data = NULL;
	}
	if(in != NULL){
		free(in);
		in = NULL;
	}
}