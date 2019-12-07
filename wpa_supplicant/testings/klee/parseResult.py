#!/usr/bin/python3
import sys
import re


testcase=""
arglist=""
cov=0.0
tbl={}

logFile='tmp.txt'
if len(sys.argv) > 1 and sys.argv[1] is not None:
    logFile=sys.argv[1]

with open (logFile) as fp:
    line = fp.readline()
    while line:
        m = re.search('KLEE-REPLAY: NOTE: Test file: (.*)' ,line)
        if m:
            if cov is not 0.0:
                tbl[cov]='(' + testcase + ')' + "  " + arglist
                cov = 0.0
            testcase = m.group(1)
         
        m = re.search('KLEE-REPLAY: NOTE: Arguments: (.*)' ,line)
        if m:
            arglist = m.group(1)

        m = re.search('Lines executed:([0-9]+.[0-9]+)% of ([0-9]+)' ,line)
        if m:
            cov += float(m.group(1)) * float(m.group(2))

        line = fp.readline()

    if cov is not 0.0:
        tbl[cov]='(' + testcase + ')' + "  " + arglist

for cov in sorted(tbl.keys(), reverse=True):
    print('CoveredLines:%10.2f' % cov, " => ", tbl[cov])

