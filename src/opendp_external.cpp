#include "opendp_external.h"

using std::cout;
using std::endl;

opendp_external::opendp_external() 
: constraint_file(""), is_evaluated(false) {};

opendp_external::~opendp_external() {};

bool opendp_external::init_opendp() {
  if( constraint_file != "" && ckt.read_constraints(constraint_file)) {
    return false;
  }

  ckt.InitOpendpAfterParse();

  return true;
}

bool opendp_external::legalize_place() {
  ckt.simple_placement(nullptr);
  ckt.calc_density_factor(4);
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
