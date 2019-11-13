#ifndef __OPENDP_EXTERNAL__
#define __OPENDP_EXTERNAL__

#include "circuit.h"
#include <vector>
#include <string>

class opendp_external {
public:
  opendp_external();
  ~opendp_external();

  void help();

  void import_lef(const char* lef);
  void import_def(const char* def);
  void import_constraint(const char* constraint);
  void export_def(const char* def);

  bool init_opendp();
  bool legalize_place();
  bool check_legality();

  double get_utilization();
  double get_sum_displacement();
  double get_average_displacement();
  double get_max_displacement();
  double get_original_hpwl();
  double get_legalized_hpwl();

private:
  opendp::circuit ckt;
  std::vector<std::string> lef_stor;
  std::string def_file;
  std::string constraint_file;
  bool is_evaluated;
}; 

#endif
