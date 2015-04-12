source /opt/intel/mkl/bin/mklvars.sh intel64
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/usr/local/lib/perceptronmkl

eparse -t 2-22 -d 22 -o opt.rbf.feat.fixed -p /Users/husnusensoy/data/conllWSJToken_wikipedia2MUNK-25-fixed -s optimize -e p-1v_p0v_p1v_c-1v_c0v_c1v_tl_lbf_rbf_root_between -l 50 -x RBF -z .5 -v 0 -c 2 -f 3650
