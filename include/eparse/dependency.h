//
//  dependency.h
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 13/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//

#ifndef Perceptron_GLM_NLP_Tasks_dependency_h
#define Perceptron_GLM_NLP_Tasks_dependency_h

#include "darray.h"
#include "corpus.h"
#include "datastructure.h"
#include "stringalgo.h"
#include "util.h"
#include "perceptron.h"
#include "feattemplate.h"

#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TRAINING_SENTENCE 50000

enum FeatureGroup {
    pword_ppos = 0,
    pword = 1,
    ppos = 2,
    cword_cpos = 3,
    cword = 4,
    cpos = 5,
    pword_ppos_cword_cpos = 6,
    ppos_cword_cpos = 7,
    pword_ppos_cpos = 8,
    pword_ppos_cword = 9,
    pword_cword = 10,
    ppos_cpos = 11,
    ppos_pposP1_cposM1_cpos = 12,
    pposM1_ppos_cposM1_cpos = 13,
    ppos_pposP1_cpos_cposP1 = 14,
    pposM1_ppos_cpos_cposP1 = 15,
    ppos_bpos_cpos = 16
};

typedef enum FeatureGroup FeatureGroup;


//IntegerIndexedFeatures IntegerIndexedFeatures_create();

struct FeatureKey {
    FeatureGroup grp;
    char* value;
};

typedef struct FeatureKey* FeatureKey;

/**
 n1: n1++ if feature defines an arc
 n2: n2++ if feature occurs for a potential arc
 */
struct FeatureValue {
    uint32_t feature_id;
    uint32_t n1;
    uint32_t n2;
};

typedef struct FeatureValue* FeatureValue;

/*
struct PerceptronModel {
    IntegerIndexedFeatures features;

    Vector_t discrete_w;
    Vector_t discrete_w_avg;
    Vector_t discrete_w_temp;

    Vector_t embedding_w;
    Vector_t embedding_w_avg;
    Vector_t embedding_w_temp;
    Vector_t embedding_w_best;

    int best_numit;

    int c;

    size_t n;

    bool use_discrete_features;
};

typedef struct PerceptronModel* PerceptronModel;
*/


struct HeadPredictionMetric{
    int true_prediction;
    int total_prediction;
};

typedef struct HeadPredictionMetric* HeadPredictionMetric;

/**
 *      all: Number of head predictions made correctly
 *      without_punc: Number of head predictions made correctly (punctuation heads are excluded)
 *      true_root_predicted: Number of roots predicted to be true
 *      total_sentence: Total number of sentence.
 *      complete_sentence: +1 if all head predictions are correct for the sentence.
 *      complete_sentence_without_punc: +1 if all head predictions are correnct for non-punctuation words in a sentence.
 *      
 */
struct ParserTestMetric{
    HeadPredictionMetric all;
    HeadPredictionMetric without_punc;
    
    int true_root_predicted;
    
    int total_sentence;
    int complete_sentence;
    int complete_sentence_without_punc;
};

typedef struct ParserTestMetric* ParserTestMetric;



/**
    FeatureKey, FeatureValue creation functions.
 */
FeatureKey FeatureKey_create(FeatureGroup group, char* value);
FeatureValue FeatureValue_create(uint32_t fid);


int feature_equal(void *k1, void *k2);
uint32_t feature_hash(void *f);

/**
 * 
 * @param transformed_embedding_length Transformed embedding length.
 * @param iif For feature enhancements (current value is always NULL)
 * @return initialized Perceptron model
 */
//PerceptronModel create_PerceptronModel(size_t transformed_embedding_length, IntegerIndexedFeatures iif);
//void PerceptronModel_free(PerceptronModel model);

//void train_perceptron_parser(PerceptronModel mdl, const CoNLLCorpus corpus, int numit, int max_rec);
//void train_once_PerceptronModel(PerceptronModel mdl, const CoNLLCorpus corpus, int max_rec);

void trainPerceptronOnce(Perceptron_t mdl, const CoNLLCorpus corpus, int max_rec);

ParserTestMetric create_ParserTestMetric();
void printParserTestMetric(ParserTestMetric metric);

void freeParserTestMetric(ParserTestMetric ptm);

//void fill_features(Hashmap *featuremap, DArray *farr, int from, int to, FeaturedSentence sentence);
void parse_and_dump(Perceptron_t mdl, FILE *fp, CoNLLCorpus corpus);

void parse(FeaturedSentence , int[]);
void get_parents(const FeaturedSentence sent, int[]);
int nmatch(const int* model, const int* empirical, int length);
void printfarch(int *parent, int len);

//void mark_best_PerceptronModel(PerceptronModel model, int numit);

#endif
