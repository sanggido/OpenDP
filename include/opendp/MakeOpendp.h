
#ifndef MAKE_OPENDP
#define MAKE_OPENDP

namespace opendp {
class opendp_external;
}

namespace ord {

class OpenRoad;

opendp::opendp_external *
makeOpendp();
void
initOpendp(OpenRoad *openroad);
void
deleteOpendp(opendp::opendp_external *opendp);

}

#endif
