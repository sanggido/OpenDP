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

// DEF's Start callback to reserve memories
int CircuitParser::DefStartCbk(
    defrCallbackType_e c,
    int num,
    defiUserData ud) {
  circuit* ckt = (circuit*) ud;
  switch(c) {
    case defrComponentStartCbkType:
      ckt->cells.reserve(num);
      break;
    case defrStartPinsCbkType:
      ckt->pins.reserve(num);
      break;
    case defrNetStartCbkType:
      ckt->nets.reserve(num);
      break;
    case defrSNetStartCbkType:
      ckt->minVddCoordiY = DBL_MAX;
      break;
  }
  return 0; 
}

// DEF's ROW parsing
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

// DEF's COMPONENT parsing
int CircuitParser::DefComponentCbk(
    defrCallbackType_e c,
    defiComponent* co, 
    defiUserData ud) {

  circuit* ckt = (circuit*) ud;
  cell* myCell = NULL;
  // newly inserted cells
  if( ckt->cell2id.find( co->id() ) == ckt->cell2id.end() ) {
    myCell = ckt->locateOrCreateCell( co->id() );
    myCell->type = ckt->macro2id[ co->name() ];
    
    macro* myMacro = &ckt->macros[ ckt->macro2id[ co->name() ]];
    myCell->width = myMacro->width * static_cast<double> (ckt->LEFdist2Microns);
    myCell->height = myMacro->height * static_cast<double> (ckt->LEFdist2Microns);
  }
  else {
    myCell = ckt->locateOrCreateCell( co->id() );
  }

  myCell->isFixed = co->isFixed();
  myCell->init_x_coord = co->placementX(); 
  myCell->init_y_coord = co->placementY();

  if( myCell->isFixed ) {
    myCell->x_coord = co->placementX();
    myCell->y_coord = co->placementY();
    myCell->isPlaced = true;
  }

  myCell->cellorient = co->placementOrientStr();
  return 0;
}

// DEF's NET
int CircuitParser::DefNetCbk(
    defrCallbackType_e c,
    defiNet* dnet, 
    defiUserData ud) {
  circuit* ckt = (circuit*) ud;
  net* myNet = NULL;

  myNet = ckt->locateOrCreateNet( dnet->name() );
  unsigned myNetId = ckt->net2id.find(myNet->name)->second;

  // subNet iterations
  for(int i=0; i<dnet->numConnections(); i++) {
    // Extract pin informations
    string pinName = (strcmp(dnet->instance(i), "PIN") == 0)? 
      dnet->pin(i) : 
      string(dnet->instance(i)) + ":" + string(dnet->pin(i));

    pin* myPin = ckt->locateOrCreatePin( pinName );
    myPin->net = myNetId;

    myNet->source = myPin->id; // ?????????

    // if it is cell inst's pin (e.g. not equal to "PIN")
    // Fill owner's info
    if( strcmp(dnet->instance(i), "PIN") != 0 ) {
      myPin->owner = ckt->cell2id[ dnet->instance(i) ];
      myPin->type = NONPIO_PIN;

      macro* theMacro = &ckt->macros[ ckt->cells[myPin->owner].type ]; 
      macro_pin* myMacroPin = &theMacro->pins[ dnet->pin(i) ];

      myPin->x_offset = 
        myMacroPin->port[0].xLL / 2 + myMacroPin->port[0].xUR / 2;
      myPin->y_offset = 
        myMacroPin->port[0].yLL / 2 + myMacroPin->port[0].yUR / 2;
    }
  }
  return 0;
}

// DEF's SPECIALNETS
// Extract VDD/VSS row informations
int CircuitParser::DefSNetPathCbk(
    defrCallbackType_e c,
    defiNet* ppath, 
    defiUserData ud) {

  cout << "snetPath" << endl;
  // Check the VDD values
  if( strcmp("vdd", ppath->name()) == 0 ||
      strcmp("VDD", ppath->name()) ) {
    if( ppath->numWires() ) {

      for(int i=0; i<ppath->numWires(); i++) {
        defiWire* wire = ppath->wire(i);

        for(int j=0; j<wire->numPaths(); j++) {
          defiPath* p = wire->path(j);
          p->initTraverse();
          int path = 0;
          while((path = (int)p->next()) != DEFIPATH_DONE ) {
            switch(path) {
              case DEFIPATH_SHAPE:
                cout << "SHAPE_STR: " << p->getShape() << endl;
                break;
            }
          } 

        }
      } 

    } 
  }
  return 0;
}

int CircuitParser::DefSNetEndCbk(
    defrCallbackType_e c,
    void*, 
    defiUserData ud) {
  
  return 0;
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
  return 0;
}

// DEF's GROUPS -> call groups
int CircuitParser::DefGroupCbk(
    defrCallbackType_e c, 
    defiGroup* gr,
    defiUserData ud) {
  
  circuit* ckt = (circuit*) ud;
  group* curGroup = ckt->locateOrCreateGroup(gr->name());
  return 0;
}
