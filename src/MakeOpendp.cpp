#include <tcl.h>
#include "StaMain.hh"
#include "openroad/OpenRoad.hh"
#include "opendp/opendp_external.h"
#include "opendp/MakeOpendp.h"

namespace sta {
// Tcl files encoded into strings.
extern const char *opendp_tcl_inits[];
}

namespace ord {

extern "C" {
extern int Opendp_Init(Tcl_Interp *interp);
}

opendp::opendp_external *
makeOpendp()
{
  return new opendp::opendp_external;
}

void
deleteOpendp(opendp::opendp_external *opendp)
{
  delete opendp;
}

void
initOpendp(OpenRoad *openroad)
{
  Tcl_Interp *tcl_interp = openroad->tclInterp();
  // Define swig TCL commands.
  Opendp_Init(tcl_interp);
  // Eval encoded sta TCL sources.
  sta::evalTclInit(tcl_interp, sta::opendp_tcl_inits);
  openroad->getOpendp()->ckt.db = openroad->getDb();
}

}
