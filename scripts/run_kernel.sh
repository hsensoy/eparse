#!/usr/bin/env bash

source /opt/intel/mkl/bin/mklvars.sh intel64
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/usr/local/lib/perceptronmkl

source /opt/intel/mkl/bin/mklvars.sh intel64
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/usr/local/lib/perceptronmkl

eparse -t 2-22 -d 22 -o opt.fixed -p /Users/husnusensoy/data/conllWSJToken_wikipedia2MUNK-25-fixed -s optimize -e p-1v_p0v_p1v_c-1v_c0v_c1v_tl_lbf_rbf_root_between -l 50 -k POLYNOMIAL -x LINEAR -b 4 -v 0 -c 4