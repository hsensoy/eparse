#!/usr/bin/env bash

source /opt/intel/mkl/bin/mklvars.sh intel64
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/usr/local/lib/perceptronmkl

export CONTEXT_EMBEDDING="0-50"
export FORM_EMBEDDING="50-100"
export DATA_BASE_DIR=/home/ubuntu/data

export FEATURE_TEMPLATE=p-1v:$CONTEXT_EMBEDDING_p0v:$CONTEXT_EMBEDDING_p1v:$CONTEXT_EMBEDDING_c-1v:$CONTEXT_EMBEDDING_c0v:$CONTEXT_EMBEDDING_c1v:$CONTEXT_EMBEDDING_p-1postag_p0postag_p1postag_c-1postag_c0postag_c1postag_tl_lbf_rbf_root_dir_betweenpostag

echo $FEATURE_TEMPLATE

eparse -t 2-22 -d 22 -o opt.fixed -p $DATA_BASE_DIR/conllWSJToken_wikipedia2MUNK-25-fixed -s optimize -e $FEATURE_TEMPLATE -k POLYNOMIAL -x LINEAR -b 4 -v 0 -c 4
