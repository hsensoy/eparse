//
//  corpus.c
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 13/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//
#include "corpus.h"

long max_num_sv = 0;
long max_narc = 0;

static Vector_t embedding_v = NULL;
static Vector_t xformed_embedding_v = NULL;
static float zero = 0.f;


/**
 * ai-parse.c file for actual storage allocation for those two variables
 */
extern const char *epattern;
extern FeatureTransformer_t ft;
extern enum FeatureTransform etransform;
extern FeatureTemplate_t feattemp;

extern float rbf_lambda;


/**
 * 
 * @param base_dir CoNLL base directory including sections
 * @param sections DArray of sections
 * @return CoNLLCorpus structure
 */
CoNLLCorpus create_CoNLLCorpus(const char *base_dir, DArray *sections) {
    CoNLLCorpus corpus = (CoNLLCorpus) malloc(sizeof(struct CoNLLCorpus));

    check_mem(corpus);

    corpus->base_dir = base_dir;
    corpus->sections = sections;

    corpus->sentences = DArray_create(sizeof(FeaturedSentence), 2000);
    check_mem(corpus->sentences);

    /*
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
     */

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
}


float **square_adjacency_matrix(int n, float init_value) {

    float **matrix = (float **) malloc(sizeof(float *) * n);
    check_mem(matrix);
    for (int i = 0; i < n; i++) {
        matrix[i] = (float *) malloc(sizeof(float) * n);

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
//Matrix_t F = NULL;


// Kernel Matrix x Feature Matrix
//Matrix_t C = NULL;

// Polynomial N tranformation on C
//Matrix_t Cpow = NULL;

// Arc scores
//Matrix_t y = NULL;
//Matrix_t yPow = NULL;

void setAdjacencyMatrix(CoNLLCorpus corpus, int sentence_idx, Perceptron_t kp, bool use_avg_alpha) {
    static Vector_t v = NULL;
    FeaturedSentence sentence = (FeaturedSentence) DArray_get(corpus->sentences, sentence_idx);
    int length = sentence->length;

    if (sentence->adjacency_matrix == NULL)
        sentence->adjacency_matrix = square_adjacency_matrix(length + 1, NEGATIVE_INFINITY);

#ifdef    BATCH_SCORE
    Matrix_t all = NULL;
    Vector_t vscore = NULL;

    for (int _from = 0; _from <= length; _from++) {
        for (int _to = 1; _to <= length; _to++)
            if (_to != _from) {

                FEATURETEMPLATE_CHECK_RETURN(arc_feature_vector(feattemp, sentence, _from, _to, &v))

                EPARSE_CHECK_RETURN(hstack(&all, memoryCPU, "all embeddings", v, false, false))
            }
    }

    debug("hstack is done");

    if (ft != NULL) {


#ifdef OPTIMIZED_TRANSFORMATION
                EPARSE_CHECK_RETURN(scoreBatch(kp, all, use_avg_alpha, &(vscore)))
            #else
        Matrix_t all_nl = NULL;
        debug("Transforming batch...");
        EPARSE_CHECK_RETURN(transformBatch(ft, all, &all_nl))
        debug("Transformed batch...");

        EPARSE_CHECK_RETURN(scoreBatch(kp, all_nl, use_avg_alpha, &(vscore)))

        deleteMatrix(all_nl);
#endif


    }
    else {
        EPARSE_CHECK_RETURN(scoreBatch(kp, all, use_avg_alpha, &(vscore)))
    }

    long idx = 0;
    for (int _from = 0; _from <= length; _from++) {
        for (int _to = 1; _to <= length; _to++)
            if (_to != _from) {
                debug("Score[%d,%d]=%f", _from, _to, (vscore->data)[idx++]);
                (sentence->adjacency_matrix)[_from][_to] = (vscore->data)[idx++];
            }
    }

    deleteMatrix(all);
    deleteVector(vscore);
#else
        for (int _from = 0; _from <= length; _from++) {
            for (int _to = 1; _to <= length; _to++)
                if (_to != _from) {
                    FEATURETEMPLATE_CHECK_RETURN(arc_feature_vector(feattemp,sentence,_from,_to,&v))
    	            EPARSE_CHECK_RETURN(score(kp, v, use_avg_alpha, &((sentence->adjacency_matrix)[_from][_to])))
                }
        }
    #endif


}

featureTemplateError_t checkFeatureTemplate(const Word w, const FeatureTemplate_t ft) {

    //todo: complete implementation

    return featureTemplateSucess;


}

Vector_t parse_vector(char *buff) {
    DArray *dim = split(buff, " ");

    Vector_t v = NULL;
    newInitializedCPUVector(&v, "embedding vector", DArray_count(dim), matrixInitNone, NULL, NULL)


    for (int i = 0; i < DArray_count(dim); i++) {
        (v->data)[i] = atof((char *) DArray_get(dim, i));
        free((char *) DArray_get(dim, i));
    }


    DArray_destroy(dim);

    return v;

}

Word parse_word(char *line) {
    Word w = (Word) malloc(sizeof(struct Word));
    check_mem(w);

    w->conll_piece = split(line, "\t");

    w->id = atoi((char *) DArray_get(w->conll_piece, 0));
    //free((char*) DArray_get(tokens, 0));

    w->form = (char *) DArray_get(w->conll_piece, 1);
    w->postag = (char *) DArray_get(w->conll_piece, 3);

    w->parent = atoi((char *) DArray_get(w->conll_piece, 6));
    //free((char*) DArray_get(tokens, 6));

    if (DArray_count(w->conll_piece) >= 11)
        w->embedding = parse_vector((char *) DArray_get(w->conll_piece, CONLL_EMBEDDING_INDEX));
    else
        w->embedding = NULL;

    FEATURETEMPLATE_CHECK_RETURN(checkFeatureTemplate(w, feattemp));

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

    FeaturedSentence sent = (FeaturedSentence) malloc(sizeof(struct FeaturedSentence));
    check_mem(sent);

    sent->words = DArray_create(sizeof(Word), 10);
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

static DArray *find_corpus_files(const char *dir, DArray *sections) {
    struct dirent *entry;
    DIR *dp;

    DArray *array = DArray_create(sizeof(char *), 100);

    check(array != NULL, "Corpus file array creation failed.");

    char path[255];
    for (int i = 0; i < DArray_count(sections); i++) {
        int section = *((int *) DArray_get(sections, i));
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

void read_corpus(CoNLLCorpus corpus, int max_sent, bool build_feat_matrix) {
    DArray *files = find_corpus_files(corpus->base_dir, corpus->sections);

    char *line = NULL;
    size_t len = 0;

    int sentCount = 0;

    bool done = false;

    FeaturedSentence sent = FeatureSentence_create();


    for (int i = 0; i < DArray_count(files) && !done; i++) {
        ssize_t read;
        conll_file_t file = (conll_file_t) DArray_get(files, i);

        FILE *fp = fopen(file->fullpath, "r");
        check(fp != NULL, "%s could not opened", file->fullpath);

        while ((read = getline(&line, &len, fp)) != -1) {

            if (strcmp(line, "\n") != 0) {
                Word w = parse_word(line);

                add_word(sent, w);

            }
            else {
                sent->section = file->section;
                DArray_push(corpus->sentences, sent);

                if (max_sent > 0)
                    sentCount++;
                if (sentCount == max_sent) {
                    done = true;
                    break;
                }

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

