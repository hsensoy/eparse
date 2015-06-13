__author__ = 'husnusensoy'
sent = []
from collections import defaultdict
from math import fabs

import logging

logging.basicConfig(level=logging.INFO)
featbetween = True

disc_feat = defaultdict(int)
for sect in range(2, 22):
    logging.info("Processing section %d..."%sect)
    with open('/Users/husnusensoy/Documents/data/conllWSJToken_wikipedia2MUNK-25-fixed/%02d/wsj_00%02d.dp' % (
            sect, sect)) as fp:
        for line in fp:

            if len(line.strip()) > 0:
                wid, form, _, postag, _, _, parent, _, _, _, _ = line.strip().split('\t')
                sent.append(postag)
            else:
                for _parent in range(0, len(sent) + 1):
                    for _child in range(1, len(sent) + 1):
                        if _parent != _child:
                            for offset in [-1, 0, +1]:
                                _from = _parent + offset
                                _to = _child + offset

                                if _from <= 0 or _from > len(sent):
                                    disc_feat['parent(%d)=*' % offset] += 1
                                else:
                                    disc_feat['parent(%d)=%s' % (offset, sent[_from - 1])] += 1

                                if _to <= 0 or _to > len(sent):
                                    disc_feat['child(%d)=*' % (offset)] += 1
                                else:
                                    disc_feat['child(%d)=%s' % (offset, sent[_to - 1])] += 1

                            if featbetween:
                                if fabs(_parent - _child) > 1:
                                    for _b in range(min(_parent, _child) + 1, max(_parent, _child)):
                                        b_postag = sent[_b - 1]

                                        disc_feat['between=%s' % b_postag] += 1

                                else:
                                    disc_feat['between=-'] += 1

                sent = []

import logging

logging.basicConfig(level=logging.INFO)

with open("features.txt", "w") as fp:
    for i, (feature, occurrence) in enumerate(sorted(disc_feat.iteritems(), key=lambda x: x[1], reverse=True)):
        print >> fp, "%d\t%s\t%d" % (i, feature, occurrence)

logging.info("Total number of features are %d" % (len(disc_feat)))



