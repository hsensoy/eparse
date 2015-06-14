###eparse version 0.0.6, released 14 June 2015
###eparse version 0.0.6.2, released 14 June 2015
####Fixes
    * Discrete feature vector is normalized before merge of embedding features
###eparse version 0.0.6.1, released 14 June 2015
####Fixes
    * vappend error in appending discrete feature vector to feature vector
    * Minor logging changes for discrete features
    * CMakeFile.txt is more flexible to handle MKL and CUDA
####New Features: 0.0.6
    * Discrete features of PosTags (exact via DictVectorizer) are now available by dense vectors.
    * `dep_disc_features.py` can be used to generate those features (in-complete)
    * `run_kernel_embedding_nd_postag.sh` is an example eparse script combining embedding and postag features.
    * `run_kernel_embedding_only.sh` is an example eparse script combining embedding features only.
    * `run_kernel_embedding_form_embedding_nd_postag.sh` is an example eparse script combining form embedding and postag features.
