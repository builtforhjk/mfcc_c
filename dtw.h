#ifndef _DTW_H_

#define _DTW_H_

#include "wav.h"
#include "mfcc.h"

typedef struct {
	int row;
	int col;
	double ** dist;
}DistAcceleratedMatrix;

void mallocDistMatrix(DistAcceleratedMatrix * distMat, int row, int col);
double getDistance(double * vector_1, double * vector_2, int startPos, int endPos);
void setDistMatrix(DistAcceleratedMatrix * distMat ,MFCC_Matrix mfcc_mat_1, MFCC_Matrix mfcc_mat_2);
double getOptimalDistance(DistAcceleratedMatrix distMat);
double dtw_execute(MFCC_Matrix mfcc_mat_1, MFCC_Matrix mfcc_mat_2);
#endif