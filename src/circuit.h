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

#ifndef EVALUATE_H
#define EVALUATE_H

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
#include <omp.h>
#include "mymeasure.h"

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

#define PI_PIN 1
#define PO_PIN 2
#define NONPIO_PIN 3

namespace opendp {

enum power { VDD, VSS };

struct rect {
  double xLL, yLL;
  double xUR, yUR;

  rect();
  void print(); 
};

struct site {
  std::string name;
  double width;                /* in microns */
  double height;               /* in microns */
  
  /* equivalent to class, I/O pad or CORE */
  std::string type;                 
  
  /* {X | Y | R90} */
  std::vector< std::string > symmetries; 

  site();
  site(const site& s);
  void print();
};

struct mincut {
  int via_num;
  double width;
  double length;
  double within;
  std::string direction;  // FROMABOVE or FROMBELOW
  mincut();
};

struct space {
  int adj;
  std::string type;
  double min, max;

  space();
};

struct layer {
  std::string name;
  std::string type;
  std::string direction;
  double xPitch;  /* in microns */
  double yPitch;  /* in microns */
  double xOffset; /* in microns */
  double yOffset; /* in microns */
  double width;   /* in microns */

  // added by SGD
  double maxWidth;
  std::string spacing;
  std::string minStep;
  double area;
  double minEnclosedArea;
  std::vector< mincut > mincut_rule;
  std::vector< space > spacing_rule;

  // -------------
  layer();
  void print();
};

struct viaRule {
  std::string name;
  std::vector< layer* > layers;
  std::vector< std::pair< double, double > > enclosure;
  std::vector< std::pair< double, double > > width;
  std::vector< std::pair< double, double > > spacing;
  rect viaRect;
  viaRule();
};

struct via {
  std::string name;
  std::string viaRule;
  std::string property;
  std::vector< std::pair< layer*, rect > > obses;
  via();
};

struct macro_pin {
  std::string direction;

  std::vector< rect > port;
  std::vector< unsigned > layer;

  std::string shape;
  macro_pin();
};

struct macro {
  std::string name;
  std::string type;        /* equivalent to class, I/O pad or CORE */
  bool isFlop;        /* clocked element or not */
  bool isMulti;       /* single row = false , multi row = true */
  double xOrig;       /* in microns */
  double yOrig;       /* in microns */
  double width;       /* in microns */
  double height;      /* in microns */
  int edgetypeLeft;   // 1 or 2
  int edgetypeRight;  // 1 or 2
  std::vector< unsigned > sites;

  OPENDP_HASH_MAP< std::string, macro_pin > pins;

  std::vector< rect > obses; /* keyword OBS for non-rectangular shapes in micros */
  power top_power;      // VDD = 0  VSS = 1 enum

  macro();
  void print();
};

struct pin {
  // from verilog
  std::string name; /* Name of pins : instance name + "_" + port_name */
  unsigned id;
  unsigned owner; /* The owners of PIs or POs are UINT_MAX */
  unsigned net;
  unsigned type;    /* 1=PI_PIN, 2=PO_PIN, 3=others */
  bool isFlopInput; /* is this pin an input  of a clocked element? */
  bool isFlopCkPort;

  // from .def
  double x_coord, y_coord; /* (in DBU) */
  double x_offset,
      y_offset; /* COG of VIA relative to the origin of a cell, (in DBU) */
  bool isFixed; /* is this node fixed? */

  pin();
  void print();
};

struct cell {
  std::string name;
  unsigned id;
  unsigned type;                  /* index to some predefined macro */
  int x_coord, y_coord;           /* (in DBU) */
  int init_x_coord, init_y_coord; /* (in DBU) */
  int x_pos, y_pos;               /* (in DBU) */
  double width, height;           /* (in DBU) */
  bool isFixed;                   /* fixed cell or not */
  bool isPlaced;
  bool inGroup;
  bool hold;
  unsigned region;
  OPENDP_HASH_MAP< std::string, unsigned > ports; /* <port name, index to the pin> */
  std::string cellorient;
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

struct net {
  std::string name;
  unsigned source;          /* input pin index to the net */
  std::vector< unsigned > sinks; /* sink pins indices of the net */

  net();
  void print();
};

struct row {
  /* from DEF file */
  std::string name;
  unsigned site;
  int origX; /* (in DBU) */
  int origY; /* (in DBU) */
  int stepX; /* (in DBU) */
  int stepY; /* (in DBU) */
  int numSites;
  std::string siteorient;
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

struct track {
  std::string axis;  // X or Y
  unsigned start;
  unsigned num_track;
  unsigned step;
  std::vector< layer* > layers;
  track()
  : axis(""), start(0), num_track(0), step(0) {}
};

class circuit {
 public:
  bool GROUP_IGNORE;

  void init_large_cell_stor();
  OPENDP_HASH_MAP< std::string, unsigned >
      macro2id; /* OPENDP_HASH_MAP between macro name and ID */
  OPENDP_HASH_MAP< std::string, unsigned >
      cell2id; /* OPENDP_HASH_MAP between cell  name and ID */
  OPENDP_HASH_MAP< std::string, unsigned >
      pin2id; /* OPENDP_HASH_MAP between pin   name and ID */
  OPENDP_HASH_MAP< std::string, unsigned >
      net2id; /* OPENDP_HASH_MAP between net   name and ID */
  OPENDP_HASH_MAP< std::string, unsigned >
      row2id; /* OPENDP_HASH_MAP between row   name and ID */
  OPENDP_HASH_MAP< std::string, unsigned >
      site2id; /* OPENDP_HASH_MAP between site  name and ID */
  OPENDP_HASH_MAP< std::string, unsigned >
      layer2id; /* OPENDP_HASH_MAP between layer name and ID */

  OPENDP_HASH_MAP< std::string, unsigned > via2id;
  std::map< std::pair< int, int >, double > edge_spacing; /* spacing OPENDP_HASH_MAP
                                                   between edges  1 to 1 , 1 to
                                                   2, 2 to 2 */
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
  double lx, rx, by,
      ty; /* placement image's left/right/bottom/top end coordintes */
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

  std::string out_def_name;
  std::string in_def_name;

  /* benchmark generation */
  std::string benchmark; /* benchmark name */

  // 2D - pixel grid;
  pixel** grid;
  cell dummy_cell;
  std::vector< sub_region > sub_regions;
  std::vector< track > tracks;

  // used for LEF file
  std::string LEFVersion;
  std::string LEFNamesCaseSensitive;
  std::string LEFDelimiter;
  std::string LEFBusCharacters;
  double LEFManufacturingGrid;

  unsigned MAXVIASTACK;
  layer* minLayer;
  layer* maxLayer;

  // used for DEF file
  std::string DEFVersion;
  std::string DEFDelimiter;
  std::string DEFBusCharacters;
  std::string design_name;
  unsigned DEFdist2Microns;
  std::vector< std::pair< unsigned, unsigned > > dieArea;

  std::vector< site > sites;   /* site list */
  std::vector< layer > layers; /* layer list */
  std::vector< macro > macros; /* macro list */
  std::vector< cell > cells;   /* cell list */
  std::vector< net > nets;     /* net list */
  std::vector< pin > pins;     /* pin list */
  
  std::vector< row > prevrows;     // fragmented row list
  std::vector< row > rows;     /* row list */

  std::vector< via > vias;
  std::vector< viaRule > viaRules;
  std::vector< group > groups; /* group list from .def */

  std::vector< std::pair< double, cell* > > large_cell_stor;

  /* locateOrCreate helper functions - parser_helper.cpp */
  macro* locateOrCreateMacro(const std::string& macroName);
  cell* locateOrCreateCell(const std::string& cellName);
  net* locateOrCreateNet(const std::string& netName);
  pin* locateOrCreatePin(const std::string& pinName);
  row* locateOrCreateRow(const std::string& rowName);
  site* locateOrCreateSite(const std::string& siteName);
  layer* locateOrCreateLayer(const std::string& layerName);
  via* locateOrCreateVia(const std::string& viaName);
  group* locateOrCreateGroup(const std::string& groupName);
  void print();

  /* IO helpers for LEF - parser.cpp */
  void read_lef_site(std::ifstream& is);
  void read_lef_property(std::ifstream& is);
  void read_lef_layer(std::ifstream& is);
  void read_lef_via(std::ifstream& is);
  void read_lef_viaRule(std::ifstream& is);
  void read_lef_macro(std::ifstream& is);
  void read_lef_macro_site(std::ifstream& is, macro* myMacro);
  void read_lef_macro_pin(std::ifstream& is, macro* myMacro);
  // priv func
  void read_lef_macro_define_top_power(macro* myMacro);

  /* IO helpers for DEF - parser.cpp */
  void read_init_def_components(std::ifstream& is);
  void read_final_def_components(std::ifstream& is);
  void read_def_vias(std::ifstream& is);
  void read_def_pins(std::ifstream& is);
  void read_def_special_nets(std::ifstream& is);
  void read_def_nets(std::ifstream& is);
  void read_def_regions(std::ifstream& is);
  void read_def_groups(std::ifstream& is);
  void write_def(const std::string& output);

  void WriteDefComponents(const std::string& inputDef);

  FILE* fileOut;

  circuit();

  /* read files for legalizer - parser.cpp */
  void print_usage();
  void read_files(int argc, char* argv[]);
  bool read_constraints(const std::string& input);
  void read_lef(const std::string& input);
  void read_tech_lef(const std::string& input);
  void read_cell_lef(const std::string& input);
  void read_def(const std::string& input, bool init_or_final);
  void read_def_size(const std::string& input);
  void copy_init_to_final();
  void calc_design_area_stats();


  // Si2 parsing engine
  int ReadDef(const std::string& input);
  // int DefVersionCbk(defrCallbackType_e c, const char* versionName, defiUserData ud);
  // int DefDividerCbk(defrCallbackType_e c, const char* h, defiUserData ud);
  // int DefDesignCbk(defrCallbackType_e c, const char* std::string, defiUserData ud);
  // int DefUnitsCbk(defrCallbackType_e c, double d, defiUserData ud);
  // int DefDieAreaCbk(defrCallbackType_e c, defiBox* box, defiUserData ud);  
  // int DefRowCbk(defrCallbackType_e c, defiRow* row, defiUserData ud);


  int ReadLef(const std::vector<std::string>& lefStor);
  

  void InitOpendpAfterParse();

  // utility.cpp - By SGD
  void power_mapping();
  void evaluation();
  double Disp();
  double HPWL(std::string mode);
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

  // check_legal.cpp - By SGD
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
