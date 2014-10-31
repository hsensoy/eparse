//
//  corpus.c
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 13/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//
#include "corpus.h"

Word Root = NULL;

long max_num_sv = 0;
long max_narc = 0;

static Vector_t embedding_v = NULL;
static Vector_t xformed_embedding_v = NULL;
static float zero = 0.;

static Vector_t avg_v = NULL;
static Vector_t distance_v = NULL;

/**
 * ai-parse.c file for actual storage allocation for those two variables
 */
extern const char *epattern;
extern enum EmbeddingTranformation etransform;
extern float rbf_lambda;
extern int edimension;

DArray* embedding_pattern_parts = NULL;

EmbeddingPattern create_EmbeddingPattern() {
    EmbeddingPattern pattern = (EmbeddingPattern) malloc(sizeof (struct EmbeddingPattern));
    check(pattern != NULL, "Embedding Pattern allocation error");

    return pattern;

error:
    exit(1);
}

// Singleton

DArray* get_embedding_pattern_parts() {
    if (embedding_pattern_parts == NULL) {
        if (epattern != NULL) {
            DArray* patterns = split(epattern, "_");

            embedding_pattern_parts = DArray_create(sizeof (EmbeddingPattern), DArray_count(patterns));

            for (int pi = 0; pi < DArray_count(patterns); pi++) {
                char *pattern = (char*) DArray_get(patterns, pi);

                EmbeddingPattern ep = create_EmbeddingPattern();

                if (strcmp(pattern, "tl") == 0) { //thresholded-length
                    ep->node = 'l';
                    ep->subnode = 't';
                }
                else if (strcmp(pattern, "nl") == 0) { // normalized-length
                    ep->node = 'l';
                    ep->subnode = 'n';
                }
                else if (strcmp(pattern, "l") == 0) { // raw length
                    ep->node = 'l';
                    ep->subnode = 'r';
                }
                else if (strcmp(pattern, "lbf") == 0) { // Left Boundary Flag
                    ep->node = 'b';
                    ep->subnode = 'l';
                }
                else if (strcmp(pattern, "rbf") == 0) { // Right Boundary Flag
                    ep->node = 'b';
                    ep->subnode = 'r';
                }
                else if (strcmp(pattern, "dir") == 0) { // Direction
                    ep->node = 'd';
                }
                else if (strcmp(pattern, "root") == 0) { // Root Flag
                    ep->node = 'r';
                }
                else if (strcmp(pattern, "between") == 0) { //Between words
                    ep->node = 'w';
                }
                else {

                    int n = sscanf(pattern, "%c%dv", &(ep->node), &(ep->offset));

                    check(n == 2, "Expected pattern format is [p|c]<offset>v where as got %s", pattern);
                    check(ep->node == 'p' || ep->node == 'c', "Unknown node name %c expected p or c", ep->node);
                }

                DArray_push(embedding_pattern_parts, ep);
            }

            debug("Number of embedding patterns is %d", DArray_count(embedding_pattern_parts));

        }
        else {
            embedding_pattern_parts = NULL;
        }

        return embedding_pattern_parts;
    }
    else {
        return embedding_pattern_parts;
    }

error:
    return NULL;
}

Word ROOT(int dim) {
    if (Root == NULL) {
        Root = (Word) malloc(sizeof (struct Word));
        check_mem(Root);

        Root->id = 0;
        Root->form = strdup("ROOT");
        Root->postag = strdup("ROOT");
        Root->parent = -1;
        Root->embedding = NULL;
        
        newInitializedCPUVector(&(Root->embedding), "Root Embedding",dim,matrixInitNone,NULL,NULL)
              
    }


    return Root;


error:

    exit(1);
}

/**
 * 
 * @param base_dir CoNLL base directory including sections
 * @param sections DArray of sections
 * @param embedding_dimension Embedding dimension per word
 * @param discrete_patterns Reserved for future use
 * @return CoNLLCorpus structure
 */
CoNLLCorpus create_CoNLLCorpus(const char* base_dir, DArray *sections, int embedding_dimension, DArray* discrete_patterns) {
    CoNLLCorpus corpus = (CoNLLCorpus) malloc(sizeof (struct CoNLLCorpus));

    check_mem(corpus);

    corpus->base_dir = base_dir;
    corpus->sections = sections;

    corpus->sentences = DArray_create(sizeof (FeaturedSentence), 2000);
    check_mem(corpus->sentences);

    corpus->hasembeddings = embedding_dimension > 0;

    if (discrete_patterns) {
        corpus->disrete_patterns_parts = DArray_create(sizeof (DArray*), DArray_count(discrete_patterns));
        check_mem(corpus->disrete_patterns_parts);

        for (int i = 0; i < DArray_count(discrete_patterns); i++)
            DArray_push(DArray_get(corpus->disrete_patterns_parts, i), split(((char*) DArray_get(discrete_patterns, i)), "_"));


    }
    else
        corpus->disrete_patterns_parts = NULL;

    corpus->Root = ROOT(embedding_dimension);

    corpus->word_embedding_dimension = embedding_dimension;
    corpus->transformed_embedding_length = -1;

    int embedding_concat_length = 1;
    for (int pi = 0; pi < DArray_count(get_embedding_pattern_parts()); pi++) {
        EmbeddingPattern pattern = (EmbeddingPattern) DArray_get(get_embedding_pattern_parts(), pi);

        if (pattern->node == 'p' || pattern->node == 'c' || pattern->node == 'w')
            embedding_concat_length += embedding_dimension;
        else if (pattern->node == 'l' && (pattern->subnode == 'r' || pattern->subnode == 'n'))
            embedding_concat_length += 1;
        else if (pattern->node == 'l' && pattern->subnode == 't')
            embedding_concat_length += 9;
        else if (pattern->node == 'b')
            embedding_concat_length += 2;
        else if (pattern->node == 'r')
            embedding_concat_length += 1;
        else if (pattern->node == 'd')
            embedding_concat_length += 2;
    }


    newInitializedCPUVector(&embedding_v, "Embedding Vector", embedding_concat_length, matrixInitFixed, &zero, NULL)

    corpus->transformed_embedding_length = embedding_concat_length;
    if (etransform == QUADRATIC)
        corpus->transformed_embedding_length = ((corpus->transformed_embedding_length) * (corpus->transformed_embedding_length + 1)) / 2;
    else if (etransform == CUBIC) {
        int emprical_xform_length = 0;
        for (int i = 0; i < embedding_concat_length; i++) {
            for (int j = 0; j <= i; j++) {
                for (int k = 0; k <= j; k++) {
                    emprical_xform_length++;
                }
            }
        }

        corpus->transformed_embedding_length = emprical_xform_length;
    }

    
    newInitializedCPUVector(&xformed_embedding_v, "Transformed Embedding Vector", corpus->transformed_embedding_length, matrixInitFixed, &zero, NULL)
    log_info("Corpus has an embedding length of %d (%ld by %d transformation)", embedding_dimension, corpus->transformed_embedding_length, etransform);



    return corpus;
error:
    exit(1);
}

void free_CoNLLCorpus(CoNLLCorpus corpus, bool free_feature_matrix) {

    for (int si = 0; si < DArray_count(corpus->sentences); si++) {
        //FeaturedSentence sent = (FeaturedSentence)DArray_get(sentences, i);

        free_FeaturedSentence(corpus, si);
    }

    debug("Sentences are released");

    if (corpus->disrete_patterns_parts != NULL)
        DArray_clear_destroy(corpus->disrete_patterns_parts);

    DArray *epparts = get_embedding_pattern_parts();
    if (epparts != NULL)
        DArray_clear_destroy(epparts);

    debug("Patterns are released");


}

#define CONCAT_EMBEDDING(target,source){                                                    \
        memcpy( ((target)->data) + offset, (source)->data, ((source)->n * sizeof(float)));   \
        offset+=(source)->n;                                                                 \
}

#define CONCAT_SINGLE_REF(target, value){                          \
        memcpy( ((target)->data) + offset, &(value), sizeof(float));   \
        offset+=1;                                                   \
}

#define CONCAT_SINGLE_VALUE(target, value){                          \
        ((target)->data)[offset] = (value);                           \
        offset+=1;                                                   \
}

/**
 * 
 * @param sent
 * @param from
 * @param to
 * @param target When NULL a new vector is created by vlinear/vquadratic functions. Release of memory is deferred to user.
 *                      When a non-NULL vector is given vlinear/vquadratic functions simply perform a copy operation with no new allocation.
 * @return 
 */
eparseError_t embedding_feature(FeaturedSentence sent, int from, int to, Vector_t target) {
    Vector_t avg_v = NULL;
    
    IS_ARC_VALID(from, to, sent->length);

    newInitializedCPUVector(&target, "Embedding Vector", target->n, matrixInitFixed, &zero, NULL)

    DArray* patterns = get_embedding_pattern_parts();

    long offset = 0;

    for (int pi = 0; pi < DArray_count(patterns); pi++) {
        EmbeddingPattern pattern = (EmbeddingPattern) DArray_get(patterns, pi);

        if (pattern->node == 'p') {
            if (from == 0)
                CONCAT_EMBEDDING(target, Root->embedding)
            else if (from + pattern->offset >= 1 && from + pattern->offset <= sent->length)    
                CONCAT_EMBEDDING(target, ((Word) DArray_get(sent->words, from + pattern->offset - 1))->embedding)
            else 
                CONCAT_EMBEDDING(target, Root->embedding)
        }
        else if (pattern->node == 'c') {

            if (to + pattern->offset >= 1 && to + pattern->offset <= sent->length) 
                CONCAT_EMBEDDING(target, ((Word) DArray_get(sent->words, to + pattern->offset - 1))->embedding)
            else 
                CONCAT_EMBEDDING(target, Root->embedding)
            


        }
        else if (pattern->node == 'w') {

            //log_info("Embedding dimension %d",edimension);
            newInitializedCPUVector(&avg_v, "Average vector of embeddings in between",edimension,matrixInitFixed,&zero,NULL)
            
            //log_info("Initialization is done");

            if (abs(from - to) > 1) {

                int n = 0;

                for (int b = MIN(from, to) + 1; b < MAX(from, to); b++) {


                    //log_info("from=%d, to=%d, b=%d",MIN(from, to),MAX(from, to), b);
                    Vector_t b_vec = ((Word) DArray_get(sent->words, b - 1))->embedding;

                    for (long bi = 0; bi < b_vec->n; bi++)
                        (avg_v->data)[bi] += (b_vec->data)[bi];

                    n++;
                }

                for (long bi = 0; bi < avg_v->n; bi++)
                    (avg_v->data)[bi] /= n;

            }
            
            CONCAT_EMBEDDING(target, avg_v)
        }
        else if (pattern->node == 'l') {

            if (pattern->subnode == 't') {
                const int threshold_arr[] = {1, 2, 3, 4, 5, 10, 20, 30, 40};
                float threshold_flag[9];

                for (int i = 0; i < 9; i++)
                    if (abs(from - to) > threshold_arr[i])
                        threshold_flag[i] = 1;
                    else
                        threshold_flag[i] = 0;
                
                
                newInitializedCPUVector(&distance_v, "from-to distance vector", 9, matrixInitCArray,threshold_flag,NULL)
                
                CONCAT_EMBEDDING(target, distance_v)

            }
            else if (pattern->subnode == 'r') {
                float rawdist  = abs(from - to);

                CONCAT_SINGLE_REF(target, rawdist)
            }
            else if (pattern->subnode == 'n') {
                float normdist  = abs(from - to)/ 250.;

                CONCAT_SINGLE_REF(target, normdist)
            }

        }
        else if (pattern->node == 'b') {
            if (pattern->subnode == 'l') {
                float boundary;

                if (from == 1)
                    boundary = 1.;
                else
                    boundary = 0.;
                
                CONCAT_SINGLE_REF(target, boundary)
                            
                    
                if (to == 1)
                    boundary = 1.;
                else
                    boundary = 0.;
                
                CONCAT_SINGLE_REF(target, boundary)
                           
            }
            else if (pattern->subnode == 'r') {
                float boundary;


                if (from == sent->length)
                    boundary= 1;
                else
                    boundary = 0.;
                
                CONCAT_SINGLE_REF(target, boundary)

                if (to == sent->length)
                    boundary = 1.;
                else
                    boundary = 0.;
                
                CONCAT_SINGLE_REF(target, boundary)
                         
            }
        }
        else if (pattern->node == 'r') {
            // Parent is root or not.
            if (from == 0)
                CONCAT_SINGLE_VALUE(target, 1.)
            else
                CONCAT_SINGLE_VALUE(target, 0.)


        }
        else if (pattern->node == 'd') {
            if (from < to) {
                CONCAT_SINGLE_VALUE(target, 1.)
                CONCAT_SINGLE_VALUE(target, 0.)
            }
            else {
                CONCAT_SINGLE_VALUE(target, 0.)
                CONCAT_SINGLE_VALUE(target, 1.)
            }
        }
    }


    // Add the bias term
    CONCAT_SINGLE_VALUE(target,1.)

            /*
             * TODO: Primal solution requires those functions
             
    switch (etransform) {
        case LINEAR:
            return vlinear(target, bigvector);
            break;
        case QUADRATIC:
            return vquadratic(target, bigvector, 1);
            break;
        case CUBIC:
            return vcubic(target, bigvector, target->n);
            break;
    }
              */
    
   
    return eparseSucess;
    
    error:
    
    return eparseFailOthers;
}

float** square_adjacency_matrix(int n, float init_value) {

    float** matrix = (float**) malloc(sizeof (float*) * n);
    check_mem(matrix);
    for (int i = 0; i < n; i++) {
        matrix[i] = (float*) malloc(sizeof (float) * n);

        for (int j = 0; j < n; j++) {

            if (i == j)
                matrix[i][j] = init_value;
            else
                matrix[i][j] = 0.0;
        }

        check_mem(matrix[i]);
    }

    return matrix;
error:
    log_err("adjacency_matrix allocation error");
    exit(1);
}

//Feature matrix of a full sentence
Matrix_t F = NULL;

/**
 * Convert this into array of chunks for batch scoring of vectors
void get_embedding_matrix(CoNLLCorpus corpus, int sentence_idx) {
    FeaturedSentence sentence = (FeaturedSentence) DArray_get(corpus->sentences, sentence_idx);
    int length = sentence->length;

    EPARSE_CHECK_RETURN(newInitializedMatrix(&F, "sentence embedding matrix (F)", length * length, xformed_v->n, matrixInitNone, NULL, NULL))



            int offset = 0;
    for (int _from = 0; _from <= length; _from++) {
        for (int _to = 1; _to <= length; _to++) {
            if (_to != _from) {

                
             
                EPARSE_CHECK_RETURN(embedding_feature(sentence,_from,_to,xformed_embedding_v))

                memcpy(F->data + offset, xformed_v->data, sizeof (float) * xformed_v->n);

                offset += xformed_v->n;
            }
        }
    }
}
 */

void set_adj_matrix_mkl(CoNLLCorpus corpus, int sentence_idx, const Matrix_t y) {
    FeaturedSentence sentence = (FeaturedSentence) DArray_get(corpus->sentences, sentence_idx);
    int length = sentence->length;

    int offset = 0;

    for (int _from = 0; _from <= length; _from++) {
        for (int _to = 1; _to <= length; _to++)
            if (_to != _from)
                (sentence->adjacency_matrix)[_from][_to] = (y->data)[offset++];
    }

    check(offset == (length + 1) * length - length, "Matrix is not of the same size with the embeddings dimension x # of support vectors");

    return;
error:
    exit(1);
}

// Kernel Matrix x Feature Matrix
Matrix_t C = NULL;

// Polynomial N tranformation on C
Matrix_t Cpow = NULL;

// Arc scores
Matrix_t y = NULL;
Matrix_t yPow = NULL;

void setAdjacencyMatrix(CoNLLCorpus corpus, int sentence_idx, Perceptron_t kp, bool use_avg_alpha) {

    FeaturedSentence sentence = (FeaturedSentence) DArray_get(corpus->sentences, sentence_idx);
    int length = sentence->length;

    if (sentence->adjacency_matrix == NULL)
        sentence->adjacency_matrix = square_adjacency_matrix(length + 1, NEGATIVE_INFINITY);




    for (int _from = 0; _from <= length; _from++) {
        for (int _to = 1; _to <= length; _to++)
            if (_to != _from) {

                EPARSE_CHECK_RETURN(embedding_feature(sentence, _from, _to, xformed_embedding_v))

                EPARSE_CHECK_RETURN(score(kp, xformed_embedding_v, use_avg_alpha, &((sentence->adjacency_matrix)[_from][_to])))

            }

    }


}

void build_adjacency_matrix(CoNLLCorpus corpus, int sentence_idx, Vector_t embeddings_w, Vector_t discrete_w) {

    FeaturedSentence sentence = (FeaturedSentence) DArray_get(corpus->sentences, sentence_idx);
    int length = sentence->length;

    if (sentence->adjacency_matrix == NULL)
        sentence->adjacency_matrix = square_adjacency_matrix(length + 1, NEGATIVE_INFINITY);


    //sentence->feature_matrix = FeatureMatrix_create(length, corpus->hasembeddings, corpus->disrete_patterns_parts != NULL);

    for (int _from = 0; _from <= length; _from++)
        for (int _to = 1; _to <= length; _to++) {
            if (_to != _from) {
                (sentence->adjacency_matrix)[_from][_to] = 0.0;
                if (corpus->disrete_patterns_parts)
                    (sentence->adjacency_matrix)[_from][_to] = -1; // TODO: Complete discrete dot product.

                if (corpus->hasembeddings) {

                    //debug("%d->%d\n", _from, _to);

                    EPARSE_CHECK_RETURN(embedding_feature(sentence, _from, _to, xformed_embedding_v))


                    check(xformed_embedding_v != NULL, "Null transformed embedding vector");


                    float result = 0.0;
                    EPARSE_CHECK_RETURN(dot(embeddings_w, xformed_embedding_v, &result))

                            (sentence->adjacency_matrix)[_from][_to] += result;


                }

            }
        }

error:
    exit(EXIT_FAILURE);
}

Vector_t parse_vector(char *buff) {
    DArray *dim = split(buff, " ");
    
    Vector_t v= NULL;
    newInitializedCPUVector(&v, "embedding vector", DArray_count(dim),matrixInitNone,NULL,NULL)


    for (int i = 0; i < DArray_count(dim); i++) {
        (v->data)[i] = atof((char*) DArray_get(dim, i));
        free((char*) DArray_get(dim, i));
    }

    DArray_destroy(dim);

    return v;

}

Word parse_word(char* line, int embedding_dimension) {
    Word w = (Word) malloc(sizeof (struct Word));
    check_mem(w);

    w->conll_piece = split(line, "\t");

    w->id = atoi((char*) DArray_get(w->conll_piece, 0));
    //free((char*) DArray_get(tokens, 0));

    w->form = (char*) DArray_get(w->conll_piece, 1);
    w->postag = (char*) DArray_get(w->conll_piece, 3);

    w->parent = atoi((char*) DArray_get(w->conll_piece, 6));
    //free((char*) DArray_get(tokens, 6));

    if (embedding_dimension > 0) {
        check(DArray_count(w->conll_piece) >= 11, "CoNLL files in corpus with embedding should contain at least 11 fields. 11. field being the embedding field. Found a line with only %d fields", DArray_count(w->conll_piece));

        w->embedding = parse_vector((char*) DArray_get(w->conll_piece, 10));
        //free((char*) DArray_get(tokens, 10));

        check(embedding_dimension == w->embedding->n, "Expected embedding dimension was %d but got %ld", embedding_dimension, w->embedding->n);
    }
    else
        w->embedding = NULL;

    return w;

error:
    exit(1);
}

void Word_free(Word w) {
    deleteVector(w->embedding);

    DArray_clear_destroy(w->conll_piece);

    free(w);
}

void add_word(FeaturedSentence sentence, Word word) {

    DArray_push(sentence->words, word);

    sentence->length++;
}

FeaturedSentence FeatureSentence_create() {

    FeaturedSentence sent = (FeaturedSentence) malloc(sizeof (struct FeaturedSentence));
    check_mem(sent);

    sent->words = DArray_create(sizeof (Word), 10);
    check_mem(sent->words);

    sent->length = 0;
    sent->adjacency_matrix = NULL;

    return sent;

error:
    log_err("Sentence allocation error.");
    exit(1);
}



// TODO: Complete implementation

void free_FeaturedSentence(CoNLLCorpus corpus, int sentence_idx) {

    FeaturedSentence sentence = (FeaturedSentence) DArray_get(corpus->sentences, sentence_idx);

    for (int wi = 0; wi < DArray_count(sentence->words); wi++) {

        Word word = (Word) DArray_get(sentence->words, wi);

        Word_free(word);
    }

}

static DArray* find_corpus_files(const char *dir, DArray* sections) {
    struct dirent *entry;
    DIR *dp;

    DArray *array = DArray_create(sizeof (char*), 100);

    check(array != NULL, "Corpus file array creation failed.");

    char path[255];
    for (int i = 0; i < DArray_count(sections); i++) {
        int section = *((int*) DArray_get(sections, i));
        sprintf(path, "%s/%02d", dir, section);

        dp = opendir(path);
        check(dp != NULL, "Directory access error %s", path);

        while ((entry = readdir(dp))) {
            if (endswith(entry->d_name, ".dp")) {

                conll_file_t file = create_CoNLLFile(dir, section, entry->d_name);

                DArray_push(array, file);
            }
        }

        closedir(dp);
    }

    return array;

error:
    log_err("Terminating...");
    exit(1);
}

void read_corpus(CoNLLCorpus corpus, bool build_feat_matrix) {
    DArray* files = find_corpus_files(corpus->base_dir, corpus->sections);

    char *line = NULL;
    size_t len = 0;

    FeaturedSentence sent = FeatureSentence_create();

    for (int i = 0; i < DArray_count(files); i++) {
        ssize_t read;
        conll_file_t file = (conll_file_t) DArray_get(files, i);

        FILE *fp = fopen(file->fullpath, "r");
        check_mem(fp);

        while ((read = getline(&line, &len, fp)) != -1) {

            if (strcmp(line, "\n") != 0) {
                Word w = parse_word(line, corpus->word_embedding_dimension);

                add_word(sent, w);

            }
            else {
                sent->section = file->section;
                DArray_push(corpus->sentences, sent);

                //debug("One more sentence is added into corpus...");

                sent = FeatureSentence_create();
            }

        }

        fclose(fp);
    }

    free(line);

    // DArray_clear_destroy(files);

    log_info("Total of %d sentences", DArray_count(corpus->sentences));


    return;
error:
    log_err("Terminating...");
    exit(1);

}

