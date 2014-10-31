/* 
 * File:   parseutil.h
 * Author: husnusensoy
 *
 * Created on March 17, 2014, 7:43 PM
 */

#ifndef PARSEUTIL_H
#define	PARSEUTIL_H
#include "perceptron.h"
#include "dependency.h"
#include "corpus.h"

#include "util.h"
#include <stdbool.h>

#ifdef __GNUC__
#include <signal.h>
#include <sys/types.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

/**
 * @param max_numit Maximum number of iterations to go
 * @param max_rec Maximum number of records to be used for training
 * @param path ConLL base directory path
 * @param train_sections_str Training sections
 * @param dev_sections_str Development sections
 * @param embedding_dimension Embedding dimension per word
 * 
 * @return Model trained
 */
Perceptron_t optimize(int max_numit, int max_rec, const char* path, const char* train_sections_str, const char* dev_sections_str, int embedding_dimension);

/**
 * 
 * @param model Abstract perceptron model (Perceptron/KernelPerceptron)
 * @param path
 * @param test_sections_str
 * @param embedding_dimension
 */
void parseall(Perceptron_t model, const char* path, const char* test_sections_str, int embedding_dimension);

#endif	/* PARSEUTIL_H */

