///////////////////////////////////////////////////////////////////////////////
//// Authors: SangGi Do(sanggido@unist.ac.kr), Mingyu Woo(mwoo@eng.ucsd.edu)
////          (respective Ph.D. advisors: Seokhyeong Kang, Andrew B. Kahng)
////
////          Original parsing structure was made by Myung-Chul Kim (IBM).
////
//// BSD 3-Clause License
////
//// Copyright (c) 2018, SangGi Do and Mingyu Woo
//// All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification, are permitted provided that the following conditions are met:
////
//// * Redistributions of source code must retain the above copyright notice, this
////   list of conditions and the following disclaimer.
////
//// * Redistributions in binary form must reproduce the above copyright notice,
////   this list of conditions and the following disclaimer in the documentation
////   and/or other materials provided with the distribution.
////
//// * Neither the name of the copyright holder nor the names of its
////   contributors may be used to endorse or promote products derived from
////   this software without specific prior written permission.
////
//// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/////////////////////////////////////////////////////////////////////////////////

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
#include <sparsehash/OPENDP_HASH_MAP>
#define INITSTR "SANGGIDO!@#!@#"
using google::OPENDP_HASH_MAP;
#define OPENDP_HASH_MAP OPENDP_HASH_MAP
#else
#include <unordered_map>
using std::unordered_map;
#define OPENDP_HASH_MAP unordered_map
#endif

// def Reader modules
#include "defrReader.hpp"
#include "defiAlias.hpp"

#define INIT false
#define FINAL true

#define PI_PIN 1
#define PO_PIN 2
#define NONPIO_PIN 3

enum power { VDD, VSS };

using namespace std;
template < class T >
using max_heap = priority_queue< T >;

struct rect {
  double xLL, yLL;
  double xUR, yUR;
  rect()
      : xLL(numeric_limits< double >::max()),
        yLL(numeric_limits< double >::max()),
        xUR(numeric_limits< double >::min()),
        yUR(numeric_limits< double >::min()) {}
  void dump() { printf("%f : %f - %f : %f\n", xLL, yLL, xUR, yUR); }
};

struct site {
  string name;
  double width;                /* in microns */
  double height;               /* in microns */
  string type;                 /* equivalent to class, I/O pad or CORE */
  vector< string > symmetries; /* {X | Y | R90} */

  site() : name(""), width(0.0), height(0.0), type("") {}
  site(const site& s)
      : name(s.name),
        width(s.width),
        height(s.height),
        type(s.type),
        symmetries(s.symmetries) {}
  void print();
};

struct mincut {
  int via_num;
  double width;
  double length;
  double within;
  string direction;  // FROMABOVE or FROMBELOW
  mincut() : via_num(0), width(0.0), length(0.0), within(0.0), direction("") {}
};

struct space {
  int adj;
  string type;
  double min, max;
  space() : adj(0), type(""), min(0), max(0) {}
};

struct layer {
  string name;
  string type;
  string direction;
  double xPitch;  /* in microns */
  double yPitch;  /* in microns */
  double xOffset; /* in microns */
  double yOffset; /* in microns */
  double width;   /* in microns */

  // added by SGD
  double maxWidth;
  string spacing;
  string minStep;
  double area;
  double minEnclosedArea;
  vector< mincut > mincut_rule;
  vector< space > spacing_rule;
  // -------------
  layer()
      : name(""),
        type(""),
        direction(""),
        xPitch(0.0),
        yPitch(0.0),
        xOffset(0.0),
        yOffset(0.0),
        width(0.0),
        maxWidth(0.0),
        spacing(""),
        minStep(""),
        area(0.0),
        minEnclosedArea(0.0) {}
  void print();
};

struct viaRule {
  string name;
  vector< layer* > layers;
  vector< pair< double, double > > enclosure;
  vector< pair< double, double > > width;
  vector< pair< double, double > > spacing;
  rect viaRect;
  viaRule() : name("") {}
};

struct via {
  string name;
  string viaRule;
  string property;
  vector< pair< layer*, rect > > obses;
  via() : name(""), viaRule(""), property("") {}
};

struct macro_pin {
  string direction;

  vector< rect > port;
  vector< unsigned > layer;

  string shape;
  macro_pin() : direction(""), shape(""), layer(0) {}
};

struct macro {
  string name;
  string type;        /* equivalent to class, I/O pad or CORE */
  bool isFlop;        /* clocked element or not */
  bool isMulti;       /* single row = false , multi row = true */
  double xOrig;       /* in microns */
  double yOrig;       /* in microns */
  double width;       /* in microns */
  double height;      /* in microns */
  int edgetypeLeft;   // 1 or 2
  int edgetypeRight;  // 1 or 2
  vector< unsigned > sites;

  OPENDP_HASH_MAP< string, macro_pin > pins;

  vector< rect > obses; /* keyword OBS for non-rectangular shapes in micros */
  power top_power;      // VDD = 0  VSS = 1 enum

  macro()
      : name(""),
        type(""),
        isFlop(false),
        isMulti(false),
        xOrig(0.0),
        yOrig(0.0),
        width(0.0),
        height(0.0),
        edgetypeLeft(0),
        edgetypeRight(0) {
#ifdef USE_GOOGLE_HASH
    pins.set_empty_key(INITSTR);
#endif
  }
  void print();
};

struct pin {
  // from verilog
  string name; /* Name of pins : instance name + "_" + port_name */
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

  pin()
      : name(""),
        id(UINT_MAX),
        owner(UINT_MAX),
        net(UINT_MAX),
        type(UINT_MAX),
        isFlopInput(false),
        isFlopCkPort(false),
        x_coord(0.0),
        y_coord(0.0),
        x_offset(0.0),
        y_offset(0.0),
        isFixed(false) {}
  void print();
};

struct cell {
  string name;
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
  OPENDP_HASH_MAP< string, unsigned > ports; /* <port name, index to the pin> */
  string cellorient;
  string group;

  double dense_factor;
  int dense_factor_count;
  unsigned binId;
  double disp;

  cell()
      : name(""),
        type(UINT_MAX),
        id(UINT_MAX),
        x_coord(0),
        y_coord(0),
        init_x_coord(0),
        init_y_coord(0),
        x_pos(INT_MAX),
        y_pos(INT_MAX),
        width(0.0),
        height(0.0),
        isFixed(false),
        isPlaced(false),
        inGroup(false),
        hold(false),
        region(UINT_MAX),
        cellorient(""),
        group(""),
        dense_factor(0.0),
        dense_factor_count(0),
        binId(UINT_MAX),
        disp(0.0) {
#ifdef USE_GOOGLE_HASH
    ports.set_empty_key(INITSTR);
#endif
  }
  void print();
};

struct pixel {
  string name;
  double util;
  int x_pos;
  int y_pos;
  unsigned group;  // group id

  cell* linked_cell;
  bool isValid;  // false for dummy place
  pixel()
      : name(""),
        util(0.0),
        x_pos(0.0),
        y_pos(0.0),
        group(UINT_MAX),
        linked_cell(NULL),
        isValid(true) {}
};

struct net {
  string name;
  unsigned source;          /* input pin index to the net */
  vector< unsigned > sinks; /* sink pins indices of the net */

  net() : name(""), source(UINT_MAX) {}
  void print();
};

struct row {
  /* from DEF file */
  string name;
  unsigned site;
  int origX; /* (in DBU) */
  int origY; /* (in DBU) */
  int stepX; /* (in DBU) */
  int stepY; /* (in DBU) */
  int numSites;
  string siteorient;
  power top_power;

  vector< cell* > cell_list;

  row()
      : name(""),
        site(UINT_MAX),
        origX(0),
        origY(0),
        stepX(0),
        stepY(0),
        numSites(0),
        siteorient("") {}
  void print();
};

struct group {
  string name;
  string type;
  string tag;
  vector< rect > regions;
  vector< cell* > siblings;
  vector< pixel* > pixels;
  rect boundary;
  double util;
  group() : name(""), type(""), tag(""), util(0.0) {}
  void dump(string temp_) {
    cout << temp_ << " name : " << name << " type : " << type
         << " tag : " << tag << " end line " << endl;
    for(int i = 0; i < regions.size(); i++) regions[i].dump();
  };
};

struct sub_region {
  rect boundary;
  int x_pos, y_pos;
  int width, height;
  vector< cell* > siblings;
  sub_region() : x_pos(0), y_pos(0), width(0), height(0) {
    siblings.reserve(8192);
  }
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
  string axis;  // X or Y
  unsigned start;
  unsigned num_track;
  unsigned step;
  vector< layer* > layers;
  track() : axis(""), start(0), num_track(0), step(0) {}
};

class circuit {
 public:
  bool GROUP_IGNORE;

  void init_large_cell_stor();
  OPENDP_HASH_MAP< string, unsigned >
      macro2id; /* OPENDP_HASH_MAP between macro name and ID */
  OPENDP_HASH_MAP< string, unsigned >
      cell2id; /* OPENDP_HASH_MAP between cell  name and ID */
  OPENDP_HASH_MAP< string, unsigned >
      pin2id; /* OPENDP_HASH_MAP between pin   name and ID */
  OPENDP_HASH_MAP< string, unsigned >
      net2id; /* OPENDP_HASH_MAP between net   name and ID */
  OPENDP_HASH_MAP< string, unsigned >
      row2id; /* OPENDP_HASH_MAP between row   name and ID */
  OPENDP_HASH_MAP< string, unsigned >
      site2id; /* OPENDP_HASH_MAP between site  name and ID */
  OPENDP_HASH_MAP< string, unsigned >
      layer2id; /* OPENDP_HASH_MAP between layer name and ID */

  OPENDP_HASH_MAP< string, unsigned > via2id;
  map< pair< int, int >, double > edge_spacing; /* spacing OPENDP_HASH_MAP
                                                   between edges  1 to 1 , 1 to
                                                   2, 2 to 2 */
  OPENDP_HASH_MAP< string, unsigned > group2id; /* group between name -> index */

  double design_util;
  double sum_displacement;

  unsigned num_fixed_nodes;
  double total_mArea; /* total movable cell area */
  double total_fArea; /* total fixed cell area (excluding terminal NIs) */
  double designArea;  /* total placeable area (excluding row blockages) */
  double rowHeight;
  double lx, rx, by,
      ty; /* placement image's left/right/bottom/top end coordintes */
  rect die;
  power initial_power;

  double max_utilization;
  double displacement;
  double max_disp_const;
  int wsite;
  int max_cell_height;
  unsigned num_cpu;

  string out_def_name;
  string in_def_name;

  /* benchmark generation */
  string benchmark; /* benchmark name */

  // 2D - pixel grid;
  pixel** grid;
  cell dummy_cell;
  vector< sub_region > sub_regions;
  vector< track > tracks;

  // used for LEF file
  string LEFVersion;
  string LEFNamesCaseSensitive;
  string LEFDelimiter;
  string LEFBusCharacters;
  unsigned LEFdist2Microns;
  double LEFManufacturingGrid;

  unsigned MAXVIASTACK;
  layer* minLayer;
  layer* maxLayer;

  // used for DEF file
  string DEFVersion;
  string DEFDelimiter;
  string DEFBusCharacters;
  string design_name;
  unsigned DEFdist2Microns;
  vector< pair< unsigned, unsigned > > dieArea;

  vector< site > sites;   /* site list */
  vector< layer > layers; /* layer list */
  vector< macro > macros; /* macro list */
  vector< cell > cells;   /* cell list */
  vector< net > nets;     /* net list */
  vector< pin > pins;     /* pin list */
  vector< row > rows;     /* row list */
  vector< via > vias;
  vector< viaRule > viaRules;
  vector< group > groups; /* group list from .def */

  vector< pair< double, cell* > > large_cell_stor;

  /* locateOrCreate helper functions - parser_helper.cpp */
  macro* locateOrCreateMacro(const string& macroName);
  cell* locateOrCreateCell(const string& cellName);
  net* locateOrCreateNet(const string& netName);
  pin* locateOrCreatePin(const string& pinName);
  row* locateOrCreateRow(const string& rowName);
  site* locateOrCreateSite(const string& siteName);
  layer* locateOrCreateLayer(const string& layerName);
  via* locateOrCreateVia(const string& viaName);
  group* locateOrCreateGroup(const string& groupName);
  void print();

  /* IO helpers for LEF - parser.cpp */
  void read_lef_site(ifstream& is);
  void read_lef_property(ifstream& is);
  void read_lef_layer(ifstream& is);
  void read_lef_via(ifstream& is);
  void read_lef_viaRule(ifstream& is);
  void read_lef_macro(ifstream& is);
  void read_lef_macro_site(ifstream& is, macro* myMacro);
  void read_lef_macro_pin(ifstream& is, macro* myMacro);

  /* IO helpers for DEF - parser.cpp */
  void read_init_def_components(ifstream& is);
  void read_final_def_components(ifstream& is);
  void read_def_vias(ifstream& is);
  void read_def_pins(ifstream& is);
  void read_def_special_nets(ifstream& is);
  void read_def_nets(ifstream& is);
  void read_def_regions(ifstream& is);
  void read_def_groups(ifstream& is);
  void write_def(const string& output);

  circuit()
      : GROUP_IGNORE(false),
        num_fixed_nodes(0),
        num_cpu(1),
        DEFVersion(""),
        DEFDelimiter("/"),
        DEFBusCharacters("[]"),
        design_name(""),
        DEFdist2Microns(0),
        sum_displacement(0.0),
        displacement(400.0),
        max_disp_const(0.0),
        max_utilization(100.0),
        wsite(0),
        max_cell_height(1) {
    macros.reserve(128);
    layers.reserve(32);
    rows.reserve(4096);
    sub_regions.reserve(100);
#ifdef USE_GOOGLE_HASH
    macro2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between macro name and ID */
    cell2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between cell  name and ID */
    pin2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between pin   name and ID */
    net2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between net   name and ID */
    row2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between row   name and ID */
    site2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between site  name and ID */
    layer2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between layer name and ID */
    via2id.set_empty_key(INITSTR);
    group2id.set_empty_key(INITSTR); /* group between name -> index */
#endif
  };

  /* read files for legalizer - parser.cpp */
  void print_usage();
  void read_files(int argc, char* argv[]);
  void read_constraints(const string& input);
  void read_lef(const string& input);
  void read_tech_lef(const string& input);
  void read_cell_lef(const string& input);
  void read_def(const string& input, bool init_or_final);
  void read_def_size(const string& input);
  void copy_init_to_final();
  void calc_design_area_stats();


  // Si2 parsing engine
  int ReadDef(const string& input);
  // int DefDieAreaCbk(defrCallbackType_e c, defiBox* box, defiUserData ud);  

  int DefRowCbk();


  int ReadLef(const vector<string>& lefStor);


  // utility.cpp - By SGD
  void power_mapping();
  void evaluation();
  double Disp();
  double HPWL(string mode);
  double calc_density_factor(double unit);

  void group_analyze();
  pair< int, int > nearest_coord_to_rect_boundary(cell* theCell, rect* theRect,
                                                  string mode);
  int dist_for_rect(cell* theCell, rect* theRect, string mode);
  bool check_overlap(rect cell, rect box);
  bool check_overlap(cell* theCell, rect* theRect, string mode);
  bool check_inside(rect cell, rect box);
  bool check_inside(cell* theCell, rect* theRect, string mode);
  pair< bool, pair< int, int > > bin_search(int x_pos, cell* theCell, int x,
                                            int y);
  pair< bool, pixel* > diamond_search(cell* theCell, int x, int y);
  bool direct_move(cell* theCell, string mode);
  bool direct_move(cell* theCell, int x, int y);
  bool shift_move(cell* theCell, int x, int y);
  bool shift_move(cell* theCell, string mode);
  bool map_move(cell* theCell, string mode);
  bool map_move(cell* theCell, int x, int y);
  vector< cell* > overlap_cells(cell* theCell);
  vector< cell* > get_cells_from_boundary(rect* theRect);
  double dist_benefit(cell* theCell, int x_coord, int y_coord);
  bool swap_cell(cell* cellA, cell* cellB);
  bool refine_move(cell* theCell, string mode);
  bool refine_move(cell* theCell, int x_coord, int y_coord);
  pixel* get_pixel(int x_pos, int y_pos);
  pair< bool, cell* > nearest_cell(int x_coord, int y_coord);

  // place.cpp - By SGD
  void simple_placement(CMeasure& measure);
  void non_group_cell_pre_placement();
  void group_cell_pre_placement();
  void non_group_cell_placement(string mode);
  void group_cell_placement(string mode);
  void group_cell_placement(string mode, string mode2);
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
  void row_check(ofstream& os);
  void site_check(ofstream& os);
  void edge_check(ofstream& os);
  void power_line_check(ofstream& os);
  void placed_check(ofstream& log);
  void overlap_check(ofstream& os);
};

// parser_helper.cpp
bool is_special_char(char c);
bool read_line_as_tokens(istream& is, vector< string >& tokens);
void get_next_token(ifstream& is, string& token, const char* beginComment);
void get_next_n_tokens(ifstream& is, vector< string >& tokens, const unsigned n,
                       const char* beginComment);
#endif
