/* 
 * File:   conll.h
 * Author: husnusensoy
 *
 * Created on May 3, 2014, 9:20 PM
 */

#ifndef CONLL_H
#define	CONLL_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include "debug.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct conll_file_t{
    char* section_dir;
    char* file;
    char* fullpath;
    int section;
};

typedef struct conll_file_t* conll_file_t;

/**
 *
 * @param conll_basedir CoNLL base directory
 * @param conll_section CoNLL section
 * @param conll_file CoNLL file
 * @return Data structure representing CoNLL file.
 */
conll_file_t create_CoNLLFile(const char* conll_basedir, int conll_section, const char* conll_file );

#endif	/* CONLL_H */

