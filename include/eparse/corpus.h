//
//  corpus.h
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 13/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//

#ifndef Perceptron_GLM_NLP_Tasks_corpus_h
#define Perceptron_GLM_NLP_Tasks_corpus_h
#include <dirent.h>
#include <math.h>
#include <float.h>

#include "darray.h"
#include "datastructure.h"
#include "util.h"
#include "perceptron.h"

#include "stringalgo.h"

#include "debug.h"
//#include "dependency.h"

#include "conll.h"
#include "featuretransform.h"
#include "feattemplate.h"


#ifdef __GNUC__
#include <sys/types.h>
#endif

#define NEGATIVE_INFINITY (-(FLT_MAX - 10.))
#define EXAMPLE_CONLL_DIR "/Users/husnusensoy/uparse/data/nlp/treebank/treebank-2.0/combined/conll"


#define CONLL_EMBEDDING_INDEX 10

struct Word {
    int id;
    int parent;
    
    int predicted_parent;       // Parent predicted by the model.
    
    char *form;
    char *postag;
    
    DArray *conll_piece;

    Vector_t embedding;
};

typedef struct Word* Word;

struct CoNLLCorpus {
    const char *base_dir;
    DArray* sections;

    DArray *sentences;

};

typedef struct CoNLLCorpus* CoNLLCorpus;


CoNLLCorpus create_CoNLLCorpus(const char* base_dir, DArray *sections);
Word parse_word(char* line);
Vector_t parse_vector(char *buff);
void read_corpus(CoNLLCorpus coprus, int max_sent, bool build_feature_matrix);

void free_CoNLLCorpus(CoNLLCorpus corpus, bool free_feature_matrix);



void add_word(FeaturedSentence sentence, Word word);

FeaturedSentence FeatureSentence_create();
void FeatureSentence_free(FeaturedSentence sent, bool free_words);

void free_feature_matrix(CoNLLCorpus corpus, int sentence_idx);


void setAdjacencyMatrix(CoNLLCorpus corpus, int sentence_idx, Perceptron_t kp, bool use_avg_alpha);

void free_FeaturedSentence(CoNLLCorpus corpus, int sentence_idx);


#endif
