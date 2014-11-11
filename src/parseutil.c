
#include "parseutil.h"


static bool keepRunning = true;

void intHandler(int dummy) {
    keepRunning = false;
}

#define STOP_ON_CONVERGE true
#define MAX_IDLE_ITER 3
#define MIN_DELTA 0.001

/**
 * ai-parse.c file for actual storage allocation for those two variables
 */
extern const char *epattern;
extern enum EmbeddingTranformation etransform;
extern enum KernelType kernel;
extern enum PerceptronType type;
extern const char *modelname;
extern int polynomial_degree;
extern float bias;
extern float rbf_lambda;
extern int num_rand_sv;

void dump_conll_word(Word w, bool true_parent, FILE* ofp) {

    for (int i = 0; i < 10; i++) {

        if (i != 6)
            fprintf(ofp, "%s", (char*) DArray_get(w->conll_piece, i));
        else {
            if (true_parent)
                fprintf(ofp, "%d", w->parent);
            else
                fprintf(ofp, "%d", w->predicted_parent);
        }

        if (i < 9)
            fprintf(ofp, "\t");
    }
    fprintf(ofp, "\n");

}

ParserTestMetric testPerceptron (Perceptron_t mdl, const CoNLLCorpus corpus, bool use_avg, FILE *gold_ofp, FILE *model_ofp) {
    ParserTestMetric metric = create_ParserTestMetric();

   
    int model[300];

    Progress_t ptested = NULL;
	
//	signal(SIGINT, intHandler);


    CHECK_RETURN(newProgress(&ptested, "test sentences", DArray_count(corpus->sentences), 0.3))

    for (int si = 0; si < DArray_count(corpus->sentences) && keepRunning ; si++) {
        FeaturedSentence sent = DArray_get(corpus->sentences, si);

        debug("Test sentence %d (section %d) of length %d", si, sent->section, sent->length);

        debug("Generating adj. matrix for sentence %d", si);
        setAdjacencyMatrix(corpus, si,mdl,use_avg);


        debug("Now parsing sentence %d", si);
        parse(sent, model);

        (metric->total_sentence)++;
        debug("Now comparing actual arcs with model generated arcs for sentence %d (Last sentence is %d)", si, sent->length);
		int pmatch_nopunc = 0, ptotal_nopunc = 0, pmatch = 0;
        for (int j = 0; j < sent->length  ; j++) {
            Word w = (Word) DArray_get(sent->words, j);

            w->predicted_parent = model[j + 1];

            // TODO: One file per section idea
            if (model_ofp != NULL)
                dump_conll_word(w, true, model_ofp);

            if (gold_ofp != NULL)
                dump_conll_word(w, false, gold_ofp);

            if (w->parent == 0 && model[j + 1] == 0)
                (metric->true_root_predicted)++;

            debug("\tTrue parent of word %d (with %s:%s) is %d whereas estimated parent is %d", j + 1, w->postag, w->form, w->parent, model[j + 1]);

            if (strcmp(w->postag, ",") != 0 && strcmp(w->postag, ":") != 0 && strcmp(w->postag, ".") != 0 && strcmp(w->postag, "``") != 0 && strcmp(w->postag, "''") != 0) {

                if (w->parent == model[j + 1]) {
                    (metric->without_punc->true_prediction)++;
                    pmatch_nopunc++;
                }

                ptotal_nopunc++;

                (metric->without_punc->total_prediction)++;
            }

     
            (metric->all->total_prediction)++;

            if (w->parent == model[j + 1]) {
                pmatch++;
                (metric->all->true_prediction)++;
            }
        }
		
        if (pmatch_nopunc == ptotal_nopunc && pmatch_nopunc != 0) {
            (metric->complete_sentence_without_punc)++;
        }
		
        if (pmatch == sent->length) {
            (metric->complete_sentence)++;
        }
		
        if (model_ofp != NULL) {
            fprintf(model_ofp, "\n");
        }

        if (gold_ofp != NULL) {
            fprintf(gold_ofp, "\n");
        }

        //free(model);

        tickProgress(ptested);
        debug("Releasing feature matrix for sentence %d", si);

        //free_feature_matrix(corpus, si);
    }

    deleteProgress(ptested);

    return metric;
}



Perceptron_t optimize(int max_numit, int max_rec, const char* path, const char* train_sections_str, const char* dev_sections_str, int embedding_dimension) {
    DArray *train_sections = parse_range(train_sections_str);
    DArray *dev_sections = parse_range(dev_sections_str);

    signal(SIGINT, intHandler);

    log_info("Development sections to be used in %s: %s", path, join_range(dev_sections));

    CoNLLCorpus dev = create_CoNLLCorpus(path, dev_sections, embedding_dimension, NULL);

    log_info("Training sections to be used in %s: %s", path, join_range(train_sections));

    CoNLLCorpus train = create_CoNLLCorpus(path, train_sections, embedding_dimension, NULL);

    log_info("Reading training corpus");
    read_corpus(train, max_rec, false);

    log_info("Reading dev corpus");
    read_corpus(dev, -1, false);

    float *numit_dev_avg = (float*) malloc(sizeof (float)* max_numit);
    float *numit_train_avg = (float*) malloc(sizeof (float)*max_numit);
	long *numit_num_sv = (long*) malloc(sizeof (long)*max_numit);

    check(numit_dev_avg != NULL, "Memory allocation failed for numit_dev_avg");
    check(numit_train_avg != NULL, "Memory allocation failed for numit_train_avg");
	check(numit_num_sv != NULL, "Memory allocation failed for numit_num_sv");

    Perceptron_t model = NULL;
    
    if (type == SIMPLE_PERCEPTRON) {
        log_info("Creating a averaged perceptron model");
        model = newSimplePerceptron();
        //model = create_PerceptronModel(train->transformed_embedding_length, NULL);
    }
    else {
        if (kernel == POLYNOMIAL_KERNEL)
            model = newPolynomialKernelPerceptron(polynomial_degree, bias);
            //kmodel = newPolynomialKernelPerceptron(polynomial_degree, bias);
        else
            model = NULL;
            //kmodel = create_RBFKernelPerceptron(rbf_lambda);
    }


    int numit;

    int best_iter = -1;
    float best_score = 0.0;

    for (numit = 1; numit <= max_numit && keepRunning; numit++) {
        log_info("BEGIN-TRAIN: Iteration %d", numit);

        trainPerceptronOnce(model, train, max_rec);

        log_info("END-TRAIN: Iteration %d", numit);

        ParserTestMetric dev_metric;
        log_info("BEGIN-TEST: Iteration %d", numit);

        dev_metric = testPerceptron(model, dev, true, NULL, NULL);
        
        log_info("END-TEST: Iteration %d", numit);

        log_info("\nnumit=%d", numit);

        printParserTestMetric(dev_metric);

        double dev_acc = (dev_metric->without_punc->true_prediction * 1.) / dev_metric->without_punc->total_prediction;
        numit_dev_avg[numit - 1] = dev_acc;
        numit_train_avg[numit - 1] = 0.0;
		
		if ( model->type == KERNEL_PERCEPTRON )
			numit_num_sv[ numit - 1 ] = ((KernelPerceptron_t)model->pDeriveObj)->kernel->matrix->ncol;	

        freeParserTestMetric(dev_metric);

        if (best_score < dev_acc) {
            if (best_score + MIN_DELTA > dev_acc)
                log_warn("Improvement is less than %f", MIN_DELTA);

            best_score = dev_acc;
            best_iter = numit;

            //mark_best_PerceptronModel(model, numit);
            
            EPARSE_CHECK_RETURN(snapshotBest(model));
        }

        if (numit - best_iter > MAX_IDLE_ITER && STOP_ON_CONVERGE) {
            log_info("No improvement in last %d iterations", MAX_IDLE_ITER);
            keepRunning = false;
        }
    }

    log_info("Iteration\tAccuracy(dev)\tAccuracy(train)\t# of SV");
    for (int i = 0; i < numit - 1; i++) {
        log_info("%d\t\t%f\t%f\t%ld%s", i + 1, numit_dev_avg[i], numit_train_avg[i], numit_num_sv[i], (i + 1 == best_iter) ? " (*)" : "");
    }

    //free_CoNLLCorpus(dev, true);
    //free_CoNLLCorpus(train, true);

    return model;

error:
    log_err("Memory allocation error");

    exit(1);

}


void parseall(Perceptron_t model, const char* path, const char* test_sections_str, int embedding_dimension) {
    DArray *test_sections = parse_range(test_sections_str);

    signal(SIGINT, intHandler);

    log_info("Test sections to be used in %s: %s", path, join_range(test_sections));

    CoNLLCorpus test = create_CoNLLCorpus(path, test_sections, embedding_dimension, NULL);

    log_info("Reading test corpus");
    read_corpus(test, -1,false);

    char* output_filename = (char*) malloc(sizeof (char) * (strlen(modelname) + 13));
    check_mem(output_filename);

    sprintf(output_filename, "%s.gold.conll", modelname);
    FILE *gold_fp = fopen(output_filename, "w");

    sprintf(output_filename, "%s.model.conll", modelname);
    FILE *model_fp = fopen(output_filename, "w");
    ParserTestMetric test_metric = testPerceptron( model, test, true, gold_fp, model_fp);
    fclose(gold_fp);
    fclose(model_fp);

    printParserTestMetric(test_metric);
    freeParserTestMetric(test_metric);

    free(output_filename);

    return;
error:
    log_err("Memory allocation error");

    exit(1);


}
