#!/usr/bin/env bash
source /opt/intel/mkl/bin/mklvars.sh intel64
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/usr/local/lib/perceptronmkl

export CONTEXT_EMBEDDING="0-50"
export FORM_EMBEDDING="50-100"

export FEATURE_TEMPLATE=p0v:0-25_c0v:0-25_p-1postag_p0postag_p1postag_c-1postag_c0postag_c1postag_tl_lbf_rbf_root_betweenpostag_dir

echo $FEATURE_TEMPLATE

eparse -t 2-22 -d 22 -o opt.fixed -p /Users/husnusensoy/data/conllWSJToken_wikipedia2MUNK-25-fixed -s optimize -e $FEATURE_TEMPLATE -k POLYNOMIAL -x LINEAR -b 4 -v 0 -c 4 -m 100