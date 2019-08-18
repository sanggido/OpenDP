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

#include "circuit.h"
#define _DEBUG

using opendp::circuit;
using opendp::cell;
using opendp::row;
using opendp::pixel;
using opendp::rect;

using std::max;
using std::min;
using std::pair;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::make_pair;
using std::to_string;

bool circuit::check_legality() {
  ofstream log("../logdir/check_legality.log");
  cout << " ==== CHECK LEGALITY ==== " << endl;

  row_check(log);
  site_check(log);
  power_line_check(log);
  edge_check(log);
  placed_check(log);
  overlap_check(log);
  return false;
}

void circuit::local_density_check(double unit, double target_Ut) {
  double gridUnit = unit * rowHeight;
  int x_gridNum = (int)ceil((rx - lx) / gridUnit);
  int y_gridNum = (int)ceil((ty - by) / gridUnit);
  int numBins = x_gridNum * y_gridNum;

  cout << " numBins       : " << numBins << " ( " << x_gridNum << " x "
       << y_gridNum << " )" << endl;
  cout << " bin dimension : " << gridUnit << " x " << gridUnit << endl;

  // 0. Initialize density map - bins
  vector< density_bin > bins(numBins);
  for(int i = 0; i < y_gridNum; i++) {
    for(int j = 0; j < x_gridNum; j++) {
      unsigned binId = i * x_gridNum + j;
      bins[binId].lx = lx + j * gridUnit;
      bins[binId].ly = by + i * gridUnit;
      bins[binId].hx = bins[binId].lx + gridUnit;
      bins[binId].hy = bins[binId].ly + gridUnit;

      bins[binId].hx = min(bins[binId].hx, rx);
      bins[binId].hy = min(bins[binId].hy, ty);

      bins[binId].area = max(
          (bins[binId].hx - bins[binId].lx) * (bins[binId].hy - bins[binId].ly),
          0.0);
      bins[binId].m_util = 0.0;
      bins[binId].f_util = 0.0;
      bins[binId].free_space = 0.0;
    }
  }

  /* 1. build density map */
  /* (a) calculate overlaps with row sites, and add them to free_space */
  for(vector< row >::iterator theRow = rows.begin(); theRow != rows.end();
      ++theRow) {
    int lcol = max((int)floor((theRow->origX - lx) / gridUnit), 0);
    int rcol =
        min((int)floor((theRow->origX + theRow->numSites * theRow->stepX - lx) /
                       gridUnit),
            x_gridNum - 1);
    int brow = max((int)floor((theRow->origY - by) / gridUnit), 0);
    int trow = min((int)floor((theRow->origY + rowHeight - by) / gridUnit),
                   y_gridNum - 1);

    for(int j = brow; j <= trow; j++)
      for(int k = lcol; k <= rcol; k++) {
        unsigned binId = j * x_gridNum + k;

        /* get intersection */
        double lx = max(bins[binId].lx, (double)theRow->origX);
        double hx = min(bins[binId].hx, (double)theRow->origX +
                                            theRow->numSites * theRow->stepX);
        double ly = max(bins[binId].ly, (double)theRow->origY);
        double hy = min(bins[binId].hy, (double)theRow->origY + rowHeight);

        if((hx - lx) > 1.0e-5 && (hy - ly) > 1.0e-5) {
          double common_area = (hx - lx) * (hy - ly);
          bins[binId].free_space += common_area;
          bins[binId].free_space =
              min(bins[binId].free_space, bins[binId].area);
        }
      }
  }

  /* (b) add utilization by fixed/movable objects */
  for(vector< cell >::iterator theCell = cells.begin(); theCell != cells.end();
      ++theCell) {
    if(macros[theCell->type].obses.size() <= 1) {
      int lcol = max((int)floor((theCell->x_coord - lx) / gridUnit), 0);
      int rcol =
          min((int)floor((theCell->x_coord + theCell->width - lx) / gridUnit),
              x_gridNum - 1);
      int brow = max((int)floor((theCell->y_coord - by) / gridUnit), 0);
      int trow =
          min((int)floor((theCell->y_coord + theCell->height - by) / gridUnit),
              y_gridNum - 1);

      for(int j = brow; j <= trow; j++)
        for(int k = lcol; k <= rcol; k++) {
          unsigned binId = j * x_gridNum + k;

          /* get intersection */
          double lx = max(bins[binId].lx, (double)theCell->x_coord);
          double hx =
              min(bins[binId].hx, (double)theCell->x_coord + theCell->width);
          double ly = max(bins[binId].ly, (double)theCell->y_coord);
          double hy =
              min(bins[binId].hy, (double)theCell->y_coord + theCell->height);

          if((hx - lx) > 1.0e-5 && (hy - ly) > 1.0e-5) {
            double common_area = (hx - lx) * (hy - ly);
            if(theCell->isFixed)
              bins[binId].f_util += common_area;
            else
              bins[binId].m_util += common_area;
          }
        }
    }
    // non-rectangular shapes
    else {
      for(vector< rect >::iterator theRect =
              macros[theCell->type].obses.begin();
          theRect != macros[theCell->type].obses.end(); ++theRect) {
        int lcol =
            max((int)floor((theCell->x_coord +
                            (unsigned)(theRect->xLL *
                                       static_cast< double >(DEFdist2Microns)) -
                            lx) /
                           gridUnit),
                0);
        int rcol =
            min((int)floor((theCell->x_coord +
                            (unsigned)(theRect->xUR *
                                       static_cast< double >(DEFdist2Microns)) -
                            lx) /
                           gridUnit),
                x_gridNum - 1);
        int brow =
            max((int)floor((theCell->y_coord +
                            (unsigned)(theRect->yLL *
                                       static_cast< double >(DEFdist2Microns)) -
                            by) /
                           gridUnit),
                0);
        int trow =
            min((int)floor((theCell->y_coord +
                            (unsigned)(theRect->yUR *
                                       static_cast< double >(DEFdist2Microns)) -
                            by) /
                           gridUnit),
                y_gridNum - 1);

        for(int j = brow; j <= trow; j++)
          for(int k = lcol; k <= rcol; k++) {
            unsigned binId = j * x_gridNum + k;

            /* get intersection */
            double lx =
                max(bins[binId].lx,
                    (double)theCell->x_coord +
                        (unsigned)(theRect->xLL *
                                   static_cast< double >(DEFdist2Microns)));
            double hx =
                min(bins[binId].hx,
                    (double)theCell->x_coord +
                        (unsigned)(theRect->xUR *
                                   static_cast< double >(DEFdist2Microns)));
            double ly =
                max(bins[binId].ly,
                    (double)theCell->y_coord +
                        (unsigned)(theRect->yLL *
                                   static_cast< double >(DEFdist2Microns)));
            double hy =
                min(bins[binId].hy,
                    (double)theCell->y_coord +
                        (unsigned)(theRect->yUR *
                                   static_cast< double >(DEFdist2Microns)));

            if((hx - lx) > 1.0e-5 && (hy - ly) > 1.0e-5) {
              double common_area = (hx - lx) * (hy - ly);
              if(theCell->isFixed)
                bins[binId].f_util += common_area;
              else
                bins[binId].m_util += common_area;
            }
          }
      }
    }
  }

  vector< double > util_array(numBins, 0.0);
  /* 2. determine the free space & utilization per bin */
  for(int j = 0; j < y_gridNum; j++)
    for(int k = 0; k < x_gridNum; k++) {
      unsigned binId = j * x_gridNum + k;
      bins[binId].free_space -= bins[binId].f_util;
      util_array[binId] = bins[binId].m_util / bins[binId].free_space;
#ifdef DEBUG
      if(util_array[binId] > 1.0) {
        cout << binId << " is not legal. " << endl;
        cout << " m_util: " << bins[binId].m_util << " f_util "
             << bins[binId].f_util << " free_space: " << bins[binId].free_space
             << endl;
        exit(1);
      }
#endif
    }

  return;
}

void circuit::row_check(ofstream& log) {
  bool valid = true;
  int count = 0;
  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    if(theCell->isFixed == true) continue;
    if((int)theCell->y_coord % (int)rowHeight != 0) {
      log << " row_check fail ==> " << theCell->name
          << "  y_coord : " << theCell->y_coord << endl;
      valid = false;
      count++;
    }
  }

  if(valid == false)
    cout << " row_check ==>> FAIL (" << count << ")" << endl;
  else
    cout << " row_check ==>> PASS " << endl;

  return;
}

void circuit::site_check(ofstream& log) {
  bool valid = true;
  int count = 0;
  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    if(theCell->isFixed == true) continue;
    if((int)theCell->x_coord % (int)wsite != 0) {
      log << " site_check fail ==> " << theCell->name
          << "  x_coord : " << theCell->x_coord << endl;
      valid = false;
      count++;
    }
  }
  if(valid == false)
    cout << " site_check ==>> FAIL (" << count << ")" << endl;
  else
    cout << " site_check ==>> PASS " << endl;
  return;
}

void circuit::edge_check(ofstream& log) {
  bool valid = true;
  int count = 0;

  for(int i = 0; i < rows.size(); i++) {
    vector< cell* > cell_list;
    assert(cell_list.size() == 0);
    for(int j = 0; j < rows[i].numSites; j++) {
      cell* grid_cell = grid[i][j].linked_cell;
      if(grid[i][j].isValid == false) continue;
      if(grid_cell != NULL && grid_cell->name != "FIXED_DUMMY") {
#ifdef DEBUG
        cout << "grid util : " << grid[i][j].util << endl;
        cout << "cell name : " << grid[i][j].linked_cell->name << endl;
#endif
        if(cell_list.size() == 0) {
          cell_list.push_back(grid[i][j].linked_cell);
        }
        else if(cell_list[cell_list.size() - 1] != grid[i][j].linked_cell) {
          cell_list.push_back(grid[i][j].linked_cell);
        }
      }
    }
#ifdef DEBUG
    cout << " row search done " << endl;
    cout << " cell list size : " << cell_list.size() << endl;
#endif
    if(cell_list.size() < 1) continue;

    for(int k = 0; k < cell_list.size() - 1; k++) {
#ifdef DEBUG
      cout << " left cell : " << cell_list[k]->name << endl;
      cout << " Right cell : " << cell_list[k + 1]->name << endl;
#endif
      if(cell_list.size() < 2) continue;
      macro* left_macro = &macros[cell_list[k]->type];
      macro* right_macro = &macros[cell_list[k + 1]->type];
      if(left_macro->edgetypeRight == 0 || right_macro->edgetypeLeft == 0)
        continue;
      int space =
          (int)floor(edge_spacing[make_pair(left_macro->edgetypeRight,
                                            right_macro->edgetypeLeft)] /
                         wsite +
                     0.5);
      int cell_dist = cell_list[k + 1]->x_coord - cell_list[k]->x_coord -
                      cell_list[k]->width;
      if(cell_dist < space) {
        log << " edge_check fail ==> " << cell_list[k]->name << " >> "
            << cell_dist << "(" << space << ") << " << cell_list[k + 1]->name
            << endl;
        count++;
      }
    }
  }

  if(valid == false)
    cout << " edge_check ==>> FAIL (" << count << ")" << endl;
  else
    cout << " edge_check ==>> PASS " << endl;
  return;
}

void circuit::power_line_check(ofstream& log) {
  bool valid = true;
  int count = 0;
  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    if(theCell->isFixed == true) continue;

    if(theCell->height / rowHeight == 1 || theCell->height / rowHeight == 3)
      continue;

    // should removed later
    if(theCell->inGroup == false) continue;

    macro* theMacro = &macros[theCell->type];
    int y_size = (int)floor(theCell->height / rowHeight + 0.5);
    int y_pos = (int)floor(theCell->y_coord / rowHeight + 0.5);
    if(y_size % 2 == 0) {
      if(theMacro->top_power == rows[y_pos].top_power) {
        log << " power_check fail ( even height ) ==> " << theCell->name
            << endl;
        valid = false;
        count++;
      }
    }
    else {
      if(theMacro->top_power == rows[y_pos].top_power) {
        if(theCell->cellorient != "N") {
          log << " power_check fail ( Should be N ) ==> " << theCell->name
              << endl;
          valid = false;
          count++;
        }
      }
      else {
        if(theCell->cellorient != "FS") {
          log << " power_check fail ( Should be FS ) ==> " << theCell->name
              << endl;
          valid = false;
          count++;
        }
      }
    }
  }

  if(valid == false)
    cout << " power_check ==>> FAIL (" << count << ")" << endl;
  else
    cout << " power_check ==>> PASS " << endl;
  return;
}

void circuit::placed_check(ofstream& log) {
  bool valid = true;
  int count = 0;
  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    if(theCell->isPlaced == false) {
      log << " placed_check fail ==> " << theCell->name << endl;
      valid = false;
      count++;
    }
  }

  if(valid == false)
    cout << " placed_check ==>> FAIL (" << count << ")" << endl;
  else
    cout << " placed_check ==>> PASS " << endl;

  return;
}

void circuit::overlap_check(ofstream& log) {
  bool valid = true;
  int row = rows.size();
  pixel** grid_2;
  grid_2 = new pixel*[row];
  for(int i = 0; i < row; i++) {
    int col = rows[i].numSites;
    grid_2[i] = new pixel[col];
  }

  for(int i = 0; i < row; i++) {
    int col = rows[i].numSites;
    for(int j = 0; j < col; j++) {
      grid_2[i][j].name = "pixel_" + to_string(i) + "_" + to_string(j);
      grid_2[i][j].y_pos = i;
      grid_2[i][j].x_pos = j;
      grid_2[i][j].linked_cell = NULL;
    }
  }

  for(int i = 0; i < cells.size(); i++) {
    cell* theCell = &cells[i];
    int x_pos = (int)floor(theCell->x_coord / wsite + 0.5);
    int y_pos = (int)floor(theCell->y_coord / rowHeight + 0.5);
    int x_step = (int)ceil(theCell->width / wsite);
    int y_step = (int)ceil(theCell->height / rowHeight);


    int x_ur = x_pos + x_step;
    int y_ur = y_pos + y_step;

    // Fixed Cell can be out of Current DIEAREA settings.
    if( theCell->isFixed ) {
      x_pos = max(0, x_pos); 
      y_pos = max(0, y_pos);
      x_ur = min(x_ur, IntConvert(die.xUR / wsite));
      y_ur = min(y_ur, IntConvert(die.yUR / rowHeight));
    }
   
//    cout << theCell->width / wsite << endl; 
//    cout << theCell->height / rowHeight << endl; 
//    cout << "x: " << x_pos << " " << x_ur << endl;
//    cout << "y: " << y_pos << " " << y_ur << endl;

    assert(x_pos > -1);
    assert(y_pos > -1);
    assert(x_step > 0);
    assert(y_step > 0);
    assert(x_ur <= (int)floor(die.xUR / wsite + 0.5));
    assert(y_ur <= (int)floor(die.yUR / rowHeight + 0.5));


    for(int j = y_pos; j < y_ur; j++) {
      for(int k = x_pos; k < x_ur; k++) {
        if(grid_2[j][k].linked_cell == NULL) {
          grid_2[j][k].linked_cell = theCell;
          grid_2[j][k].util = 1.0;
        }
        else {
          log << "overlap_check ==> FAIL!! ( cell " << theCell->name
              << " is overlap with " << grid_2[j][k].linked_cell->name << " ) "
              << " ( " 
              << IntConvert(k*wsite + core.xLL) << ", " 
              << IntConvert(j*rowHeight + core.yLL) << " )" 
              << endl;
          valid = false;
        }
      }
    }
  }
  if(valid == false)
    cout << " overlap_check ==>> FAIL " << endl;
  else
    cout << " overlap_check ==>> PASS " << endl;
  return;
}
