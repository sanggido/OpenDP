#include "circuit.h"
#include "circuitParser.h"

using opendp::circuit;

using std::max;
using std::min;
using std::pair;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::make_pair;
using std::to_string;
using std::string;

namespace opendp {

int
circuit::ReadLef() {
  CircuitParser cp(this);
  void* userData = cp.Circuit();
  
#if 0
  // Layer
  lefrSetLayerCbk(cp.LefLayerCbk);
  
  // Site
  lefrSetSiteCbk(cp.LefSiteCbk);
  
  // Macro 
  lefrSetMacroBeginCbk(cp.LefStartCbk);
  lefrSetMacroCbk(cp.LefMacroCbk);
//  lefrSetMacroCbk(macroCB);
  lefrSetPinCbk(cp.LefMacroPinCbk);
  lefrSetObstructionCbk(cp.LefMacroObsCbk);
  lefrSetMacroEndCbk(cp.LefEndCbk);
  
  lefrSetWarningLogFunction(printWarning);
  
  lefrSetLineNumberFunction(lineNumberCB);
  lefrSetDeltaNumberLines(10000);

  lefrSetRegisterUnusedCallbacks();
  lefrSetLogFunction(errorCB);
  lefrSetWarningLogFunction(warningCB);

  for(auto curLefLoc : lefStor) {
    lefrReset();

    if ((f = fopen(curLefLoc.c_str(),"r")) == 0) {
      cout << "Couldn't open input file " << curLefLoc << endl;
      return(2);
    }

    (void)lefrEnableReadEncrypted();

    int status = lefwInit(fout); // initialize the lef writer,
    // need to be called 1st
    if (status != LEFW_OK)
      return 1;

    int res = lefrRead(f, curLefLoc.c_str(), (void*)userData);

    if (res) {
      cout << "Reader returns bad status: " << curLefLoc << endl;
      exit(1);
    }
    else {
      cout << "Reading " << curLefLoc << " is Done" << endl;
    }

//    (void)lefrPrintUnusedCallbacks(fout);
    (void)lefrReleaseNResetMemory();

  }
#endif
  return 0;
}

}
