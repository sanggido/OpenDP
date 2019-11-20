#ifndef __OPENDP_EXTERNAL__
#define __OPENDP_EXTERNAL__

#include "circuit.h"
#include <vector>
#include <string>

class opendp_external {
public:
  opendp_external();
  ~opendp_external();

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
  std::string constraint_file;
  bool is_evaluated;
};

#endif
