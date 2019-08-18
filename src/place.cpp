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

#include "circuit.h"

#define _DEBUG

using opendp::circuit;
using opendp::cell;
using opendp::row;
using opendp::pixel;
using opendp::rect;

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::vector;
using std::pair;
using std::sort;
using std::make_pair;

double disp(cell* theCell) {
  return abs(theCell->init_x_coord - theCell->x_coord) +
         abs(theCell->init_y_coord - theCell->y_coord);
}

bool SortUpOrder(cell* a, cell* b) {
  if(a->width * a->height > b->width * b->height)
    return true;
  else if(a->width * a->height < b->width * b->height)
    return false;
  else
    return (a->dense_factor > b->dense_factor);
  // return ( disp(a) > disp(b) );
}

bool SortByDisp(cell* a, cell* b) {
  if(a->disp > b->disp)
    return true;
  else
    return false;
}

bool SortByDense(cell* a, cell* b) {
  // if( a->dense_factor*a->height > b->dense_factor*b->height )
  if(a->dense_factor > b->dense_factor)
    return true;
  else
    return false;
}

bool SortDownOrder(cell* a, cell* b) {
  if(a->width * a->height < b->width * b->height)
    return true;
  else if(a->width * a->height > b->width * b->height)
    return false;
  else
    return (disp(a) > disp(b));
}

// SIMPLE PLACEMENT ( NOTICE // FUNCTION ORDER SHOULD BE FIXED )
void circuit::simple_placement(CMeasure& measure) {
  if(groups.size() > 0) {
    // group_cell -> region assign
    group_cell_region_assign();
    cout << " group_cell_region_assign done .." << endl;
  }
  // non group cell -> sub region gen & assign
  non_group_cell_region_assign();
  cout << " non_group_cell_region_assign done .." << endl;
  cout << " - - - - - - - - - - - - - - - - - - - - - - - - " << endl;

  measure.stop_clock("resgin assign");

  // pre placement out border ( Need region assign function previously )
  if(groups.size() > 0) {
    group_cell_pre_placement();
    cout << " group_cell_pre_placement done .." << endl;
    non_group_cell_pre_placement();
    cout << " non_group_cell_pre_placement done .." << endl;
    cout << " - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
  }

  measure.stop_clock("pre-placement");

  // naive method placement ( Multi -> single )
  if(groups.size() > 0) {
    group_cell_placement("init_coord");
    cout << " group_cell_placement done .. " << endl;
    for(int i = 0; i < groups.size(); i++) {
      group* theGroup = &groups[i];
      for(int j = 0; j < 3; j++) {
        int count_a = group_refine(theGroup);
        int count_b = group_annealing(theGroup);
        if(count_a < 10 || count_b < 100) break;
      }
    }
    measure.stop_clock("Group cell placement");
  }
  non_group_cell_placement("init_coord");
  measure.stop_clock("non Group cell placement");
  cout << " non_group_cell_placement done .. " << endl;
  cout << " - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
  return;
}

void circuit::non_group_cell_pre_placement() {
  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    bool inGroup = false;
    rect* target;
    pair< int, int > coord;
    if(theCell->inGroup == true || theCell->isPlaced == true) continue;
    for(int j = 0; j < groups.size(); j++) {
      group* theGroup = &groups[j];
      for(int k = 0; k < theGroup->regions.size(); k++) {
        rect* theRect = &theGroup->regions[k];
        if(check_overlap(theCell, theRect, "init_coord") == true) {
          inGroup = true;
          target = theRect;
        }
      }
    }
    if(inGroup == true) {
      pair< int, int > coord =
          nearest_coord_to_rect_boundary(theCell, target, "init_coord");
      if(map_move(theCell, coord.first, coord.second) == true)
        theCell->hold = true;
    }
  }
  return;
}

void circuit::group_cell_pre_placement() {
  for(int i = 0; i < groups.size(); i++) {
    group* theGroup = &groups[i];
    for(int j = 0; j < theGroup->siblings.size(); j++) {
      cell* theCell = theGroup->siblings[j];
      if(theCell->isFixed == true || theCell->isPlaced == true) continue;
      int dist = INT_MAX;
      bool inGroup = false;
      rect* target;
      for(int k = 0; k < theGroup->regions.size(); k++) {
        rect* theRect = &theGroup->regions[k];
        if(check_inside(theCell, theRect, "init_coord") == true) inGroup = true;
        int temp_dist = dist_for_rect(theCell, theRect, "init_coord");
        if(temp_dist < dist) {
          dist = temp_dist;
          target = theRect;
        }
      }
      if(inGroup == false) {
        pair< int, int > coord =
            nearest_coord_to_rect_boundary(theCell, target, "init_coord");
        if(map_move(theCell, coord.first, coord.second) == true)
          theCell->hold = true;
      }
    }
  }
  return;
}

void circuit::non_group_cell_placement(string mode) {
  vector< cell* > cell_list;
  cell_list.reserve(cells.size());

  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    if(theCell->isFixed || theCell->inGroup || theCell->isPlaced) continue;

    cell_list.push_back(theCell);
  }
  sort(cell_list.begin(), cell_list.end(), SortUpOrder);

  for(int i = 0; i < cell_list.size(); i++) {
    cell* theCell = cell_list[i];
    macro* theMacro = &macros[theCell->type];
    if(theMacro->isMulti == true)
      if(map_move(theCell, mode) == false) shift_move(theCell, mode);
  }
  for(int i = 0; i < cell_list.size(); i++) {
    cell* theCell = cell_list[i];
    macro* theMacro = &macros[theCell->type];
    if(theMacro->isMulti == false)
      if(map_move(theCell, mode) == false) shift_move(theCell, mode);
  }

  return;
}

void circuit::group_cell_placement(string mode) {
  group_cell_placement(mode, "INIT");
}

void circuit::group_cell_placement(string mode, string mode2) {
  for(int i = 0; i < groups.size(); i++) {
    bool single_pass = true;
    bool multi_pass = true;

    group* theGroup = &groups[i];
    vector< cell* > cell_list;
    cell_list.reserve(cells.size());
    for(int j = 0; j < theGroup->siblings.size(); j++) {
      cell* theCell = theGroup->siblings[j];
      if(theCell->isFixed || theCell->isPlaced) continue;
      cell_list.push_back(theCell);
    }
    sort(cell_list.begin(), cell_list.end(), SortUpOrder);
    // sort( cell_list.begin(), cell_list.end(), SortByDense);
    // place multi-deck cells on each group region
    for(int j = 0; j < cell_list.size(); j++) {
      cell* theCell = cell_list[j];
      if(theCell->isFixed || theCell->isPlaced) continue;
      assert(theCell->inGroup == true);
      macro* theMacro = &macros[theCell->type];
      if(theMacro->isMulti == true) {
        multi_pass = map_move(theCell, mode);
        if(multi_pass == false) {
          cout << "map_move fail" << endl;
          break;
        }
      }
    }
    // cout << "Group util : " << theGroup->util << endl;
    if(multi_pass == true) {
      //				cout << " Group : " << theGroup->name <<
      //" multi-deck placement done - ";
      // place single-deck cells on each group region
      for(int j = 0; j < cell_list.size(); j++) {
        cell* theCell = cell_list[j];
        if(theCell->isFixed || theCell->isPlaced) continue;
        assert(theCell->inGroup == true);
        macro* theMacro = &macros[theCell->type];
        if(theMacro->isMulti == false) {
          single_pass = map_move(theCell, mode);
          if(single_pass == false) {
            //						cout << "map_move fail" <<
            //endl;
            break;
          }
        }
      }
    }
    //			if( single_pass == true )
    //				cout << "single-deck placement done" << endl;

    if(single_pass == false || multi_pass == false) {
      // Erase group cells
      for(int j = 0; j < theGroup->siblings.size(); j++) {
        cell* theCell = theGroup->siblings[j];
        erase_pixel(theCell);
      }
      //				cout << "erase done" << endl;

      // determine brick placement by utilization
      if(theGroup->util > 0.95) {
        brick_placement_1(theGroup);
      }
      else {
        brick_placement_2(theGroup);
      }
    }
  }
  return;
}

// place toward group edges
void circuit::brick_placement_1(group* theGroup) {
  rect theRect = theGroup->boundary;
  vector< pair< int, cell* > > sort_by_dist;
  sort_by_dist.reserve(theGroup->siblings.size());

  for(int i = 0; i < theGroup->siblings.size(); i++) {
    cell* theCell = theGroup->siblings[i];
    int x_tar = 0;
    int y_tar = 0;

    if(theCell->init_x_coord > (theRect.xLL + theRect.xUR) / 2)
      x_tar = theRect.xUR;
    else
      x_tar = theRect.xLL;

    if(theCell->init_y_coord > (theRect.yLL + theRect.yUR) / 2)
      y_tar = theRect.yUR;
    else
      y_tar = theRect.yLL;

    sort_by_dist.push_back(make_pair(
        abs(theCell->init_x_coord - x_tar) + abs(theCell->init_y_coord - y_tar),
        theCell));
  }

  sort(sort_by_dist.begin(), sort_by_dist.end(),
       [](const pair< int, cell* >& lhs, const pair< int, cell* >& rhs) {
         return (lhs.first < rhs.first);
       });

  for(int i = 0; i < sort_by_dist.size(); i++) {
    cell* theCell = sort_by_dist[i].second;
    int x_tar = 0;
    int y_tar = 0;
    if(theCell->init_x_coord > (theRect.xLL + theRect.xUR) / 2)
      x_tar = theRect.xUR;
    else
      x_tar = theRect.xLL;
    if(theCell->init_y_coord > (theRect.yLL + theRect.yUR) / 2)
      y_tar = theRect.yUR;
    else
      y_tar = theRect.yLL;

    bool valid = map_move(theCell, x_tar, y_tar);
    if(valid == false) {
      cout << "== WARNING !! ==" << endl;
      cout << " Can't place single ( brick place 1 ) " << theCell->name << endl;
    }
  }
  return;
}

// place toward region edges
void circuit::brick_placement_2(group* theGroup) {
  vector< pair< int, cell* > > sort_by_dist;
  sort_by_dist.reserve(theGroup->siblings.size());

  for(int i = 0; i < theGroup->siblings.size(); i++) {
    cell* theCell = theGroup->siblings[i];
    rect theRect = theGroup->regions[theCell->region];
    int x_tar = 0;
    int y_tar = 0;
    if(theCell->init_x_coord > (theRect.xLL + theRect.xUR) / 2)
      x_tar = theRect.xUR;
    else
      x_tar = theRect.xLL;
    if(theCell->init_y_coord > (theRect.yLL + theRect.yUR) / 2)
      y_tar = theRect.yUR;
    else
      y_tar = theRect.yLL;

    sort_by_dist.push_back(make_pair(
        abs(theCell->init_x_coord - x_tar) + abs(theCell->init_y_coord - y_tar),
        theCell));
  }

  sort(sort_by_dist.begin(), sort_by_dist.end(),
       [](const pair< int, cell* >& lhs, const pair< int, cell* >& rhs) {
         return (lhs.first < rhs.first);
       });

  for(int i = 0; i < sort_by_dist.size(); i++) {
    cell* theCell = sort_by_dist[i].second;
    if(theCell->hold == true) continue;
    rect theRect = theGroup->regions[theCell->region];
    int x_tar = 0;
    int y_tar = 0;
    if(theCell->init_x_coord > (theRect.xLL + theRect.xUR) / 2)
      x_tar = theRect.xUR;
    else
      x_tar = theRect.xLL;
    if(theCell->init_y_coord > (theRect.yLL + theRect.yUR) / 2)
      y_tar = theRect.yUR;
    else
      y_tar = theRect.yLL;
    bool valid = map_move(theCell, x_tar, y_tar);
    if(valid == false) {
      cout << "== WARNING !! ==" << endl;
      cout << " Can't place single ( brick place 2 ) " << theCell->name << endl;
    }
  }

  return;
}

int circuit::group_refine(group* theGroup) {
  vector< pair< double, cell* > > sort_by_disp;
  sort_by_disp.reserve(theGroup->siblings.size());

  for(int i = 0; i < theGroup->siblings.size(); i++) {
    cell* theCell = theGroup->siblings[i];
    double disp = abs(theCell->init_x_coord - theCell->x_coord) +
                  abs(theCell->init_y_coord - theCell->y_coord);
    sort_by_disp.push_back(make_pair(disp, theCell));
  }

  sort(sort_by_disp.begin(), sort_by_disp.end(),
       [](const pair< double, cell* >& lhs, const pair< double, cell* >& rhs) {
         return (lhs.first > rhs.first);
       });

  int count = 0;
  for(int i = 0; i < sort_by_disp.size() / 20; i++) {
    cell* theCell = sort_by_disp[i].second;
    if(theCell->hold == true) continue;

    if(refine_move(theCell, "init_coord") == true) count++;
  }
  // cout << " Group refine : " << count << endl;
  return count;
}

int circuit::group_annealing(group* theGroup) {
  srand(777);
  // srand(time(NULL));
  int count = 0;

  for(int i = 0; i < 1000 * theGroup->siblings.size(); i++) {
    cell* cellA = theGroup->siblings[rand() % theGroup->siblings.size()];
    cell* cellB = theGroup->siblings[rand() % theGroup->siblings.size()];

    if(cellA->hold == true || cellB->hold == true) continue;

    if(swap_cell(cellA, cellB) == true) count++;
  }
  // cout << " swap cell count : " << count << endl;
  return count;
}

int circuit::non_group_annealing() {
  srand(777);
  int count = 0;
  for(int i = 0; i < 100 * cells.size(); i++) {
    cell* cellA = &cells[rand() % cells.size()];
    cell* cellB = &cells[rand() % cells.size()];
    if(cellA->hold == true || cellB->hold == true) continue;

    if(swap_cell(cellA, cellB) == true) count++;
  }
  // cout << " swap cell count : " << count << endl;
  return count;
}

int circuit::non_group_refine() {
  vector< pair< double, cell* > > sort_by_disp;
  sort_by_disp.reserve(cells.size());

  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    if(theCell->isFixed || theCell->hold || theCell->inGroup) continue;
    sort_by_disp.push_back(make_pair(disp(theCell), theCell));
  }
  sort(sort_by_disp.begin(), sort_by_disp.end(),
       [](const pair< double, cell* >& lhs, const pair< double, cell* >& rhs) {
         return (lhs.first > rhs.first);
       });

  int count = 0;
  for(int i = 0; i < sort_by_disp.size() / 50; i++) {
    cell* theCell = sort_by_disp[i].second;
    if(theCell->hold == true) continue;
    if(refine_move(theCell, "init_coord") == true) count++;
  }
  // cout << " nonGroup refine : " << count << endl;
  return count;
}
