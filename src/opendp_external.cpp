#include "opendp/opendp_external.h"

using std::cout;
using std::endl;

namespace sta {
// Tcl files encoded into strings.
extern const char *opendp_tcl_inits[];
}

namespace opendp {

extern "C" {
extern int Opendp_Init(Tcl_Interp *interp);
}

opendp_external::opendp_external() 
: is_evaluated(false) {};

opendp_external::~opendp_external() {};

void opendp_external::init(Tcl_Interp *tcl_interp,
			   odb::dbDatabase *db) {
  // Define swig TCL commands.
  Opendp_Init(tcl_interp);

  ckt.db = db;
}

void
opendp_external::read_constraints(std::string constraint_file) {
  ckt.read_constraints(constraint_file);
}

  
bool opendp_external::legalize_place() {
  // insert row check in top level command -cherry
  //  if( ckt->prevrows.size() <= 0)
  //    cerr << "  ERROR: rowSize is 0. Please define at least one ROW in DEF" << endl;

  ckt.db_to_circuit();
  ckt.InitOpendpAfterParse();

  ckt.simple_placement(nullptr);
  ckt.calc_density_factor(4);
  ckt.update_db_inst_locations();
  return true;
}

bool opendp_external::check_legality() {
  return ckt.check_legality();
}

double opendp_external::get_utilization() {
  return ckt.design_util; 
}

double opendp_external::get_sum_displacement() {
  if( !is_evaluated ) {
    ckt.evaluation();
    is_evaluated = true;
  }
  return ckt.sum_displacement;
}

double opendp_external::get_average_displacement() {
  if( !is_evaluated ) {
    ckt.evaluation();
    is_evaluated = true;
  }
  return ckt.avg_displacement;
}

double opendp_external::get_max_displacement() {
  if( !is_evaluated ) {
    ckt.evaluation();
    is_evaluated = true;
  }
  return ckt.max_displacement;
}

double opendp_external::get_original_hpwl() {
  return ckt.HPWL("INIT");
}

double opendp_external::get_legalized_hpwl() {
  return ckt.HPWL("");
}

} // namespace
