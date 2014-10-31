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



#ifdef __GNUC__
#include <sys/types.h>
#endif

#define NEGATIVE_INFINITY (-(FLT_MAX - 10.))
#define EXAMPLE_CONLL_DIR "/Users/husnusensoy/uparse/data/nlp/treebank/treebank-2.0/combined/conll"

#define  STOP  "<STOP>"
#define START  "*"
//static const char* ROOT = "root";


#define IS_ARC_VALID(from,to, length) check((from) != (to) && (from) <= (length) && (from) >= 0 && (to)>= 1 && (to) <= (length), "Arc between suspicious words %d to %d for sentence length %d", (from), (to), (length))

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

    bool hasembeddings;
    DArray *disrete_patterns_parts;

    Word Root;
    int word_embedding_dimension;
    long transformed_embedding_length;
};

typedef struct CoNLLCorpus* CoNLLCorpus;






enum EmbeddingTranformation{
    CUBIC,
    QUADRATIC,
    LINEAR
};

struct EmbeddingPattern {
    int offset;
    char node;
    char subnode;
};

typedef struct EmbeddingPattern* EmbeddingPattern;



CoNLLCorpus create_CoNLLCorpus(const char* base_dir, DArray *sections, int embedding_dimension, DArray* discrete_patterns) ;
void read_corpus(CoNLLCorpus coprus, bool build_feature_matrix);

void free_CoNLLCorpus(CoNLLCorpus corpus, bool free_feature_matrix);



void add_word(FeaturedSentence sentence, Word word);

FeaturedSentence FeatureSentence_create();
void FeatureSentence_free(FeaturedSentence sent, bool free_words);

void free_feature_matrix(CoNLLCorpus corpus, int sentence_idx);

void build_adjacency_matrix(CoNLLCorpus corpus, int sentence_idx, Vector_t embeddings_w, Vector_t discrete_w);
void setAdjacencyMatrix(CoNLLCorpus corpus, int sentence_idx, Perceptron_t kp, bool use_avg_alpha);

void free_FeaturedSentence(CoNLLCorpus corpus, int sentence_idx);

eparseError_t embedding_feature(FeaturedSentence sent, int from, int to, Vector_t target);

#endif
