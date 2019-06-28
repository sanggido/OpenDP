#ifndef __CIRCUIT_PARSER__
#define __CIRCUIT_PARSER__ 0


#include "circuit.h"

// lef Reader modules
#include "lefrReader.hpp"
// def Reader modules
#include "defrReader.hpp"
#include "defiAlias.hpp"

class CircuitParser {
protected:
  opendp::circuit* ckt_;
  static opendp::macro* topMacro_;
  static opendp::group* topGroup_;

public:
	CircuitParser(opendp::circuit* ckt_);
  opendp::circuit* Circuit() { return ckt_; };

  // LEF CallBacks.
  static int LefLayerCbk( lefrCallbackType_e c, lefiLayer* la, lefiUserData ud );
  static int LefSiteCbk( lefrCallbackType_e c, lefiSite* si, lefiUserData ud );
  
  static int LefStartCbk( lefrCallbackType_e c, const char* name, lefiUserData ud );
  static int LefMacroCbk( lefrCallbackType_e c, lefiMacro* ma, lefiUserData ud ); 
  static int LefMacroPinCbk( lefrCallbackType_e c, lefiPin * ma, lefiUserData ud );
  static int LefMacroObsCbk( lefrCallbackType_e c, lefiObstruction* obs, lefiUserData ud );
  static int LefEndCbk( lefrCallbackType_e c, const char* name, lefiUserData ud );


  // DEF CallBacks
  static int DefDieAreaCbk( defrCallbackType_e c, defiBox* box, defiUserData ud );
  static int DefDesignCbk(defrCallbackType_e c, const char* string, defiUserData ud);
  static int DefUnitsCbk(defrCallbackType_e c, double d, defiUserData ud);

  static int DefStartCbk(defrCallbackType_e c, int num, defiUserData ud);
  static int DefEndCbk(defrCallbackType_e c, void*, defiUserData ud);
  
  static int DefRowCbk(defrCallbackType_e c, defiRow* ro, defiUserData ud);
  static int DefPinCbk(defrCallbackType_e c, defiPin* pi, defiUserData ud);
  static int DefComponentCbk(defrCallbackType_e c, defiComponent* co, defiUserData ud);
  static int DefNetCbk(defrCallbackType_e c, defiNet* net, defiUserData ud);
  static int DefSNetCbk(defrCallbackType_e c, defiNet* wire, defiUserData ud);

  static int DefRegionCbk(defrCallbackType_e c, defiRegion* re, defiUserData ud);

  static int DefGroupNameCbk(defrCallbackType_e c, const char* name, defiUserData ud);
  static int DefGroupMemberCbk(defrCallbackType_e c, const char* name, defiUserData ud);


  // DEF writing function
  static int DefComponentWriteCbk(defrCallbackType_e c, defiComponent* co, defiUserData ud);

};

#endif
