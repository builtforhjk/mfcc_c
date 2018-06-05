#include "dtw.h"
void mallocDistMatrix(DistAcceleratedMatrix * distMat, int row, int col){
	distMat->row =  row;
	distMat->col = col;
	distMat->dist = (double **)malloc(sizeof(double *)*row);
	double * inData = (double *)malloc(sizeof(double)*row*col);
	int i;
	for(i = 0; i < row; ++i){
		distMat->dist[i] = &(inData[i*col]);
	}
}

double getDistance(double * vector_1, double * vector_2, int startPos, int endPos){
	int i;
	double sum = 0.0;
	for(i = startPos - 1; i < endPos; ++i){
		sum += (vector_1[i] - vector_2[i]) * (vector_1[i] - vector_2[i]);
	}
	return (double)sqrt(sum);
}
void setDistMatrix(DistAcceleratedMatrix * distMat, MFCC_Matrix mfcc_mat_1, MFCC_Matrix mfcc_mat_2){
	if(mfcc_mat_1.dimension != mfcc_mat_2.dimension){
		printf("error: cannot calculation\n");
		return;
	}
	mallocDistMatrix(distMat, mfcc_mat_1.frameNum, mfcc_mat_2.frameNum);
	int i,j;
	for(i = 0; i < mfcc_mat_1.frameNum; ++i){
		for(j = 0; j < mfcc_mat_2.frameNum; ++j){
			distMat->dist[i][j] = getDistance(mfcc_mat_1.coeff[i], mfcc_mat_2.coeff[j], 2, mfcc_mat_1.dimension);
		}
	}
}

double getOptimalDistance(DistAcceleratedMatrix distMat){
	int * queue = (int *)malloc(sizeof(int) * (distMat.row * distMat.col * 2));
	int rear = 0, head = 0;
	DistAcceleratedMatrix optimalMatrix, visitedMatrix;
	mallocDistMatrix(&optimalMatrix, distMat.row, distMat.col);
	mallocDistMatrix(&visitedMatrix, distMat.row, distMat.col);
	int i,j;
	for(i = 0; i < visitedMatrix.row; ++i){
		for(j = 0; j < visitedMatrix.col; ++j){
			visitedMatrix.dist[i][j] = -1.0;
		}
	}
	queue[rear++] = 0;
	queue[rear++] = 0;
	while(head != rear){
		int tmp_x = queue[head++], tmp_y = queue[head++];
		double upDist = DBL_MAX, diagonalDist = DBL_MAX, leftDist = DBL_MAX;
		if(tmp_x > 0&& tmp_y > 0){
			diagonalDist = distMat.dist[tmp_x][tmp_y] + optimalMatrix.dist[tmp_x - 1][tmp_y - 1];
			upDist = distMat.dist[tmp_x][tmp_y] + optimalMatrix.dist[tmp_x - 1][tmp_y];
			leftDist = distMat.dist[tmp_x][tmp_y] + optimalMatrix.dist[tmp_x][tmp_y - 1];
		}
		else if(tmp_x > 0){
			upDist = distMat.dist[tmp_x][tmp_y] + optimalMatrix.dist[tmp_x - 1][tmp_y];
		}
		else if(tmp_y > 0){
			leftDist = distMat.dist[tmp_x][tmp_y] + optimalMatrix.dist[tmp_x][tmp_y - 1];
		}
		else{
			diagonalDist = distMat.dist[tmp_x][tmp_y];
		}
		double minDist = diagonalDist;
		if(minDist > upDist){
			minDist = upDist;
		}
		if(minDist > leftDist){
			minDist = leftDist;
		}
		optimalMatrix.dist[tmp_x][tmp_y] = minDist;
		//printf("(%d, %d): %lf\n", tmp_x+1, tmp_y+1, optimalMatrix.dist[tmp_x][tmp_y]);
		if(tmp_y < visitedMatrix.col - 1 && visitedMatrix.dist[tmp_x][tmp_y + 1] < 0){
			visitedMatrix.dist[tmp_x][tmp_y + 1] = 1.0;
			queue[rear++] = tmp_x;
			queue[rear++] = tmp_y + 1;
		}
		if(tmp_x < visitedMatrix.row - 1 && visitedMatrix.dist[tmp_x + 1][tmp_y] < 0){
			visitedMatrix.dist[tmp_x + 1][tmp_y] = 1.0;
			queue[rear++] = tmp_x + 1;
			queue[rear++] = tmp_y;
		}
	}
	free(queue);
	queue = NULL;
	
	double res = optimalMatrix.dist[optimalMatrix.row - 1][optimalMatrix.col - 1];
	freeMatrix(optimalMatrix.dist);
	freeMatrix(visitedMatrix.dist);
	return res;
}

double dtw_execute(MFCC_Matrix mfcc_mat_1, MFCC_Matrix mfcc_mat_2){
	DistAcceleratedMatrix distMatrix;
	setDistMatrix(&distMatrix, mfcc_mat_1, mfcc_mat_2);
	double res = getOptimalDistance(distMatrix);
	freeMatrix(distMatrix.dist);
	return res;
}