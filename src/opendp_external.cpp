#include "opendp_external.h"

using std::cout;
using std::endl;

opendp_external::opendp_external() 
: def_file(""), constraint_file(""), is_evaluated(false) {};

opendp_external::~opendp_external() {};

void opendp_external::help() {
  cout << "help message will be printed" << endl;
}

void opendp_external::import_lef(const char* lef) {
  lef_stor.push_back(lef);
}

void opendp_external::import_def(const char* def) {
  ckt.in_def_name = def_file = def;
}

void opendp_external::import_constraint(const char* constraint) {
  constraint_file = constraint;
}

void opendp_external::export_def(const char* def) {
  ckt.write_def(def);
}

bool opendp_external::init_opendp() {
  if( ckt.ReadLef(lef_stor)) {
    return false;
  }
  if( ckt.ReadDef(def_file) ) {
    return false;
  }

  if( constraint_file != "" && ckt.read_constraints(constraint_file)) {
    return false;
  }

  ckt.InitOpendpAfterParse();

  return true;
}

bool opendp_external::legalize_place() {
  ckt.simple_placement(nullptr);
  ckt.calc_density_factor(4);
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
