#ifndef __OPENDP_EXTERNAL__
#define __OPENDP_EXTERNAL__

#include "circuit.h"
#include <vector>
#include <string>
#include "openroad/OpenRoad.hh"
#include "opendp/MakeOpendp.h"

namespace opendp {

class opendp_external {
public:
  opendp_external();
  ~opendp_external();

  void read_constraints(std::string constraint_file);
  void legalize_place();
  bool check_legality();
  void report_evaluation();

  double get_utilization();
  double get_sum_displacement();
  double get_average_displacement();
  double get_max_displacement();
  double get_original_hpwl();
  double get_legalized_hpwl();

private:
  circuit ckt;
  bool is_evaluated;

  friend void ord::initOpendp(ord::OpenRoad *);
};

} // namespace

#endif
