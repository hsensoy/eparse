//
// Created by husnu sensoy on 02/05/15.
//

#ifndef EPARSE_FEATTEMPLATE_H
#define EPARSE_FEATTEMPLATE_H

#include "debug.h"
#include "regex.h"
#include "util.h"
#include "eputil.h"
#include "datastructure.h"
#include "corpus.h"
#include "murmurhash.h"

enum featureTemplateError_t {
    featureTemplateSucess = 0,
    featureTemplateMemoryError,
    featureTemplateInvalidTemplate,
    featureTemplateOtherError
};

typedef enum featureTemplateError_t featureTemplateError_t;

#define FEATURETEMPLATE_CHECK_RETURN(v) {                                            \
        featureTemplateError_t stat = v;                                        \
        if (stat != featureTemplateSucess) {                                        \
            fprintf(stderr, "Error %d at line %d in file %s\n",                    \
                    stat, __LINE__, __FILE__);        \
            exit(1);                                                            \
        } }

enum FeatureType {
    FT_THRESHOLDED_DISTANCE,
    FT_NORMALIZED_DISTANCE,
    FT_RAW_DISTANCE,
    FT_EMBEDDING,
    FT_POSTAG,
    FT_BETWEEN_POSTAG,
    FT_BETWEEN_EMBEDDING,
    FT_ROOT,
    FT_DIRECTION,
    FT_RIGHT_BOUNDARY_FLAG,
    FT_LEFT_BOUNDARY_FLAG
};

#define NODE_PARENT 'p'
#define NODE_CHILD 'c'
#define NODE_NONE '-'

#define SEED 0
#define DEFAULT_N_DISC_FEATURE 500

struct FeatureTemplate_st {
    int nfeatures;
    enum FeatureType *type;
    int *offset;
    char *node;
    long *start;
    long *end;

    uint32_t ndisc_feature;


    //Intermediate structures for performance enhancements
    Vector_t avg_v;
    Vector_t disc;
};


typedef struct FeatureTemplate_st *FeatureTemplate_t;

FeatureTemplate_t createFeatureTemplate(const char *templatestr,uint32_t max_feature);

#define  STOP  "<STOP>"
#define START  "*"
//static const char* ROOT = "root";

#define ROOT_EMBEDDING_VAL 0.f


#define VIRTUAL_SUBVECTOR(vector, start, end) {                 \
if (start == -1 && end == -1){                                  \
     vdata = (vector)->data;                                    \
     vlength = (vector)->n;                                     \
}else{                                                          \
    check((start) >= 0 && (end) >= 0 && (end) > (start) && (end) <= (vector)->n,"Invalid subvector range of [%ld,%ld] for %s of length %ld", (start),(end), (vector)->identifier, (vector)->n);     \
    vdata = (vector)->data + (start);                 \
    vlength = (end) - (start);                         \
}                                                       \
}                                                       \



#define IS_ARC_VALID(from, to, length, section) check((from) != (to) && (from) <= (length) && (from) >= 0 && (to)>= 1 && (to) <= (length), "Arc between suspicious words %d to %d for section %u sentence length %d", (from), (to),(section), (length))


featureTemplateError_t arc_feature_vector(FeatureTemplate_t ft, FeaturedSentence sent, int from, int to,
                                          Vector_t *target);

featureTemplateError_t sentence_feature_matrix(FeatureTemplate_t ft, FeaturedSentence sent, Matrix_t *target);

//featureTemplateError_t checkFeatureTemplate(const Word w, const FeatureTemplate_t ft);

featureTemplateError_t printFeatureTemplate(const FeatureTemplate_t ft);
#endif //EPARSE_FEATTEMPLATE_H
