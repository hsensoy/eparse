//
// Created by husnu sensoy on 04/05/15.
//
#include <stdlib.h>
#include "feattemplate.h"

#define array_length(arr) (sizeof ( (arr) ) / sizeof (float))

const char *epattern = NULL;
enum FeatureTransform etransform;

enum PerceptronType type = KERNEL_PERCEPTRON;
enum KernelType kernel = POLYNOMIAL_KERNEL;

int num_parallel_mkl_slaves = -1;
const char *modelname = NULL;
enum BudgetMethod budget_method;
int budget_target = 50000;
int polynomial_degree = 4;
float bias = 1.0;
float rbf_lambda = 0.025;
int edimension = 0;

int verbosity = 0;

int num_rand_sv = 300000;

FeatureTransformer_t ft = NULL;

FeatureTemplate_t feattemp;

const char *mysentence = "1	Ms.	Ms.	NNP	NNP	_	2	NMOD	_	_	0.026682 -0.330119\n\
2	Haag	Haag	NNP	NNP	_	3	SUB	_	_	0.199891 -0.29262\n\
3	plays	plays	VBZ	VBZ	_	0	ROOT	_	_	0.124389 0.052897\n\
4	Elianti	Elianti	NNP	NNP	_	3	OBJ	_	_	-0.125 -0.353247\n\
5	.	.	.	.	_	3	P	_	_	-0.125 -0.25";


int main(int argc, char **argv) {
    FeaturedSentence sent = FeatureSentence_create();
    sent->section = 2;

    DArray *words = split(mysentence, "\n");

    for (int i = 0; i < DArray_count(words); ++i) {
        char *line = (char *) DArray_get(words, i);

        if (strcmp(line, "\n") != 0) {
            Word w = parse_word(line);
            add_word(sent, w);
            //printMatrix("Embedding", w->embedding, stdout);
        }
    }


    const char *template[] = {"p-1postag_p0postag_p1postag_c-1postag_c0v_c1postag_betweenpostag",
                              "p-1postag_p0postag_p1postag_c-1postag_c0postag_c1postag_tl",
                              "p-1postag_p0postag_p1postag_c-1postag_c0postag_c1postag_tl_lbf_rbf_root_dir_betweenv"};

    Vector_t v = NULL;
    for (int j = 0; j < 1; ++j) {

        feattemp = createFeatureTemplate(template[j], 15);

        debug("%s feature template is creeated", template[j]);

        //0->1 check
        arc_feature_vector(feattemp, sent, 0, 1, &v);


        //printMatrixVerbose("===0->1===",v,stdout,500,1);

        //1->4 check
        arc_feature_vector(feattemp, sent, 1, 4, &v);


        //printMatrixVerbose("===1->4===",v,stdout,500,1);

        //1->5 check
        arc_feature_vector(feattemp, sent, 1, 5, &v);


         printMatrixVerbose("===1->5===",v,stdout,500,1);
    }

    printFeatureTemplate(feattemp);









    //log_info("Feature vector dimension is %ld", v->n);

    //printMatrixVerbose("Feature Vect", v, stdout, 52, 1);



    return EXIT_SUCCESS;
    error:
    return EXIT_FAILURE;
}
