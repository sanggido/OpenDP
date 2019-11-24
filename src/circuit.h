/////////////////////////////////////////////////////////////////////////////
// Authors: SangGi Do(sanggido@unist.ac.kr), Mingyu Woo(mwoo@eng.ucsd.edu)
//          (respective Ph.D. advisors: Seokhyeong Kang, Andrew B. Kahng)
//
//          Original parsing structure was made by Myung-Chul Kim (IBM).
//
// BSD 3-Clause License
//
// Copyright (c) 2018, SangGi Do and Mingyu Woo
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#ifndef OPENDP_CIRCUIT_H
#define OPENDP_CIRCUIT_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <climits>
#include <algorithm>
#include <limits>
#include <assert.h>
#include <queue>
#include "mymeasure.h"
#include "opendb/db.h"

// hashmap settings
#ifdef USE_GOOGLE_HASH
#include <sparsehash/dense_hash_map>
#define INITSTR "SANGGIDO!@#!@#"
#define OPENDP_HASH_MAP google::dense_hash_map 
#else
#include <unordered_map>
#define OPENDP_HASH_MAP std::unordered_map
#endif

#define INIT false
#define FINAL true

namespace opendp {

using odb::dbDatabase;
using odb::dbBlock;
using odb::dbLib;
using odb::dbSite;
using odb::dbMaster;
using odb::dbMPin;
using odb::dbRow;
using odb::dbInst;
using odb::dbMTerm;
using odb::dbOrientType;

enum power { VDD, VSS };

struct rect {
  double xLL, yLL;
  double xUR, yUR;

  rect();
  void print(); 
};

struct macro {
  dbMaster *db_master;
  bool isMulti;       /* single row = false , multi row = true */
  int edgetypeLeft;   // 1 or 2
  int edgetypeRight;  // 1 or 2
  power top_power;      // VDD = 0  VSS = 1 enum

  macro();
  void print();
};

struct cell {
  dbInst *db_inst;
  unsigned id;
  macro *cell_macro;
  int x_coord, y_coord;           /* (in DBU) */
  int init_x_coord, init_y_coord; /* (in DBU) */
  int x_pos, y_pos;               /* (in DBU) */
  double width, height;           /* (in DBU) */
  bool isFixed;                   /* fixed cell or not */
  bool isPlaced;
  bool inGroup;
  bool hold;
  unsigned region;
  std::string group;

  double dense_factor;
  int dense_factor_count;
  unsigned binId;
  double disp;

  cell();
  void print();
};

struct pixel {
  std::string name;
  double util;
  int x_pos;
  int y_pos;
  unsigned group;  // group id

  cell* linked_cell;
  bool isValid;  // false for dummy place

  pixel();
};

struct row {
  dbRow *db_row;
  /* from DEF file */
  std::string name;
  dbSite *site;
  int origX; /* (in DBU) */
  int origY; /* (in DBU) */
  int stepX; /* (in DBU) */
  int stepY; /* (in DBU) */
  int numSites;
  dbOrientType siteorient;
  power top_power;

  std::vector< cell* > cell_list;

  row();
  void print();
};

struct group {
  std::string name;
  std::string type;
  std::string tag;
  std::vector< rect > regions;
  std::vector< cell* > siblings;
  std::vector< pixel* > pixels;
  rect boundary;
  double util;

  group();
  void print(std::string gName);
};

struct sub_region {
  rect boundary;
  int x_pos, y_pos;
  int width, height;
  std::vector< cell* > siblings;
  sub_region();
};

struct density_bin {
  double lx, hx;     /* low/high x coordinate */
  double ly, hy;     /* low/high y coordinate */
  double area;       /* bin area */
  double m_util;     /* bin's movable cell area */
  double f_util;     /* bin's fixed cell area */
  double free_space; /* bin's freespace area */
  double overflow;
  double density_limit;
  void print();
};

class circuit {
 public:
  bool GROUP_IGNORE;

  void init_large_cell_stor();

  std::map<dbMaster*, macro*> db_master_map;
  std::map<dbInst*, cell*> db_inst_map;

  /* spacing between edges  1 to 1 , 1 to 2, 2 to 2 */
  std::map< std::pair< int, int >, double > edge_spacing; 

  OPENDP_HASH_MAP< std::string, unsigned > group2id; /* group between name -> index */

  double design_util;

  // to report design info
  double sum_displacement;
  double max_displacement;
  double avg_displacement;

  unsigned num_fixed_nodes;
  double total_mArea; /* total movable cell area */
  double total_fArea; /* total fixed cell area (excluding terminal NIs) */
  double designArea;  /* total placeable area (excluding row blockages) */
  double rowHeight;
  double lx, rx, by, ty; /* placement image's left/right/bottom/top end coordintes */
  rect die;
  rect core; // COREAREA

  double minVddCoordiY; // VDD stripe coordinates for parsing
  power initial_power; // informations

  double max_utilization;
  double displacement;
  double max_disp_const;
  int wsite;
  int max_cell_height;
  unsigned num_cpu;

  /* benchmark generation */
  std::string benchmark; /* benchmark name */

  // 2D - pixel grid;
  pixel** grid;
  cell dummy_cell;
  std::vector< sub_region > sub_regions;

  unsigned DEFdist2Microns;
  std::vector< std::pair< unsigned, unsigned > > dieArea;

  dbDatabase *db;
  dbBlock *block;
  std::vector< macro > macros; /* macro list */
  std::vector< cell > cells;   /* cell list */
  std::vector< row > prevrows;     // fragmented row list
  std::vector< row > rows;     /* row list */

  std::vector< group > groups; /* group list from .def */

  std::vector< std::pair< double, cell* > > large_cell_stor;

  circuit();

  // Make circuit structs from db.
  void db_to_circuit();
  void make_sites(dbLib *db_lib);
  void make_macros(dbLib *db_lib);

  void macro_define_top_power(macro* myMacro);
  int find_ymax(dbMTerm *term);
  void make_rows();
  void make_core_rows();
  void make_cells();
  double dbuToMicrons(int dbu) { return dbu / double(DEFdist2Microns); }
  void update_db_inst_locations();

  /* read files for legalizer - parser.cpp */
  bool read_constraints(const std::string& input);
  void copy_init_to_final();
  void calc_design_area_stats();
  void report_area_stats();

  void InitOpendpAfterParse();

  // utility.cpp - By SGD
  void power_mapping();
  void evaluation();
  double Disp();
  double HPWL(bool initial);
  double calc_density_factor(double unit);

  void group_analyze();
  std::pair< int, int > nearest_coord_to_rect_boundary(cell* theCell, rect* theRect,
                                                  std::string mode);
  int dist_for_rect(cell* theCell, rect* theRect, std::string mode);
  bool check_overlap(rect cell, rect box);
  bool check_overlap(cell* theCell, rect* theRect, std::string mode);
  bool check_inside(rect cell, rect box);
  bool check_inside(cell* theCell, rect* theRect, std::string mode);
  std::pair< bool, std::pair< int, int > > bin_search(int x_pos, cell* theCell, int x,
                                            int y);
  std::pair< bool, pixel* > diamond_search(cell* theCell, int x, int y);
  bool direct_move(cell* theCell, std::string mode);
  bool direct_move(cell* theCell, int x, int y);
  bool shift_move(cell* theCell, int x, int y);
  bool shift_move(cell* theCell, std::string mode);
  bool map_move(cell* theCell, std::string mode);
  bool map_move(cell* theCell, int x, int y);
  std::vector< cell* > overlap_cells(cell* theCell);
  std::vector< cell* > get_cells_from_boundary(rect* theRect);
  double dist_benefit(cell* theCell, int x_coord, int y_coord);
  bool swap_cell(cell* cellA, cell* cellB);
  bool refine_move(cell* theCell, std::string mode);
  bool refine_move(cell* theCell, int x_coord, int y_coord);
  pixel* get_pixel(int x_pos, int y_pos);
  std::pair< bool, cell* > nearest_cell(int x_coord, int y_coord);

  // place.cpp - By SGD
  void simple_placement(CMeasure* measure = nullptr);
  void non_group_cell_pre_placement();
  void group_cell_pre_placement();
  void non_group_cell_placement(std::string mode);
  void group_cell_placement(std::string mode);
  void group_cell_placement(std::string mode, std::string mode2);
  void brick_placement_1(group* theGroup);
  void brick_placement_2(group* theGroup);
  int group_refine(group* theGroup);
  int group_annealing(group* theGroup);
  int non_group_annealing();
  int non_group_refine();

  // assign.cpp - By SGD
  void fixed_cell_assign();
  void print_pixels();
  void group_cell_region_assign();
  void non_group_cell_region_assign();
  void y_align();
  void cell_y_align(cell* theCell);
  void group_pixel_assign();
  void group_pixel_assign_2();
  void erase_pixel(cell* theCell);
  bool paint_pixel(cell* theCell, int x_pos, int y_pos);

  bool check_legality();
  void local_density_check(double unit, double target_Ut);
  void row_check(std::ofstream& os);
  void site_check(std::ofstream& os);
  void edge_check(std::ofstream& os);
  void power_line_check(std::ofstream& os);
  void placed_check(std::ofstream& log);
  void overlap_check(std::ofstream& os);
};

// parser_helper.cpp
bool is_special_char(char c);
bool read_line_as_tokens(std::istream& is, std::vector< std::string >& tokens);
void get_next_token(std::ifstream& is, std::string& token, const char* beginComment);
void get_next_n_tokens(std::ifstream& is, std::vector< std::string >& tokens, const unsigned n,
                       const char* beginComment);

int IntConvert(double fp);

}

#endif
