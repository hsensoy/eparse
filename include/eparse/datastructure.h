//
//  datastructure.h
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 19/02/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//

#ifndef Perceptron_GLM_NLP_Tasks_datastructure_h
#define Perceptron_GLM_NLP_Tasks_datastructure_h
#include <stdbool.h>
#include "epblas.h"

/*
struct IntegerIndexedFeatures{
    Hashmap *map;
    uint32_t feature_id;
};

typedef struct IntegerIndexedFeatures* IntegerIndexedFeatures;
*/

/*
struct FeatureVector {
    DArray *discrete_v;
    vector continous_v;
};

typedef struct FeatureVector* FeatureVector;
*/

/*
struct FeatureMatrix{
    FeatureVector** matrix_data;
    uint16_t size;
    uint32_t embedding_length;
    bool has_discrete_features;
};

typedef  struct FeatureMatrix* FeatureMatrix;
*/

struct FeaturedSentence {
    uint8_t section;

    DArray* words;
    int length;
    //DArray* postags;
    //DArray* embedding;
    //DArray* parents;

    //DArray ***feature_matrix;   // For each potential link from-->to you have a set of features.
    //FeatureVector **feature_matrix;
    
    float **adjacency_matrix; // Score of each potential link between words
};


typedef struct FeaturedSentence* FeaturedSentence;

#endif
