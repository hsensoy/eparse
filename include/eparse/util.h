//
//  util.h
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 03/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//

#ifndef Perceptron_GLM_NLP_Tasks_util_h
#define Perceptron_GLM_NLP_Tasks_util_h

#include <stdint.h>
#include <stdio.h>

#include <assert.h>
#include <stdlib.h>
#include "darray.h"

#include "stringalgo.h"

#define PARSER_RATE_VERBOSITY 250       // Log parser rate in every PARSER_RATE_VERBOSITY sentences.

/**
 * @brief Returns a DArray containing integer sequence [start, end )
 * @param start
 * @param end
 * @return DArray containing all integers [start, end )
 */
DArray* range(int start, int end);
DArray* parse_range(const char *rangestr);

void print_range(const char *promt, DArray* range);
char* join_range(DArray *range);


struct Rate {
    double t_begin;
    double t_end;

    double total_elapsed;

    int count;
};

typedef struct Rate* Rate;

Rate start(Rate *r) ;
void stop(Rate *r);

#endif
