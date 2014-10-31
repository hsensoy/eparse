//
//  stringalgo.h
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 03/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//

#ifndef Perceptron_GLM_NLP_Tasks_stringalgo_h
#define Perceptron_GLM_NLP_Tasks_stringalgo_h
	#include <string.h>
	#include <stdarg.h>
        #include <alloca.h>
	#include "darray.h"

	int endswith(const char* str, const char* suffix);
	DArray* split( const char* str, const char *delims);


	/**
	 * @brief Python join function for C.
	 *  Join n strings into buffer using single space character as the delimiter. Function is not safe and cause overflow in buffer array.
	 * @param buffer Sufficiently large array in which will be stored.
	 * @param n Number of strings to be joined.
	 */
	void join(char buffer[], int n, ...);
#endif
