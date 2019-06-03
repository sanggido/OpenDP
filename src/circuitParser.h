#ifndef __CIRCUIT_PARSER__
#define __CIRCUIT_PARSER__ 0


#include "circuit.h"

class CircuitParser {
protected:
	circuit* ckt_;
public:
	CircuitParser(circuit* ckt_);
  circuit* Circuit() { return ckt_; };

  static int DefDieAreaCbk( defrCallbackType_e c, defiBox* box, defiUserData ud );
  static int DefRowCbk(defrCallbackType_e c, defiRow* _row, defiUserData ud);

  static int DefDesignCbk(defrCallbackType_e c, const char* string, defiUserData ud);
  static int DefUnitsCbk(defrCallbackType_e c, double d, defiUserData ud);

};

#endif
