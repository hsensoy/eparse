//
//  stringalgo.c
//  Perceptron GLM NLP Tasks
//
//  Created by husnu sensoy on 03/01/14.
//  Copyright (c) 2014 husnu sensoy. All rights reserved.
//

#include "stringalgo.h"


int endswith(const char* str, const char* suffix) {

    if (strlen(suffix) <= strlen(str)) {

        unsigned long n = strlen(str) - strlen(suffix);
        for (int i = 0; i < strlen(suffix); i++) {

            //            printf("%c %c\n", suffix[i], str[n+i]);

            if (suffix[i] != str[n + i])
                return 0;
        }

        return 1;

    } else {
        return 0;
    }
}

// TODO: strtok does not release any memory ?

DArray* split(const char* str, const char *delims) {
    DArray *tokens = DArray_create(sizeof (char*), 3);
    check_mem(tokens);

    char *strclone = (char *) alloca(strlen(str) + 1);

    strcpy(strclone, str);
    //char *strclone = strdup(str);

    char *pch = strtok(strclone, delims);
    while (pch != NULL) {
        DArray_push(tokens, strdup(pch));
        pch = strtok(NULL, delims);
    }

    return tokens;
error:
    exit(1);
}

void join(char buffer[], int n, ...) {
    va_list ap;

    buffer[0] = '\0';
    va_start(ap, n);
    for (int j = 1; j <= n; j++) {
        strcat(buffer, (va_arg(ap, char*)));

        if (j < n)
            strcat(buffer, " ");
    }

    va_end(ap);
    //    log_info("%s",result);
}
