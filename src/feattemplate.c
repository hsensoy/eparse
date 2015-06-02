//
// Created by husnu sensoy on 02/05/15.
//
#include "feattemplate.h"


static const char *ROOT = "*";
static const char *b_none = "-";


static featureTemplateError_t init_ft_arrays(FeatureTemplate_t *ft, int n, uint32_t n_disc_feature) {
    *ft = (FeatureTemplate_t) malloc(sizeof(struct FeatureTemplate_st));

    if (*ft == NULL)
        return featureTemplateMemoryError;

    (*ft)->nfeatures = n;
    (*ft)->offset = (int *) malloc(sizeof(int) * n);
    (*ft)->type = (enum FeatureType *) malloc(sizeof(enum FeatureType) * n);


    (*ft)->start = (long *) malloc(sizeof(long) * n);
    (*ft)->end = (long *) malloc(sizeof(long) * n);
    (*ft)->node = (char *) malloc(sizeof(char) * n);

    (*ft)->avg_v = NULL;

    (*ft)->ndisc_feature = (n_disc_feature > 0) ? n_disc_feature : DEFAULT_N_DISC_FEATURE;
    (*ft)->disc = NULL;


    if ((*ft)->offset == NULL || (*ft)->type == NULL || (*ft)->start == NULL || (*ft)->end == NULL ||
        (*ft)->node == NULL)
        return featureTemplateMemoryError;


    return featureTemplateSucess;
}

static featureTemplateError_t add_template_piece(const char *pattern, int index, FeatureTemplate_t ft) {
    int offset;
    char p_or_c;
    int start = -1;
    int end = -1;

    (ft->node)[index] = NODE_NONE;

    if (strcmp(pattern, "tl") == 0) { //thresholded-length
        (ft->type)[index] = FT_THRESHOLDED_DISTANCE;
    }
    else if (strcmp(pattern, "nl") == 0) { // normalized-length
        (ft->type)[index] = FT_NORMALIZED_DISTANCE;
    }
    else if (strcmp(pattern, "l") == 0) { // raw length
        (ft->type)[index] = FT_RAW_DISTANCE;
    }
    else if (strcmp(pattern, "lbf") == 0) { // Left Boundary Flag
        (ft->type)[index] = FT_LEFT_BOUNDARY_FLAG;
    }
    else if (strcmp(pattern, "rbf") == 0) { // Right Boundary Flag
        (ft->type)[index] = FT_RIGHT_BOUNDARY_FLAG;
    }
    else if (strcmp(pattern, "dir") == 0) { // Direction
        (ft->type)[index] = FT_DIRECTION;
    }
    else if (strcmp(pattern, "root") == 0) { // Root Flag
        (ft->type)[index] = FT_ROOT;
    }
    else if (strcmp(pattern, "betweenpostag") == 0) {   // Between PosTag
        (ft->type)[index] = FT_BETWEEN_POSTAG;


    }
    else if (strstr(pattern, "betweenv") != NULL) { // Between embedding
        /**
         * todo betweenv:0-50 Average embeddings of words between parent and child (first 50 elements)
         * todo betweenv  Average embeddings of words between parent and child
         */
        (ft->type)[index] = FT_BETWEEN_EMBEDDING;

        if (strstr(pattern, ":") != NULL) {

            int n = sscanf(pattern, "betweenv:%d-%d", &start, &end);

            check(n == 2, "Expected pattern format is betweenv:<begin>-<end> where as got %s", pattern);
            check(start < end, "v:%d-%d is not a valid vector substring", start, end);
        }

    }
    else {

        /**
         * p0postag PartOfSpeech Tag of parent node
         * c0postag PartOfSpeech Tag of child node
         * p0v  Embedding vector of parent node.
         * p0e  Embedding vector of parent node.
         * p0v:0-50 Embedding vector of parent node (first 50 elements)
         * p0e:0-50 Embedding vector of parent node (first 50 elements)
         */
        if (strstr(pattern, "postag") != NULL) {
            int n = sscanf(pattern, "%c%dpostag", &p_or_c, &offset);

            check(n == 2, "Expected pattern format is [p|c]<offset>postag where as got %s", pattern);
            check(p_or_c == 'p' || p_or_c == 'c', "Unknown node name %c expected p or c", p_or_c);

            (ft->type)[index] = FT_POSTAG;


        } else if (strstr(pattern, ":") != NULL) {
            int n = sscanf(pattern, "%c%dv:%d-%d", &p_or_c, &offset, &start, &end);

            check(n == 4, "Expected pattern format is [p|c]<offset>v:<begin>-<end> where as got %s", pattern);
            check(p_or_c == 'p' || p_or_c == 'c', "Unknown node name %c expected p or c", p_or_c);
            check(start < end, "v:%d-%d is not a valid vector substring", start, end);

            (ft->type)[index] = FT_EMBEDDING;
        } else {
            int n = sscanf(pattern, "%c%dv", &p_or_c, &offset);

            check(n == 2, "Expected pattern format is [p|c]<offset>v where as got %s", pattern);
            check(p_or_c == 'p' || p_or_c == 'c', "Unknown node name %c expected p or c", p_or_c);

            (ft->type)[index] = FT_EMBEDDING;
        }

        (ft->node)[index] = (p_or_c == 'p') ? NODE_PARENT : NODE_CHILD;
    }

    (ft->offset)[index] = offset;
    (ft->start)[index] = start;
    (ft->end)[index] = end;

    return featureTemplateSucess;

    error:
    return featureTemplateInvalidTemplate;

}


FeatureTemplate_t createFeatureTemplate(const char *templatestr, uint32_t max_feature) {
    FeatureTemplate_t ft = NULL;

    DArray *patterns = split(templatestr, "_");

    FEATURETEMPLATE_CHECK_RETURN(init_ft_arrays(&ft, DArray_count(patterns), max_feature))

    for (int pi = 0; pi < DArray_count(patterns); pi++) {
        char *pattern = (char *) DArray_get(patterns, pi);

        FEATURETEMPLATE_CHECK_RETURN(add_template_piece(pattern, pi, ft))

    }

    //todo: Some idiot malloc fault.
    //DArray_clear_destroy(patterns);


    return ft;
}

/**
 *
 * @param sent
 * @param from
 * @param to
 * @param target When NULL a new vector is created by vlinear/vquadratic functions. Release of memory is deferred to user.
 *                      When a non-NULL vector is given vlinear/vquadratic functions simply perform a copy operation with no new allocation.
 * @return
 */
featureTemplateError_t arc_feature_vector(FeatureTemplate_t ft, FeaturedSentence sent, int from, int to,
                                          Vector_t *target) {

    IS_ARC_VALID(from, to, sent->length, sent->section);

    float zero = 0.f;

    newInitializedCPUVector(target, "Dense Feature Vector", 0, matrixInitFixed, &zero, NULL)
    newInitializedCPUVector(&(ft->disc), "Discrete Dense Vector", ft->ndisc_feature, matrixInitFixed, &zero, NULL)

    debug("%s matrix(%ldx%ld) of %ld elements", (*target)->identifier, (*target)->nrow, (*target)->ncol, (*target)->n);

    long offset = 0;

    char disc_feature[1024];

    bool there_is_discrete = false;


    for (int pi = 0; pi < ft->nfeatures; pi++) {

        if ((ft->type)[pi] == FT_EMBEDDING) {
            Vector_t v0 = ((Word) DArray_get(
                    sent->words,
                    0))->embedding;

            long start = ((ft->start)[pi] == -1) ? 0 : (ft->start)[pi];
            long end = ((ft->end)[pi] == -1) ? (v0->n) : ((ft->end)[pi]);

            if ((ft->node)[pi] == NODE_PARENT) {

                if (from + (ft->offset)[pi] >= 1 && from + (ft->offset)[pi] <= sent->length) {

                    Vector_t v = ((Word) DArray_get(
                            sent->words,
                            from +
                            (ft->offset)[pi] -
                            1))->embedding;

                    float *vdata = NULL;
                    long vlength;

                    VIRTUAL_SUBVECTOR(v, start, end)


                    EPARSE_CHECK_RETURN(vappend_array(target, memoryCPU, "Dense Feature Vector", vlength, vdata))
                } else {
                    for (long i = 0; i < end - start; ++i) {
                        EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", ROOT_EMBEDDING_VAL))
                    }
                }


            } else {

                if (to + (ft->offset)[pi] >= 1 && to + (ft->offset)[pi] <= sent->length) {
                    Vector_t v = ((Word) DArray_get(
                            sent->words,
                            to +
                            (ft->offset)[pi] -
                            1))->embedding;

                    float *vdata = NULL;
                    long vlength;

                    VIRTUAL_SUBVECTOR(v, start, end)


                    EPARSE_CHECK_RETURN(vappend_array(target, memoryCPU, "Dense Feature Vector", vlength, vdata))

                } else {
                    for (long i = 0; i < end - start; ++i) {
                        EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", ROOT_EMBEDDING_VAL))
                    }
                }

            }


        }
        else if ((ft->type)[pi] == FT_POSTAG) {


            if ((ft->node)[pi] == NODE_PARENT) {

                char *postag;
                if (from + (ft->offset)[pi] >= 1 && from + (ft->offset)[pi] <= sent->length) {

                    postag = ((Word) DArray_get(
                            sent->words,
                            from +
                            (ft->offset)[pi] -
                            1))->postag;

                } else {
                    postag = ROOT;
                }

                sprintf(disc_feature, "parent(%d)=%s", (ft->offset)[pi], postag);

            } else {

                char *postag;
                if (to + (ft->offset)[pi] >= 1 && to + (ft->offset)[pi] <= sent->length) {

                    postag = ((Word) DArray_get(
                            sent->words,
                            to +
                            (ft->offset)[pi] -
                            1))->postag;

                } else {
                    postag = ROOT;
                }

                sprintf(disc_feature, "child(%d)=%s", (ft->offset)[pi], postag);

            }

            there_is_discrete = true;

            long indx = (long) (murmurhash(disc_feature, (uint32_t) strlen(disc_feature), SEED) %
                                (ft->ndisc_feature));

            debug("%s->%ld", disc_feature, indx);

            (ft->disc->data)[indx]++;


        }
        else if ((ft->type)[pi] == FT_BETWEEN_POSTAG) {


            char *p_postag, *b_postag, *c_postag;

            p_postag = (from > 0) ? ((Word) DArray_get(sent->words, from - 1))->postag : ROOT;
            c_postag = (to <= sent->length) ? ((Word) DArray_get(sent->words, to - 1))->postag : ROOT;

            if (abs(from - to) > 1) {

                int n = 0;

                for (int b = MIN(from, to) + 1; b < MAX(from, to); b++) {


                    //log_info("from=%d, to=%d, b=%d",MIN(from, to),vMAX(from, to), b);
                    char *b_postag = ((Word) DArray_get(sent->words, b - 1))->postag;


                    sprintf(disc_feature, "parent=%s_between=%s_child=%s", p_postag, b_postag, c_postag);

                    long indx = (long) (murmurhash(disc_feature, (uint32_t) strlen(disc_feature), SEED) %
                                        (ft->ndisc_feature));

                    debug("%s->%ld", disc_feature, indx);

                    (ft->disc->data)[indx]++;
                }
            } else {

                sprintf(disc_feature, "parent=%s_between=%s_child=%s", p_postag, b_none, c_postag);

                long indx = (long) (murmurhash(disc_feature, (uint32_t) strlen(disc_feature), SEED) %
                                    (ft->ndisc_feature));

                debug("%s->%ld", disc_feature, indx);

                (ft->disc->data)[indx]++;

            }

            there_is_discrete = true;


        }
        else if ((ft->type)[pi] == FT_BETWEEN_EMBEDDING) {
            Vector_t v0 = ((Word) DArray_get(
                    sent->words,
                    0))->embedding;
            long start = ((ft->start)[pi] == -1) ? 0 : (ft->start)[pi];
            long end = ((ft->end)[pi] == -1) ? (v0->n) : ((ft->end)[pi]);

            newInitializedCPUVector(&(ft->avg_v), "Average vector of embeddings in between",
                                    end - start,
                                    matrixInitFixed,
                                    &zero, NULL)


            if (abs(from - to) > 1) {

                int n = 0;

                for (int b = MIN(from, to); b <= MAX(from, to); b++) {


                    //log_info("from=%d, to=%d, b=%d",MIN(from, to),vMAX(from, to), b);
                    Vector_t b_vec = ((Word) DArray_get(sent->words, b - 1))->embedding;

                    float *vdata = NULL;
                    long vlength;

                    VIRTUAL_SUBVECTOR(b_vec, start, end)

                    for (long bi = 0; bi < vlength; bi++)
                        (ft->avg_v->data)[bi] += (vdata)[bi];

                    n++;
                }

                for (long bi = 0; bi < ft->avg_v->n; bi++)
                    (ft->avg_v->data)[bi] /= n;

            }

            EPARSE_CHECK_RETURN(vappend_vector(target, memoryCPU, "Dense Feature Vector", ft->avg_v))
        }
        else if ((ft->type)[pi] == FT_THRESHOLDED_DISTANCE) {
            const int threshold_arr[] = {1, 2, 3, 4, 5, 10, 20, 30, 40};
            float threshold_flag[9];

            for (int i = 0; i < 9; i++)
                if (abs(from - to) > threshold_arr[i])
                    threshold_flag[i] = 1.;
                else
                    threshold_flag[i] = 0.;


            EPARSE_CHECK_RETURN(vappend_array(target, memoryCPU, "Dense Feature Vector", 9, threshold_flag))

        }
        else if ((ft->type)[pi] == FT_RAW_DISTANCE) {
            float rawdist = abs(from - to);

            EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", rawdist))
        }
        else if ((ft->type)[pi] == FT_NORMALIZED_DISTANCE) {
            float normdist = abs(from - to) / 250.;

            EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", normdist))
        }


        else if ((ft->type)[pi] == FT_LEFT_BOUNDARY_FLAG) {

            float boundary = (from == 1) ? 1.f : 0.f;

            EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", boundary))

            boundary = (to == 1) ? 1.f : 0.f;

            EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", boundary))

        }
        else if ((ft->type)[pi] == FT_RIGHT_BOUNDARY_FLAG) {
            float boundary = (from == sent->length) ? 1.f : 0.f;

            EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", boundary))

            boundary = (to == sent->length) ? 1.f : 0.f;

            EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", boundary))

        }

        else if ((ft->type)[pi] == FT_ROOT) {
            // Parent is root or not.
            if (from == 0) {EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", 1.))}
            else {EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", 0.))}

        }
        else if ((ft->type)[pi] == FT_DIRECTION) {
            if (from < to) {
                EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", 1.))
                EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", 0.))
            }
            else {
                EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", 0.))
                EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", 1.))
            }
        }
    }

    if (there_is_discrete) {
        EPARSE_CHECK_RETURN(vappend_vector(target, memoryCPU, "Dense Feature Vector", ft->disc))
    }

    // Add the bias term
    EPARSE_CHECK_RETURN(vappend(target, memoryCPU, "Dense Feature Vector", 1.))

    return featureTemplateSucess;

    error:

    return featureTemplateOtherError;
}

/*
featureTemplateError_t checkFeatureTemplate(const Word w, const FeatureTemplate_t ft) {

    //todo: complete implementation

    return featureTemplateSucess;


}
 */


featureTemplateError_t printFeatureTemplate(const FeatureTemplate_t ft) {

    for (int pi = 0; pi < ft->nfeatures; pi++) {

        if ((ft->type)[pi] == FT_EMBEDDING) {
            long start = (ft->start)[pi], end = (ft->end)[pi];

            if ((ft->node)[pi] == NODE_PARENT) {

                log_info("Embedding of parent (%d)", (ft->offset)[pi]);

            } else {


                log_info("Embedding of child (%d)", (ft->offset)[pi]);

            }


        }
        else if ((ft->type)[pi] == FT_POSTAG) {
            log_info("Context based PoSTag");
        }
        else if ((ft->type)[pi] == FT_BETWEEN_POSTAG) {
            log_info("Between PoSTag");
        }
        else if ((ft->type)[pi] == FT_BETWEEN_EMBEDDING) {
            long start = (ft->start)[pi];

            long end = (ft->end)[pi];


            log_info("Avg Embedding of words between parent and child (%ld, %ld)", start, end);
        }
        else if ((ft->type)[pi] == FT_THRESHOLDED_DISTANCE) {

            log_info("9 threshold distance");

        }
        else if ((ft->type)[pi] == FT_RAW_DISTANCE) {

            log_info("Raw distance");
        }
        else if ((ft->type)[pi] == FT_NORMALIZED_DISTANCE) {

            log_info("Normalized raw distance");
        }


        else if ((ft->type)[pi] == FT_LEFT_BOUNDARY_FLAG) {


            log_info("2 lbf flag");

        }
        else if ((ft->type)[pi] == FT_RIGHT_BOUNDARY_FLAG) {

            log_info("2 rbf flag");

        }

        else if ((ft->type)[pi] == FT_ROOT) {

            log_info("1 ROOT flag");

        }
        else if ((ft->type)[pi] == FT_DIRECTION) {

            log_info("2-bit direction flag");
        }
    }


    // Add the bias term

    log_info("1 bit bias flag");

    return featureTemplateSucess;


}
