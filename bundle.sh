git archive --format=tar --prefix=eparse-$1/ v$1 >eparse-$1.tar
tar --append --file=eparse-$1.tar data
gzip eparse-$1.tar
