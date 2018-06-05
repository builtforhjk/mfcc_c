#include "train.h"


char * arrangeFileName(char * relativePath, int size, int type){
    //WARNING
    const char prefix_absolutePath[36] = "/Users/a123/Downloads/assignment/";
    const int prefix_absolutePath_size = 33;
    const char suffix_dataFile[10] = ".bin";
    const int suffix_dataFile_size = 4;
    const char suffix_audioFile[10] = ".wav";
    const int suffix_audioFile_size = 4;
    char * filePath = (char *)malloc(sizeof(char) * 100);
    int i;
    for(i = 0; i < prefix_absolutePath_size; ++i){
        filePath[i] = prefix_absolutePath[i];
    }
    for(i = 0; i < size; ++i){
        filePath[i + prefix_absolutePath_size] = relativePath[i];
    }
    switch(type){
        case 1:
            for(i = 0; i < suffix_audioFile_size; ++i){
                filePath[i + prefix_absolutePath_size + size] = suffix_audioFile[i];
            }
            filePath[i + prefix_absolutePath_size + size + suffix_audioFile_size] = '\0';
            break;
        case 2:
            for(i = 0; i < suffix_dataFile_size; ++i){
                filePath[i + prefix_absolutePath_size + size] = suffix_dataFile[i];
            }
            filePath[i + prefix_absolutePath_size + size + suffix_dataFile_size] = '\0';
            break;
        default:
            break;
    }
    return filePath;
}

void CMPParameterInit(CMPParameter * cmpp){
    cmpp->upper = DBL_MIN;
    cmpp->avg = 0.0;
    cmpp->lower = DBL_MAX;
    cmpp->refIndex = _CANNOT_MATCH_STANDARD_;
}
void CMPFactorInit(CMPFactor * cmpf){
    CMPParameterInit(&(cmpf->std));
    CMPParameterInit(&(cmpf->diff_1));
    CMPParameterInit(&(cmpf->diff_2));
}
Boolean update(const MFCC_Matrix mfcc_std, const MFCC_Matrix mfcc_diff_1, const MFCC_Matrix mfcc_diff_2, char * binFileName){
    FILE * fp = NULL;
    fp = fopen(binFileName, "wb");
    if(!fp){
        printf("error: write search path failed\n");
        return FALSE;
    }
    fwrite(&(mfcc_std.dimension), sizeof(int), 1, fp);
    fwrite(&mfcc_std.frameNum, sizeof(int), 1, fp);
    int i = 0;
    for(; i < mfcc_std.frameNum; ++i){
        int j = 0;
        for(; j < mfcc_std.dimension; ++j){
            fwrite(&(mfcc_std.coeff[i][j]), sizeof(double), 1, fp);
        }
    }
    for(i = 0; i < mfcc_diff_1.frameNum; ++i){
        int j = 0;
        for(; j < mfcc_diff_1.dimension; ++j){
            fwrite(&(mfcc_diff_1.coeff[i][j]), sizeof(double), 1, fp);
        }
    }
    for(i = 0; i < mfcc_diff_2.frameNum; ++i){
        int j = 0;
        for(; j < mfcc_diff_2.dimension; ++j){
            fwrite(&(mfcc_diff_2.coeff[i][j]), sizeof(double), 1, fp);
        }
    }
    fclose(fp);
    return TRUE;
}

Boolean load(MFCC_Matrix *mfcc_std, MFCC_Matrix *mfcc_diff_1, MFCC_Matrix *mfcc_diff_2, char * binFileName){
    FILE * fp = NULL;
    fp = fopen(binFileName, "rb");
    if(!fp){
        printf("error: read search path failed\n");
        return FALSE;
    }
    int dim = 0, frameNum = 0;
    fread(&dim, sizeof(int), 1, fp);
    fread(&frameNum, sizeof(int), 1, fp);
    mallocMFCCMatrix(mfcc_std, dim, frameNum);
    mallocMFCCMatrix(mfcc_diff_1, dim, frameNum);
    mallocMFCCMatrix(mfcc_diff_2, dim, frameNum);
    int i;
    for(i = 0; i < mfcc_std->frameNum; ++i){
        int j = 0;
        for(; j < mfcc_std->dimension; ++j){
            if(feof(fp)){
                printf("error: file size insufficient\n");
                return FALSE;
            }
            fread(&(mfcc_std->coeff[i][j]), sizeof(double), 1, fp);
        }
    }
    for(i = 0; i < mfcc_diff_1->frameNum; ++i){
        int j = 0;
        for(; j < mfcc_diff_1->dimension; ++j){
            if(feof(fp)){
                printf("error: file size insufficient\n");
                return FALSE;
            }
            fread(&(mfcc_diff_1->coeff[i][j]), sizeof(double), 1, fp);
        }
    }
    for(i = 0; i < mfcc_diff_2->frameNum; ++i){
        int j = 0;
        for(; j < mfcc_diff_2->dimension; ++j){
            if(feof(fp)){
                printf("error: file size insufficient\n");
                return FALSE;
            }
            fread(&(mfcc_diff_2->coeff[i][j]), sizeof(double), 1, fp);
        }
    }
    fclose(fp);
    return TRUE;
}

void getDegreeOfMatch(CMPFactor * cmpf, MFCC_Matrix mfcc_std_test, MFCC_Matrix mfcc_diff_1_test, MFCC_Matrix mfcc_diff_2_test, _RECOGNITION_RESULT_ TYPE){
    CMPFactorInit(cmpf);
    int sample_id, content_id;
    if(TYPE == _PERSON_1_TURN_ON_){
        sample_id = 1;
        content_id = 0;
    }
    else if(TYPE == _PERSON_1_TURN_DOWN_){
        sample_id = 1;
        content_id = 1;
    }
    else if(TYPE == _PERSON_2_TURN_ON_){
        sample_id = 2;
        content_id = 0;
    }
    else if(TYPE == _PERSON_2_TURN_DOWN_){
        sample_id = 2;
        content_id = 1;
    }
    else if(TYPE == _PERSON_3_TURN_ON_){
        sample_id = 3;
        content_id = 0;
    }
    else if(TYPE == _PERSON_3_TURN_DOWN_){
        sample_id = 3;
        content_id = 1;
    }
    else if(TYPE == _PERSON_4_TURN_ON_){
        sample_id = 4;
        content_id = 0;
    }
    else if(TYPE == _PERSON_4_TURN_DOWN_){
        sample_id = 4;
        content_id = 1;
    }
    else return;
    int i = 1;
    //WARNING 
    char dataFilePath[100] = "/Users/a123/Downloads/assignment/sample01/sample01.bin";
    dataFilePath[48] = content_id + 48;
    dataFilePath[40] = sample_id + 48;
    while(i <= 5){
        MFCC_Matrix mfcc_std, mfcc_diff_1, mfcc_diff_2;
        dataFilePath[49] = i + 48;
        Boolean sign = load(&mfcc_std, &mfcc_diff_1, &mfcc_diff_2, dataFilePath);
        if(sign == TRUE){
            double std_degree_match = dtw_execute(mfcc_std_test, mfcc_std);
            double diff_1_degree_match = dtw_execute(mfcc_diff_1_test, mfcc_diff_1);
            double diff_2_degree_match = dtw_execute(mfcc_diff_2_test, mfcc_diff_2);
            if(cmpf->std.upper < std_degree_match){
                cmpf->std.upper = std_degree_match;
            }
            if(cmpf->std.lower > std_degree_match){
                cmpf->std.lower = std_degree_match;
            }
            cmpf->std.avg += std_degree_match;
            if(cmpf->diff_1.upper < diff_1_degree_match){
                cmpf->diff_1.upper = diff_1_degree_match;
            }
            if(cmpf->diff_1.lower > diff_1_degree_match){
                cmpf->diff_1.lower = diff_1_degree_match;
            }
            cmpf->diff_1.avg += diff_1_degree_match;
            if(cmpf->diff_2.upper < diff_2_degree_match){
                cmpf->diff_2.upper = diff_2_degree_match;
            }
            if(cmpf->diff_2.lower > diff_2_degree_match){
                cmpf->diff_2.lower = diff_2_degree_match;
            }
            cmpf->diff_2.avg += diff_2_degree_match;
            freeMatrix(mfcc_std.coeff);
            freeMatrix(mfcc_diff_1.coeff);
            freeMatrix(mfcc_diff_2.coeff);
        }
        else break;
        ++i;
    }
    cmpf->std.avg = cmpf->std.avg / 5;
    cmpf->diff_1.avg = cmpf->diff_1.avg / 5;
    cmpf->diff_2.avg = cmpf->diff_2.avg / 5;
    cmpf->std.refIndex = cmpf->std.lower * 0.25 + cmpf->std.avg * 0.65 + cmpf->std.upper * 0.1;
    cmpf->diff_1.refIndex = cmpf->diff_1.lower * 0.25 + cmpf->diff_1.avg * 0.65 + cmpf->diff_1.upper * 0.1;
    cmpf->diff_2.refIndex = cmpf->diff_2.lower * 0.25 + cmpf->diff_2.avg * 0.65 + cmpf->diff_2.upper * 0.1;
    cmpf->matchIndex = cmpf->std.refIndex * 0.8 + cmpf->diff_1.refIndex + cmpf->diff_2.refIndex;
    printf("%d\n",TYPE);
    printf("std lower: %.6lf\n", cmpf->std.lower);
    printf("std upper: %.6lf\n", cmpf->std.upper);
    printf("std avg: %.6lf\n", cmpf->std.avg);
    printf("std refIndex: %.6lf\n", cmpf->std.refIndex);
    printf("diff_1 lower: %.6lf\n", cmpf->diff_1.lower);
    printf("diff_1 upper: %.6lf\n", cmpf->diff_1.upper);
    printf("diff_1 avg: %.6lf\n", cmpf->diff_1.avg);
    printf("diff_1 refIndex: %.6lf\n", cmpf->diff_1.refIndex);
    printf("diff_2 lower: %.6lf\n", cmpf->diff_2.lower);
    printf("diff_2 upper: %.6lf\n", cmpf->diff_2.upper);
    printf("diff_2 avg: %.6lf\n", cmpf->diff_2.avg);
    printf("diff_2 refIndex: %.6lf\n", cmpf->diff_2.refIndex);
    printf("matchIndex: %.6lf\n", cmpf->matchIndex);
    printf("***********************\n\n\n\n");
}

_RECOGNITION_RESULT_ getMatchTarget(MFCC_Matrix mfcc_std_test, MFCC_Matrix mfcc_diff_1_test, MFCC_Matrix mfcc_diff_2_test){
    CMPFactor cmpf_person_1_turn_down, cmpf_person_1_turn_on;
    CMPFactor cmpf_person_2_turn_down, cmpf_person_2_turn_on;
    CMPFactor cmpf_person_3_turn_down, cmpf_person_3_turn_on;
    CMPFactor cmpf_person_4_turn_down, cmpf_person_4_turn_on;
    getDegreeOfMatch(&cmpf_person_1_turn_on, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_1_TURN_ON_);
    getDegreeOfMatch(&cmpf_person_1_turn_down, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_1_TURN_DOWN_);
    getDegreeOfMatch(&cmpf_person_2_turn_on, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_2_TURN_ON_);
    getDegreeOfMatch(&cmpf_person_2_turn_down, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_2_TURN_DOWN_);
    getDegreeOfMatch(&cmpf_person_3_turn_on, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_3_TURN_ON_);
    getDegreeOfMatch(&cmpf_person_3_turn_down, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_3_TURN_DOWN_);
    getDegreeOfMatch(&cmpf_person_4_turn_on, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_4_TURN_ON_);
    getDegreeOfMatch(&cmpf_person_4_turn_down, mfcc_std_test, mfcc_diff_1_test, mfcc_diff_2_test, _PERSON_4_TURN_DOWN_);
    
    _RECOGNITION_RESULT_ matchTarget = _PERSON_1_TURN_DOWN_;
    double pivot = cmpf_person_1_turn_down.matchIndex;
    if(pivot > cmpf_person_1_turn_on.matchIndex){
        matchTarget = _PERSON_1_TURN_ON_;
        pivot = cmpf_person_1_turn_on.matchIndex;
    }
    if(pivot > cmpf_person_2_turn_down.matchIndex){
        matchTarget = _PERSON_2_TURN_DOWN_;
        pivot = cmpf_person_2_turn_down.matchIndex;
    }
    if(pivot > cmpf_person_2_turn_on.matchIndex){
        matchTarget = _PERSON_2_TURN_ON_;
        pivot = cmpf_person_2_turn_on.matchIndex;
    }
    if(pivot > cmpf_person_3_turn_down.matchIndex){
        matchTarget = _PERSON_3_TURN_DOWN_;
        pivot = cmpf_person_3_turn_down.matchIndex;
    }
    if(pivot > cmpf_person_3_turn_on.matchIndex){
        matchTarget = _PERSON_3_TURN_ON_;
        pivot = cmpf_person_3_turn_on.matchIndex;
    }
    if(pivot > cmpf_person_4_turn_down.matchIndex){
        matchTarget = _PERSON_4_TURN_DOWN_;
        pivot = cmpf_person_4_turn_down.matchIndex;
    }
    if(pivot > cmpf_person_4_turn_on.matchIndex){
        matchTarget = _PERSON_4_TURN_ON_;
        pivot = cmpf_person_4_turn_on.matchIndex;
    }
    if(pivot > _NO_MATCH_THRESHOLD) matchTarget = _UNKNOWN_TARGET_;
    return matchTarget;
}

void printRecognitionResult(_RECOGNITION_RESULT_ matchTarget){
    if(matchTarget == _PERSON_1_TURN_ON_) printf("识别人: PERSON_1     指令: 开机\n");
    else if(matchTarget == _PERSON_1_TURN_DOWN_) printf("识别人: PERSON_1     指令: 关机\n");
    else if(matchTarget == _PERSON_2_TURN_ON_) printf("识别人: PERSON_2     指令: 开机\n");
    else if(matchTarget == _PERSON_2_TURN_DOWN_) printf("识别人: PERSON_2     指令: 关机\n");
    else if(matchTarget == _PERSON_3_TURN_ON_) printf("识别人: PERSON_3     指令: 开机\n");
    else if(matchTarget == _PERSON_3_TURN_DOWN_) printf("识别人: PERSON_3     指令: 关机\n");
    else if(matchTarget == _PERSON_4_TURN_ON_) printf("识别人: PERSON_4     指令: 开机\n");
    else if(matchTarget == _PERSON_4_TURN_DOWN_) printf("识别人: PERSON_4     指令: 关机\n");
    else printf("无法匹配已有模板\n");
}