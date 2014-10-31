//
//  util.c
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 03/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//

#include "util.h"

DArray* range(int start, int end){
    DArray *result = DArray_create(sizeof(int), end - start);
    check_mem(result);
    
    for (int v=start; v < end; v++) {
        int* i = (int*)malloc(sizeof(int));
        check_mem(i);
        
        *i = v;
        
        DArray_push(result, i);
    }
    
    return result;
error:
    exit(1);
}

DArray* parse_range(const char *rangestr){
    DArray *result = DArray_create(sizeof(int), 1);
    check_mem(result);
    
    if (strchr(rangestr, '-') != NULL){
        DArray *tokens = split(rangestr, "-");
        
        check(DArray_count(tokens) == 2, "Invalid range string %s %d", rangestr, DArray_count(tokens) == 2);
        
        int start = atoi((char*)DArray_get(tokens, 0));
        int end = atoi((char*)DArray_get(tokens, 1));
        
        DArray_clear_destroy(tokens);
        
        return range(start, end);
        
    }else if (strchr(rangestr, ',') != NULL){
        DArray *tokens = split(rangestr, ",");
        
        for(int i = 0;i < DArray_count(tokens);i++){
            int *iptr = (int*)malloc(sizeof(int));
            check_mem(iptr);
            
            *iptr = atoi((char*)DArray_get(tokens, i));
            
            DArray_push(result, iptr);
        }
        
        DArray_clear_destroy(tokens);
    }else{
        int *iptr = (int*)malloc(sizeof(int));
        check_mem(iptr);
        
        *iptr = atoi(rangestr);
        
        DArray_push(result, iptr);
    }
    
    
    return result;
error:
    exit(1);
}

char* join_range(DArray *range){
    char* buffer = (char*)malloc(sizeof(char) * 1024);
    check_mem(buffer);
    
    buffer[0] = '\0';
    
    for (int i = 0 ; i < DArray_count(range); i++) {
        
        if (i == 0 ){
            sprintf(buffer, "%d", *((int*)DArray_get(range, i)));
        }else{
            sprintf(buffer, "%s, %d", buffer, *((int*)DArray_get(range, i)));
        }
    }
    
    return buffer;
error:
    exit(1);
}


Rate start(Rate *r) {

    if (*r == NULL) {
        *r = (Rate) malloc(sizeof (struct Rate));
        (*r)->count = 0;
        (*r)->total_elapsed = 0.0;
        check(r != NULL, "Memory allocation error");
    }

    (*r)->t_begin = 0.;//TODO: dsecnd();
    (*r)->t_end = -1;

    return *r;

error:
    exit(1);

}

void stop(Rate *r) {

    (*r)->t_end = 1.;//dsecnd();
    (*r)->total_elapsed += ((*r)->t_end - (*r)->t_begin);
    ((*r)->count)++;

    if (((*r)->count) % PARSER_RATE_VERBOSITY == 0 && ((*r)->count) > 0) {
        log_info("Parser Rate is %lf sentences/sec", ((*r)->count) / ((*r)->total_elapsed));
    }
}



 
 
 
 



