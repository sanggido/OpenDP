#include "circuit.h"
#include <cfloat>

namespace opendp {

using std::max;
using std::min;
using std::pair;
using std::cout;
using std::cerr;
using std::vector;
using std::make_pair;
using std::to_string;
using std::string;
using std::fixed;
using std::numeric_limits;
using std::ifstream;
using std::ofstream;
using std::endl;

using odb::dbRowDir;
using odb::adsRect;
using odb::dbOrientType;
using odb::dbPlacementStatus;
using odb::dbOrientType;
using odb::dbMaster;
using odb::dbMTerm;
using odb::dbMPin;
using odb::dbSigType;
using odb::dbBox;

// Row Sort Function
static bool SortByRowCoordinate(const row& lhs,
				const row& rhs);
// Row Generation Function
static vector<opendp::row> GetNewRow(const circuit* ckt);
static bool
swapWidthHeight(dbOrientType orient );

void
circuit::db_to_circuit()
{
  // LEF
  DEFdist2Microns = db->getTech()->getDbUnitsPerMicron();
  for (auto db_lib : db->getLibs()) {
    make_macros(db_lib);
  }

  block = db->getChip()->getBlock();
  adsRect die_area;
  block->getDieArea(die_area);
  lx = die.xLL = die_area.xMin();
  by = die.yLL = die_area.yMin();
  rx = die.xUR = die_area.xMax();
  ty = die.yUR = die_area.yMax();

  make_rows();
  make_cells();

  // bizzare. -cherry
  lx = die.xLL = 0.0;
  by = die.yLL = 0.0;
  rx = die.xUR = core.xUR - core.xLL;
  ty = die.yUR = core.yUR - core.yLL;

  // sort ckt->rows
  sort(prevrows.begin(), prevrows.end(), SortByRowCoordinate);
  // make rows in CoreArea;
  make_core_rows();

  cout << "CoreArea: " << endl;
  core.print();
  cout << "DieArea: " << endl;
  die.print();
}

void
circuit::make_macros(dbLib *db_lib)
{
  auto db_masters = db_lib->getMasters();
  macros.reserve(db_masters.size());
  for (auto db_master : db_masters) {
    macros.push_back(macro());
    struct macro &macro = macros.back();
    db_master_map[db_master] = &macro;

    macro.db_master = db_master;

    make_macro_obstructions(db_master, macro);
    macro_define_top_power(&macro);
  }
}

void
circuit::make_macro_obstructions(dbMaster *db_master,
				 struct macro &macro)
{
  for (auto db_box : db_master->getObstructions()) {
    rect tmpRect;
    tmpRect.xLL = db_box->xMin();
    tmpRect.yLL = db_box->yMin();
    tmpRect.xUR = db_box->xMax();
    tmpRect.yUR = db_box->yMax();
    macro.obses.push_back(tmpRect);
  }
}

// - - - - - - - define multi row cell & define top power - - - - - - - - //
void circuit::macro_define_top_power(macro* myMacro) {
  dbMaster *master = myMacro->db_master;

  dbMTerm *power = nullptr;
  dbMTerm *gnd = nullptr;
  for (dbMTerm *mterm : master->getMTerms()) {
    dbSigType sig_type = mterm->getSigType();
    if (sig_type == dbSigType::POWER)
      power = mterm;
    else if (sig_type == dbSigType::GROUND)
      gnd = mterm;
  }

  int power_y_max = power ? find_ymax(power) : 0;
  int gnd_y_max = gnd ? find_ymax(gnd) : 0;
  if (power_y_max > gnd_y_max)
    myMacro->top_power = VDD;
  else
    myMacro->top_power = VSS;

  if (power && gnd) {
    if (power->getMPins().size() > 1
	|| gnd->getMPins().size() > 1)
      myMacro->isMulti = true;
  }
}

int
circuit::find_ymax(dbMTerm *mterm)
{
  int ymax = 0;
  for (dbMPin *mpin : mterm->getMPins()) {
    for (dbBox *box : mpin->getGeometry())
      ymax = max(ymax, box->yMax());
  }
  return ymax;
}

void
circuit::make_rows()
{
  auto db_rows = block->getRows();
  rows.reserve(db_rows.size());
  for (auto db_row : db_rows) {
    prevrows.push_back(row());
    struct row &row = prevrows.back();

    row.db_row = db_row;
    const char *row_name = db_row->getConstName();
    row.name = row_name;
    row.site = db_row->getSite();
    int x, y;
    db_row->getOrigin(x, y);
    row.origX = x;
    row.origY = y;
    row.siteorient = db_row->getOrient().getString();
    row.numSites = db_row->getSiteCount();
    switch (db_row->getDirection()) {
    case dbRowDir::HORIZONTAL:
      row.stepX = db_row->getSpacing();
      row.stepY = 0;
      break;
    case dbRowDir::VERTICAL:
      row.stepX = 0;
      row.stepY = db_row->getSpacing();
      break;
    }

    // initialize rowHeight variable (double)
    if( fabs(rowHeight - 0.0f) <= DBL_EPSILON ) {
      rowHeight = row.site->getHeight();
    }

    // initialize wsite variable (int)
    if( wsite == 0 ) {
      wsite = row.site->getWidth();
    }
  
    core.xLL = min(1.0*row.origX, core.xLL);
    core.yLL = min(1.0*row.origY, core.yLL);
    core.xUR = max(1.0*row.origX + row.numSites * wsite, core.xUR);
    core.yUR = max(1.0*row.origY + rowHeight, core.yUR);
  }
}

// Y first and X second
// First row should be in the first index to get orient
static bool SortByRowCoordinate (const row& lhs,
				 const row& rhs) {
  if( lhs.origY < rhs.origY ) {
    return true;
  }
  if( lhs.origY > rhs.origY ) {
    return false;
  }

  return ( lhs.origX < rhs.origX );
}

// Generate new rows based on CoreArea
void
circuit::make_core_rows() {
  // calculation X and Y from CoreArea
  int rowCntX = IntConvert((core.xUR - core.xLL)/wsite);
  int rowCntY = IntConvert((core.yUR - core.yLL)/rowHeight);

  dbSite *site = prevrows[0].site;
  dbOrientType curOrient = prevrows[0].siteorient;

  rows.reserve(rowCntY);
  rows.clear();
  for(int i=0; i<rowCntY; i++) {
    opendp::row myRow;
    myRow.name = prevrows[i].name;
    myRow.db_row = prevrows[i].db_row;
    myRow.site = site;
    myRow.origX = IntConvert(core.xLL);
    myRow.origY = IntConvert(core.yLL + i * rowHeight);

    myRow.stepX = wsite;
    myRow.stepY = 0;

    myRow.numSites = rowCntX;
    myRow.siteorient = curOrient;
    rows.push_back(myRow);

    // curOrient is flipping. e.g. R0 -> MX -> R0 -> MX -> ...
    curOrient = (curOrient == dbOrientType::R0)? dbOrientType::MX : dbOrientType::R0;
  }
}

void
circuit::make_cells()
{
  auto db_insts = block->getInsts();
  cells.reserve(db_insts.size());
  for (auto db_inst : db_insts) {
    cells.push_back(cell());
    struct cell &cell = cells.back();
    cell.db_inst = db_inst;
    db_inst_map[db_inst] = &cell;

    dbMaster *master = db_inst->getMaster();
    auto miter = db_master_map.find(master);
    if (miter != db_master_map.end()) {
      macro *macro = miter->second;
      cell.cell_macro = macro;
   
      double width = master->getWidth();
      double height = master->getHeight();
      if (swapWidthHeight(db_inst->getOrient()))
	std::swap(width, height);
      cell.width = width;
      cell.height = height;

      cell.isDummy = false;
      cell.isFixed = (db_inst->getPlacementStatus() == dbPlacementStatus::FIRM);
  
      // Shift by core.xLL and core.yLL
      int x, y;
      db_inst->getLocation(x, y);
      cell.init_x_coord = std::max(0.0, x - core.xLL);
      cell.init_y_coord = std::max(0.0, y - core.yLL);

      // fixed cells
      if( cell.isFixed ) {
	// Shift by core.xLL and core.yLL
	cell.x_coord = x - core.xLL;
	cell.y_coord = y - core.yLL;
	cell.isPlaced = true;
      }
    }
  }
}

static bool
swapWidthHeight(dbOrientType orient ) {
  switch(orient) {
  case dbOrientType::R90:
  case dbOrientType::MXR90:
  case dbOrientType::R270:
  case dbOrientType::MYR90:
    return true;
  case dbOrientType::R0:
  case dbOrientType::R180:
  case dbOrientType::MY:
  case dbOrientType::MX:
    return false;
  }
}

}
