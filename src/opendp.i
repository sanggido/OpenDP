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

bool
db_has_rows()
{
  odb::dbDatabase *db = ord::OpenRoad::openRoad()->getDb();
  return db->getChip()
    && db->getChip()->getBlock()
    && db->getChip()->getBlock()->getRows().size() > 0;
}

%} // inline
