#include "openroad/OpenRoad.hh"
#include "opendp/opendp_external.h"
#include "opendp/MakeOpendp.h"

namespace ord {

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
  openroad->getOpendp()->init(openroad->tclInterp(),
			      openroad->getDb());
}

}
