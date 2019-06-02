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
binaryName = "./opendp"
outpos = "../output"
logpos = "../logdir"
numThreads = 1

def GetFileName( pos, extension ):
  lis = os.listdir( pos )
  res = []
  for curFile in lis:
    if curFile.endswith("." + extension):
      res.append(curFile)
  return res

def GetFileStr( pos, folder, ext, modeStr ):
  retList = GetFileName("%s/%s"%(pos, folder), ext)
  retStr = ""
  for rStr in retList:
    retStr = retStr + "%s %s/%s/%s " % (modeStr, pos, folder, rStr)
  return retStr

dirList = os.listdir(dirpos)

if len(sys.argv) <=1:
  print("usage:   ./execute.py <benchname or number>")
  print("Example: ")
  print("         ./execute.py 1")
  print("         ./execute.py des_perf_b_md1")

  for idx, cdir in enumerate(sorted(dirList)):
    print("%d %s" % (idx, cdir))
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

    lefStr = GetFileStr( dirpos, curBench, 'lef', '-lef' )
    defStr = GetFileStr( dirpos, curBench, 'def', '-def' )
    constStr = GetFileStr( dirpos, curBench, 'constraints', '-placement_constraints' )

    exeStr = binaryName + " " + lefStr + defStr + constStr + " -cpu %d -output_def %s/%s_%s.def | tee %s/%s_%s_out.log" % (numThreads, outpos, curBench, curTime, logpos, curBench, curTime)
    print(exeStr)
    sp.call(exeStr, shell=True)
    cnt = cnt+1

else:
  lefStr = GetFileStr( dirpos, benchName, 'lef', '-lef' )
  defStr = GetFileStr( dirpos, benchName, 'def', '-def' )
  constStr = GetFileStr( dirpos, benchName, 'constraints', '-placement_constraints' )

  exeStr = binaryName + " " + lefStr + defStr + constStr + " -cpu %d -output_def %s/%s_%s.def | tee %s/%s_%s_out.log" % (numThreads, outpos, benchName, curTime, logpos, benchName, curTime)

  print(exeStr)
  sp.call(exeStr, shell=True)


