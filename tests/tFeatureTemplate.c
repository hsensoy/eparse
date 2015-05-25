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


    const char *template[] = {"p-1v_p0v_p1v_c-1v_c0v_c1v", "p-1v_p0v_p1v_c-1v_c0v_c1v_tl",
                              "p-1v_p0v_p1v_c-1v_c0v_c1v_tl_lbf_rbf_root_dir_betweenv",
                              "p0v:0-1_c0v:0-1_p-1v:1-2_p0v:1-2_p1v:1-2_c-1v:1-2_c0v:1-2_c1v:1-2_tl_lbf_rbf_root_dir_betweenv:1-2",
                              "p-1postag_p0postag_p1postag_c-1postag_c0v_c1postag",
                              "p-1postag_p0postag_p1postag_c-1postag_c0postag_c1postag_tl",
                              "p-1postag_p0postag_p1postag_c-1postag_c0postag_c1postag_tl_lbf_rbf_root_dir_betweenv"};

    const char *vexpected01[] = {
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 1.000000",
            "0.000000 0.000000 "                //p-1v
                    "0.000000 0.000000 "        //p0v
                    "0.000000 0.000000 "        //p1v
                    "0.000000 0.000000 "        //c-1v
                    "0.026682 -0.330119 "       //c0v
                    "0.199891 -0.292620 "       //c1v
                    "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 " //tl
                    "0.000000 1.000000 "    //lbf
                    "0.000000 0.000000 "    //rbf
                    "1.000000 "             //root
                    "1.000000 0.000000 "    //dir
                    "0.000000 0.000000 "    //betweenv
                    "1.000000",
            "0.000000 0.026682 0.000000 0.000000 0.000000 0.000000 -0.330119 -0.29262 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 1.000000 0.000000 0.000000 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000"};

    const char *vexpected14[] = {
            "0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 0.124389 0.052897 -0.125 -0.353247 -0.125 -0.25 1.000000",
            "0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 0.124389 0.052897 -0.125 -0.353247 -0.125 -0.25 1.000000 1.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 1.000000",
            "0.000000 0.000000 "
                    "0.026682 -0.330119 "
                    "0.199891 -0.292620 "
                    "0.124389 0.052897 "
                    "-0.125 -0.353247 "
                    "-0.125 -0.25 "
                    "1.000000 1.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 "
                    "1.000000 0.000000 "
                    "0.000000 0.000000 "
                    "0.000000 "
                    "1.000000 0.000000 "
                    "0.16214 -0.1198615 "
                    "1.000000",
            "0.026682 -0.125 0.000000 -0.330119  -0.292620 0.052897 -0.353247 -0.25 1.000000 1.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.1198615 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000"};

    const char *vexpected15[] = {
            "0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 -0.125 -0.353247 -0.125 -0.25 0.000000 0.000000 1.000000",
            "0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 -0.125 -0.353247 -0.125 -0.25 0.000000 0.000000 1.000000 1.000000 1.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 1.000000",
            "0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 -0.125 -0.353247 -0.125 -0.25 0.000000 0.000000 1.000000 1.000000 1.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 "
                    "1.000000 0.000000 "
                    "0.000000 1.000000 "
                    "0.000000 "
                    "1.000000 0.000000 "
                    "0.066427 -0.1198615 "
                    "1.000000",
            "0.026682 -0.125 0.000000 -0.330119  -0.292620 0.052897 -0.353247 -0.25 1.000000 1.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.1198615 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000",
            "0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.026682 -0.330119 0.199891 -0.292620 1.000000"};

    Vector_t v = NULL;
    for (int j = 0; j < 4; ++j) {

        feattemp = createFeatureTemplate(template[j], 0);

        debug("%s feature template is creeated", template[j]);

        //0->1 check
        arc_feature_vector(feattemp, sent, 0, 1, &v);


        Vector_t v0 = parse_vector(vexpected01[j]);


        check(vequal(v0, v), "Expected vector does not match for %s", template[j]);

        log_info("VERIFIED: %s for %d->%d", template[j], 0, 1);

        //1->4 check
        arc_feature_vector(feattemp, sent, 1, 4, &v);


        v0 = parse_vector(vexpected14[j]);


        check(vequal(v0, v), "Expected vector does not match for %s", template[j]);

        log_info("VERIFIED: %s for %d->%d", template[j], 1, 4);

        //1->5 check
        arc_feature_vector(feattemp, sent, 1, 5, &v);


        v0 = parse_vector(vexpected15[j]);


        check(vequal(v0, v), "Expected vector does not match for %s", template[j]);

        log_info("VERIFIED: %s for %d->%d", template[j], 1, 5);

        //deleteVector(v0);
    }









    //log_info("Feature vector dimension is %ld", v->n);

    //printMatrixVerbose("Feature Vect", v, stdout, 52, 1);



    return EXIT_SUCCESS;
    error:
    return EXIT_FAILURE;
}
