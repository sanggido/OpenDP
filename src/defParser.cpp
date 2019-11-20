// *****************************************************************************
// *****************************************************************************
// Copyright 2012 - 2017, Cadence Design Systems
//
// This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
// Distribution,  Product Version 5.8.
//
// Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
//
//  $Author$
//  $Revision$
//  $Date$
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif /* not WIN32 */
#include "circuit.h"
#include "circuitParser.h"

namespace opendp {

int circuit::ReadDef() {
#if 0
  CircuitParser cp(this);

  // 
  // CircuitCallBack 
  //
  defrSetDesignCbk(cp.DefDesignCbk);
  defrSetUnitsCbk(cp.DefUnitsCbk);
  defrSetDieAreaCbk((defrBoxCbkFnType)cp.DefDieAreaCbk);
 
  // rows 
  defrSetRowCbk((defrRowCbkFnType)cp.DefRowCbk);
  
  defrSetComponentStartCbk(cp.DefStartCbk);
  defrSetNetStartCbk(cp.DefStartCbk);
  defrSetSNetStartCbk(cp.DefStartCbk);
  defrSetStartPinsCbk(cp.DefStartCbk);

  defrSetSNetEndCbk(cp.DefEndCbk);

  // Components
  defrSetComponentCbk(cp.DefComponentCbk);
  
  // pins
  defrSetPinCbk((defrPinCbkFnType)cp.DefPinCbk);

  // Nets
  defrSetNetCbk(cp.DefNetCbk);
  defrSetSNetCbk(cp.DefSNetCbk);
  defrSetAddPathToNet();

  // Regions
  defrSetRegionCbk((defrRegionCbkFnType)cp.DefRegionCbk);

  // Groups
  defrSetGroupNameCbk((defrStringCbkFnType)cp.DefGroupNameCbk);
  defrSetGroupMemberCbk((defrStringCbkFnType)cp.DefGroupMemberCbk);

  // End Design
  defrSetDesignEndCbk(cp.DefEndCbk);
#endif
  return 0;
}

}
