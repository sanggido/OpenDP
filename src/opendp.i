%module opendp
 
%{
#include "openroad/OpenRoad.hh"
#include "opendp/opendp_external.h"
%}

%include "opendp/opendp_external.h"

%inline %{

opendp::opendp_external *
get_opendp()
{
  return ord::OpenRoad::openRoad()->getOpendp();
}

%} // inline
