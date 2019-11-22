import os
import sys
import subprocess as sp

useValgrind = False 
useScreen = False 

test_path = os.path.realpath(__file__)
test_dir = os.path.dirname(test_path)
opendp_dir = os.path.dirname(test_path)
openroad_dir = os.path.dirname(os.path.dirname(os.path.dirname(opendp_dir)))
prog = os.path.join(openroad_dir, "build", "src", "openroad")

def ExecuteCommand( cmd ):
  print( cmd )
  sp.call( cmd, shell=True )

def NangateRun(curList):
  # regression for TD test cases
  for curCase in curList:
    ExecuteCommand("rm -rf %s/exp" % (curCase))
  
  for curCase in curList:
    print ( "Access " + curCase + ":")
    for cFile in os.listdir(curCase):
      if cFile.endswith(".tcl") == False:
        continue
      print ( "  " + cFile )
      cmd1 = "cd %s && " + prog + " -no_init < %s | Tee ../exp/%s.log"
      cmd = cmd1 % (curCase, cFile, cFile) 
      if useValgrind: 
        ExecuteCommand("cd %s && valgrind --log-fd=1 ../opendp < %s |& tee exp/%s_valgrind.log" % (curCase, cFile, cFile) )
      elif useScreen:
        scName = "%s_%s" %(curCase, cFile)
        ExecuteCommand("screen -dmS %s bash" %(scName))
        ExecuteCommand("screen -S %s -X stuff \"%s \n\"" % (scName, cmd))
      else:
        ExecuteCommand(cmd)


if len(sys.argv) <= 1:
  print("Usage: python regression.py run")
  print("Usage: python regression.py skill")
  print("Usage: python regression.py get")
  sys.exit(0)

dirList = os.listdir(".")
nangateList = []
iccadList = []
for cdir in sorted(dirList):
  if os.path.isdir(cdir) == False:
    continue

# disable lef5.8 tests
#  if "iccad17-test" in cdir:
#    iccadList.append(cdir)
  if "nangate45-test" in cdir:
    nangateList.append(cdir)

if sys.argv[1] == "run":
  NangateRun(nangateList)
  NangateRun(iccadList)
elif sys.argv[1] == "skill":
  ExecuteCommand("for scr in $(screen -ls | awk '{print $1}'); do screen -S $scr -X kill; done")
else:
  ExecuteCommand("watch -n 3 \"grep -r '' *-test*/exp/*.rpt\"")
