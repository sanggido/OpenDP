#include "circuitParser.h"

CircuitParser::CircuitParser(circuit* ckt )
: ckt_(ckt) {};

int 
CircuitParser::DefDieAreaCbk(
    defrCallbackType_e c, 
    defiBox* box, 
    defiUserData ud ) {
  circuit* ckt = (circuit*) ud;
  ckt->die.xLL = box->xl();
  ckt->die.yLL = box->yl();
  ckt->die.xUR = box->xh();
  ckt->die.yUR = box->yh();
  return 0;
}

// ROW
int CircuitParser::DefRowCbk(
    defrCallbackType_e c, 
    defiRow* _row,
    defiUserData ud) {

  circuit* ckt = (circuit*) ud;
  row* myRow = ckt->locateOrCreateRow( _row->name() );
  myRow->site = ckt->site2id.at( _row->macro() );
  myRow->origX = _row->x();
  myRow->origY = _row->y();
  myRow->siteorient = _row->orient();
  myRow->numSites = max(_row->xNum(),_row->yNum());
  myRow->stepX = _row->xStep();
  myRow->stepY = _row->yStep();
  return 0;
}


int CircuitParser::DefDesignCbk(
    defrCallbackType_e c, 
    const char* string, 
    defiUserData ud) { 
  circuit* ckt = (circuit*) ud;
  ckt->design_name = string;
  return 0;
}

int CircuitParser::DefUnitsCbk(
    defrCallbackType_e c, 
    double d, 
    defiUserData ud) {
  circuit* ckt = (circuit*) ud;
  ckt->DEFdist2Microns = d;
  return 0;
}
