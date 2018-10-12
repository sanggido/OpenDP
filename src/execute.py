#!/usr/bin/python
#################################################################
# This script is written by Mingyu Woo. (16/12/17)
# and modifed by SangGi Do (6.Oct.2018)
# http://mgwoo.github.io/
#################################################################

import os
import sys
import subprocess as sp
from datetime import datetime

dirpos = "../bench/benchmarks"
binaryName = "./OpenDP"
outpos = "../output"
logpos = "../logdir"
numThreads = 1

if len(sys.argv) <=1:
	print "usage:   ./execute.py <benchname or number>"
	print "Example: "
	print "         ./execute.py 1"
	print "         ./execute.py des_perf_b_md1"
	sys.exit(1)

benchNum = -1
benchName = ""
if sys.argv[1].isdigit():
	benchNum = int(sys.argv[1])
	benchName = sorted(os.listdir(dirpos))[benchNum]
elif sys.argv[1] == "all":
	benchName = sorted(os.listdir(dirpos))
else:
	benchName = sys.argv[1]

curTime = datetime.now().strftime('%Y-%m-%d_%H:%M:%S')

if type(benchName) is list:
	cnt = 0
	for curBench in benchName:

		exeStr = "%s -tech_lef %s/%s/tech.lef -cell_lef %s/%s/cells_modified.lef -input_def %s/%s/placed.def -cpu %d -placement_constraints %s/%s/placement.constraints -output_def %s/%s_%s.def |& tee %s/%s_%s_out.log" % (binaryName, dirpos, curBench, dirpos, curBench, dirpos, curBench, numThreads, dirpos, curBench, outpos, curBench, curTime, logpos, curBench, curTime)
		print exeStr
		sp.call(exeStr, shell=True)
		cnt = cnt+1

else:
	exeStr = "%s -tech_lef %s/%s/tech.lef -cell_lef %s/%s/cells_modified.lef -input_def %s/%s/placed.def -cpu %d -placement_constraints %s/%s/placement.constraints -output_def %s/%s_%s.def |& tee %s/%s_%s_out.log" % (binaryName, dirpos, benchName, dirpos, benchName, dirpos, benchName, numThreads, dirpos, benchName, outpos, benchName, curTime, logpos, benchName, curTime)
	print exeStr
	sp.call(exeStr, shell=True)

