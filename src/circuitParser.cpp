#include "circuitParser.h"
#include <cfloat>

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


// DEF's COMPONENT parsing
int CircuitParser::DefComponentInitCbk(
    defrCallbackType_e c,
    int num,
    defiUserData ud) {
  circuit* ckt = (circuit*) ud;
  ckt->cells.reserve(num);
}

int CircuitParser::DefComponentCbk(
    defrCallbackType_e c,
    defiComponent* co, 
    defiUserData ud) {

  circuit* ckt = (circuit*) ud;
  cell* myCell = NULL;
  // newly inserted cells
  if( ckt->cell2id.find( co->name() ) == ckt->cell2id.end() ) {
    myCell = ckt->locateOrCreateCell( co->name() );
  }

}


// Fence region handling
// DEF's REGIONS -> call groups
int CircuitParser::DefRegionCbk(
    defrCallbackType_e c,
    defiRegion* re, 
    defiUserData ud) {

  circuit* ckt = (circuit*) ud;
  group* curGroup = ckt->locateOrCreateGroup(re->name());

  // initialize for BB
  curGroup->boundary.xLL = DBL_MAX;
  curGroup->boundary.yLL = DBL_MAX;
  curGroup->boundary.xUR = DBL_MIN;
  curGroup->boundary.yUR = DBL_MIN;

  for(int i = 0; i < re->numRectangles(); i++) {
    rect tmpRect;
    tmpRect.xLL = re->xl(i);
    tmpRect.yLL = re->yl(i);
    tmpRect.xUR = re->xl(i) + re->xh(i);
    tmpRect.yUR = re->yl(i) + re->yh(i); 
    
    // Extract BB
    curGroup->boundary.xLL = min(curGroup->boundary.xLL, tmpRect.xLL);
    curGroup->boundary.yLL = min(curGroup->boundary.yLL, tmpRect.yLL);
    curGroup->boundary.xUR = max(curGroup->boundary.xUR, tmpRect.xUR);
    curGroup->boundary.yUR = max(curGroup->boundary.yUR, tmpRect.yUR);
    
    // push rect info
    curGroup->regions.push_back( tmpRect );
  }
  curGroup->type = re->type();
}

// DEF's GROUPS -> call groups
int CircuitParser::DefGroupCbk(
    defrCallbackType_e c, 
    defiGroup* gr,
    defiUserData ud) {
  
  circuit* ckt = (circuit*) ud;
  group* curGroup = ckt->locateOrCreateGroup(gr->name());

}
