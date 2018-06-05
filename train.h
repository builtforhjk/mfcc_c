#ifndef _TRAIN_H_

#define _TRAIN_H_

#include "wav.h"
#include "mfcc.h"
#include "dtw.h"

#define _CANNOT_MATCH_STANDARD_ 5399.0
#define _NO_MATCH_THRESHOLD 1155.0
typedef struct Tuple{
    double upper;
    double avg;
    double lower;
    double refIndex;
}CMPParameter;
typedef struct DMatch{
    CMPParameter std;
    CMPParameter diff_1;
    CMPParameter diff_2;
    double matchIndex;
}CMPFactor;

typedef enum RECOGNITION_RESULT{
    _PERSON_1_TURN_ON_ = 1,
    _PERSON_1_TURN_DOWN_, 
    _PERSON_2_TURN_ON_,
    _PERSON_2_TURN_DOWN_,
    _PERSON_3_TURN_ON_,
    _PERSON_3_TURN_DOWN_,
    _PERSON_4_TURN_ON_,
    _PERSON_4_TURN_DOWN_,
    _UNKNOWN_TARGET_
} _RECOGNITION_RESULT_;

typedef enum Mode{ _TRAIN_ = 0, _TEST_, _RESERVED_}Mode;


char * arrangeFileName(char * relativePath, int size, int type); 
void CMPParameterInit(CMPParameter * cmpp);
void CMPFactorInit(CMPFactor * cmpf);
Boolean load(MFCC_Matrix *mfcc_std, MFCC_Matrix *mfcc_diff_1, MFCC_Matrix *mfcc_diff_2, char * binFileName);
Boolean update(const MFCC_Matrix mfcc_std, const MFCC_Matrix mfcc_diff_1, const MFCC_Matrix mfcc_diff_2, char * binFileName);

void getDegreeOfMatch(CMPFactor * cmpf, MFCC_Matrix mfcc_std_test, MFCC_Matrix mfcc_diff_1_test, MFCC_Matrix mfcc_diff_2_test, _RECOGNITION_RESULT_ TYPE);

_RECOGNITION_RESULT_ getMatchTarget(MFCC_Matrix mfcc_std_test, MFCC_Matrix mfcc_diff_1_test, MFCC_Matrix mfcc_diff_2_test);

void printRecognitionResult(_RECOGNITION_RESULT_ );
#endif