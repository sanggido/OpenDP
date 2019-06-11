#include "circuitParser.h"
#include <cfloat>

CircuitParser::CircuitParser(circuit* ckt )
: ckt_(ckt) {};



//////////////////////////////////////////////////
// DEF Parsing Cbk Functions
//

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

// DEF's End callback
int CircuitParser::DefEndCbk(
    defrCallbackType_e c,
    void*,
    defiUserData ud) {

  circuit* ckt = (circuit*) ud;
  switch(c) {
    case defrSNetEndCbkType:
      // fill initial_power information
      if( (int(ckt->minVddCoordiY+0.5f) / int(ckt->rowHeight+0.5f)) % 2 == 0 ) {
        ckt->initial_power = VDD; 
      }
      else {
        ckt->initial_power = VSS;
      }
      break;

    default:
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

int CircuitParser::DefPinCbk(
    defrCallbackType_e c, 
    defiPin* pi,
    defiUserData ud) {
  
  circuit* ckt = (circuit*) ud;
  pin* myPin = ckt->locateOrCreatePin( pi->pinName() );
  if( strcmp(pi->direction(), "INPUT") == 0 ) {
    myPin -> type = PI_PIN;
  }
  else if( strcmp(pi->direction(), "OUTPUT") == 0 ) {
    myPin -> type = PO_PIN;
  }

  myPin->isFixed = pi->isFixed();

  myPin->x_coord = pi->placementX();
  myPin->y_coord = pi->placementY();

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

//      cout << "owner: " << myPin->owner << endl;
//      cout << "type: " << ckt->cells[myPin->owner].type << endl; 
      macro* theMacro = &ckt->macros[ ckt->cells[myPin->owner].type ];
//      cout << "theMacro: " << theMacro << endl;
//      cout << "theMacro pin size: " << theMacro->pins.size() << endl;
      macro_pin* myMacroPin = &theMacro->pins[ dnet->pin(i) ];
//      cout << "myMacroPin: " << myMacroPin << endl;
//      cout << "port: " << myMacroPin->port.size() << endl;

      myPin->x_offset = 
        myMacroPin->port[0].xLL / 2 + myMacroPin->port[0].xUR / 2;
      myPin->y_offset = 
        myMacroPin->port[0].yLL / 2 + myMacroPin->port[0].yUR / 2;
    }
  }
  return 0;
}

// DEF's SPECIALNETS
// Extract VDD/VSS row informations for mixed-height legalization
int CircuitParser::DefSNetCbk(
    defrCallbackType_e c,
    defiNet* swire, 
    defiUserData ud) {
  
  circuit* ckt = (circuit*) ud;

  // Check the VDD values
  if( strcmp("vdd", swire->name()) == 0 ||
      strcmp("VDD", swire->name()) ) {
    if( swire->numWires() ) {
      for(int i=0; i<swire->numWires(); i++) {
        defiWire* wire = swire->wire(i);

        for(int j=0; j<wire->numPaths(); j++) {
          defiPath* p = wire->path(j);
          p->initTraverse();
          int path = 0;
          int x1 = 0, y1 = 0, x3 = 0, y3 = 0;

          bool isMeetFirstLayer = false;
          bool isMeetFirstPoint = false;
          bool isViaRelated = false;

          int pathCnt = 0;
          while((path = (int)p->next()) != DEFIPATH_DONE ) {
            pathCnt++;
            switch(path) {
              case DEFIPATH_LAYER:
                // HARD CODE for extracting metal1. 
                // Need to be fixed later
                if( strcmp( p->getLayer(), "metal1") == 0) {
                  isMeetFirstLayer = true;
                  isMeetFirstPoint = false;
                }
                break;
              case DEFIPATH_POINT:
                if( isMeetFirstLayer ) {
                  if( !isMeetFirstPoint ) {
                    p->getPoint(&x1, &y1);
                    isMeetFirstPoint = true;
                  }
                  else {
                    p->getPoint(&x3, &y3);
                  }
                }
                break;
              case DEFIPATH_VIA:
                isViaRelated = true;
                break;

              default:
                break; 
            }
          }

          if( isMeetFirstLayer && !isViaRelated ) {
            ckt->minVddCoordiY = (ckt->minVddCoordiY < y1)? ckt->minVddCoordiY : y1;
//            cout << x1 << " " << y1 << " " << x3 << " " << y3 << endl;
          }
        }
      } 

    } 
  }
//  cout << "Final VDD min coordi: " << ckt->minVddCoordiY << endl;
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
