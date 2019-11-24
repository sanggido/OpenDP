#include "opendp/opendp_external.h"

namespace opendp {

opendp_external::opendp_external() 
: is_evaluated(false) {};

opendp_external::~opendp_external() {};

void
opendp_external::read_constraints(std::string constraint_file) {
  ckt.read_constraints(constraint_file);
}

void opendp_external::legalize_place() {
  ckt.db_to_circuit();
  ckt.InitOpendpAfterParse();

  ckt.simple_placement(nullptr);
  ckt.calc_density_factor(4);
  ckt.update_db_inst_locations();
}

bool opendp_external::check_legality() {
  return ckt.check_legality();
}

double opendp_external::get_utilization() {
  return ckt.design_util; 
}

void opendp_external::report_evaluation() {
  ckt.evaluation();
  is_evaluated = true;
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
  return ckt.HPWL(true);
}

double opendp_external::get_legalized_hpwl() {
  return ckt.HPWL(false);
}

} // namespace
