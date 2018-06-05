#include "wav.h"
#include "mfcc.h"
#include "dtw.h"
#include "train.h"
#include "arduino-serial-lib.h"
int main(){
	printf("训练: 0     测试: 1     查看数据: 2\n");
	Mode mode;
	int usersMode = 0;
	scanf("%d", &usersMode);
	if(usersMode == 0) mode = _TRAIN_;
	else if(usersMode == 1) mode = _TEST_;
	else mode = _RESERVED_;
	if(mode == _TRAIN_){
		char testFileName[20];
		char fixedFileName[20];
		printf("训练文件: \n");
		scanf("%s", testFileName);
		printf("数据存放位置: \n");
		scanf("%s", fixedFileName);
		int tSize = 0;
		int fSize = 0;
		while(testFileName[tSize] != '\0'){
			++tSize;
		}
		while(fixedFileName[fSize] != '\0'){
			++fSize;
		}
		char * testFilePath = arrangeFileName(testFileName, tSize, 1);
		char * fixedFilePath = arrangeFileName(fixedFileName, fSize, 2);
		WAV_STRUCT wavFileEg;
		wavStructInit(&wavFileEg);
		double * originData = wavFile_execute(testFilePath, &wavFileEg);
		//printWaveFileHeaderInfo(wavFileEg.wavHeader);
		MFCC_Matrix mfcc_std, mfcc_diff_1, mfcc_diff_2;
		mfcc_execute(&mfcc_std, originData, wavFileEg.numSamples, wavFileEg.wavHeader.fmt.sampleRate, 13, 128, 512, 24);
		mfcc_diff_execute(&mfcc_std, &mfcc_diff_1);
		mfcc_diff_execute(&mfcc_diff_1, &mfcc_diff_2);
		Boolean sign = update(mfcc_std, mfcc_diff_1, mfcc_diff_2, fixedFilePath);
		if(sign == TRUE){
			printf("training successful\n");
		}
		else {
			printf("error\n");
		}
		if(wavFileEg.data != NULL){
			free(wavFileEg.data);
			wavFileEg.data = NULL;
		}


		if(originData != NULL){
			free(originData);
			originData = NULL;
		}

		if(testFilePath != NULL){
			free(testFilePath);
			testFilePath = NULL;
		}

		if(fixedFilePath != NULL){
			free(fixedFilePath);
			fixedFilePath = NULL;
		}

		freeMatrix(mfcc_std.coeff);
		
		freeMatrix(mfcc_diff_1.coeff);
		
		freeMatrix(mfcc_diff_2.coeff);
	}
	else if(mode == _TEST_){
		char testFileName[20];
		printf("测试文件: \n");
		scanf("%s", testFileName);
		int tSize = 0;
		while(testFileName[tSize] != '\0'){
			++tSize;
		}
		char * testFilePath = arrangeFileName(testFileName, tSize, 1);
	
		WAV_STRUCT wavFileEg;
		wavStructInit(&wavFileEg);
		double * originData = wavFile_execute(testFilePath, &wavFileEg);
		//printWaveFileHeaderInfo(wavFileEg.wavHeader);
		MFCC_Matrix mfcc_std, mfcc_diff_1, mfcc_diff_2;
		mfcc_execute(&mfcc_std, originData, wavFileEg.numSamples, wavFileEg.wavHeader.fmt.sampleRate, 13, 128, 512, 24);
		mfcc_diff_execute(&mfcc_std, &mfcc_diff_1);
		mfcc_diff_execute(&mfcc_diff_1, &mfcc_diff_2);
		// CMPFactor cmpf;
		// getDegreeOfMatch(&cmpf, mfcc_std, mfcc_diff_1, mfcc_diff_2, _PERSON_1_TURN_ON_);
		// printf("%.6lf\n", cmpf.std.lower);
		// printf("%.6lf\n", cmpf.std.upper);
		// printf("%.6lf\n", cmpf.std.avg);
		// printf("****************\n");
		// printf("%.6lf\n", cmpf.diff_1.lower);
		// printf("%.6lf\n", cmpf.diff_1.upper);
		// printf("%.6lf\n", cmpf.diff_1.avg);
		// printf("****************\n");
		// printf("%.6lf\n", cmpf.diff_2.lower);
		// printf("%.6lf\n", cmpf.diff_2.upper);
		// printf("%.6lf\n", cmpf.diff_2.avg);
		// printf("****************\n");
		// printf("%.6lf\n", cmpf.matchIndex);
		_RECOGNITION_RESULT_ matchTarget = getMatchTarget(mfcc_std, mfcc_diff_1, mfcc_diff_2);
		printRecognitionResult(matchTarget);
		serialport_execute("/dev/cu.usbmodem1411", 9600, (char)(matchTarget + 48));
		if(wavFileEg.data != NULL){
			free(wavFileEg.data);
			wavFileEg.data = NULL;
		}


		if(originData != NULL){
			free(originData);
			originData = NULL;
		}

		if(testFilePath != NULL){
			free(testFilePath);
			testFilePath = NULL;
		}

		freeMatrix(mfcc_std.coeff);
	
		freeMatrix(mfcc_diff_1.coeff);
	
		freeMatrix(mfcc_diff_2.coeff);
	}
	else {
		char fixedFileName[20];
		printf("数据存放位置: \n");
		scanf("%s", fixedFileName);
		int fSize = 0;
		while(fixedFileName[fSize] != '\0'){
			++fSize;
		}
		char * fixedFilePath = arrangeFileName(fixedFileName, fSize, 2);
		MFCC_Matrix mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test;
		Boolean sign = load(&mfcc_std_test, &mfcc_diff_1_test, &mfcc_diff_2_test, fixedFilePath);
		if(sign == TRUE){
			int i,j;
			printf("*****************************匹配文件(%s)***********************\n", fixedFileName);
			for(i = 0; i < mfcc_std_test.frameNum; ++i){
				printf("第(%d)帧:\n",i+1);
				printf("mfcc系数: \n");
				for(j = 0; j < mfcc_std_test.dimension; ++j){
					printf("%.6lf ",mfcc_std_test.coeff[i][j]);
				}
				printf("\n");
				printf("mfcc一阶系数: \n");
				for(j = 0; j < mfcc_diff_1_test.dimension; ++j){
					printf("%.6lf ",mfcc_diff_1_test.coeff[i][j]);
				}
				printf("\n");
				printf("mfcc二阶系数: \n");
				for(j = 0; j < mfcc_diff_2_test.dimension; ++j){
					printf("%.6lf ",mfcc_diff_2_test.coeff[i][j]);
				}
				printf("\n");
			}
			freeMatrix(mfcc_std_test.coeff);
			freeMatrix(mfcc_diff_1_test.coeff);
			freeMatrix(mfcc_diff_2_test.coeff);
		}
	}
	// int i,j;
	// printf("*****************************测试文件(%s)***********************\n", testFileName);
	// for(i = 0; i < mfcc_std.frameNum; ++i){
	// 	printf("第(%d)帧:\n",i+1);
	// 	printf("mfcc系数: \n");
	// 	for(j = 0; j < mfcc_std.dimension; ++j){
	// 		printf("%.6lf ",mfcc_std.coeff[i][j]);
	// 	}
	// 	printf("\n");
	// 	printf("mfcc一阶系数: \n");
	// 	for(j = 0; j < mfcc_diff_1.dimension; ++j){
	// 		printf("%.6lf ",mfcc_diff_1.coeff[i][j]);
	// 	}
	// 	printf("\n");
	// 	printf("mfcc二阶系数: \n");
	// 	for(j = 0; j < mfcc_diff_2.dimension; ++j){
	// 		printf("%.6lf ",mfcc_diff_2.coeff[i][j]);
	// 	}
	// 	printf("\n");
	// }
	// MFCC_Matrix mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test;
	// Boolean sign = load(&mfcc_std_test, &mfcc_diff_1_test, &mfcc_diff_2_test, fixedFilePath);
	// if(sign == TRUE){
	// 	// printf("*****************************匹配文件(%s)***********************\n", fixedFileName);
	// 	// for(i = 0; i < mfcc_std_test.frameNum; ++i){
	// 	// 	printf("第(%d)帧:\n",i+1);
	// 	// 	printf("mfcc系数: \n");
	// 	// 	for(j = 0; j < mfcc_std_test.dimension; ++j){
	// 	// 		printf("%.6lf ",mfcc_std_test.coeff[i][j]);
	// 	// 	}
	// 	// 	printf("\n");
	// 	// 	printf("mfcc一阶系数: \n");
	// 	// 	for(j = 0; j < mfcc_diff_1_test.dimension; ++j){
	// 	// 		printf("%.6lf ",mfcc_diff_1_test.coeff[i][j]);
	// 	// 	}
	// 	// 	printf("\n");
	// 	// 	printf("mfcc二阶系数: \n");
	// 	// 	for(j = 0; j < mfcc_diff_2_test.dimension; ++j){
	// 	// 		printf("%.6lf ",mfcc_diff_2_test.coeff[i][j]);
	// 	// 	}
	// 	// 	printf("\n");
	// 	// }
	// 	printf("匹配数值: %.6lf\n", dtw_execute(mfcc_std, mfcc_std_test));
	// 	printf("匹配数值: %.6lf\n", dtw_execute(mfcc_diff_1, mfcc_diff_1_test));
	// 	printf("匹配数值: %.6lf\n", dtw_execute(mfcc_diff_2, mfcc_diff_2_test));
	// 	freeMatrix(mfcc_std_test.coeff);
	// 	freeMatrix(mfcc_diff_1_test.coeff);
	// 	freeMatrix(mfcc_diff_2_test.coeff);
	// }
	// else{
	// 	update(mfcc_std, mfcc_diff_1, mfcc_diff_2, fixedFilePath);
	// }
	

	return 0;
}