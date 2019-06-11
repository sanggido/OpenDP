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

  static int DefDesignCbk(defrCallbackType_e c, const char* string, defiUserData ud);
  static int DefUnitsCbk(defrCallbackType_e c, double d, defiUserData ud);

  static int DefStartCbk(defrCallbackType_e c, int num, defiUserData ud);
  
  static int DefRowCbk(defrCallbackType_e c, defiRow* _row, defiUserData ud);

  static int DefComponentCbk(defrCallbackType_e c, defiComponent* co, defiUserData ud);

  static int DefNetCbk(defrCallbackType_e c, defiNet* net, defiUserData ud);

  static int DefSNetPathCbk(defrCallbackType_e c, defiNet* ppath, defiUserData ud);
  static int DefSNetEndCbk(defrCallbackType_e c, void*, defiUserData ud);

  static int DefRegionCbk(defrCallbackType_e c, defiRegion* re, defiUserData ud);

  static int DefGroupCbk(defrCallbackType_e c, defiGroup* group, defiUserData ud);

};

#endif
