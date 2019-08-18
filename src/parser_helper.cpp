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

using opendp::circuit;
using opendp::cell;
using opendp::row;
using opendp::pixel;
using opendp::rect;
using opendp::pin;
using opendp::macro;
using opendp::net;
using opendp::site;
using opendp::layer;
using opendp::via;
using opendp::group;
using opendp::density_bin;

using std::max;
using std::min;
using std::pair;
using std::cout;
using std::cerr;
using std::endl;
using std::istream;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::make_pair;
using std::to_string;
using std::string;
using std::fixed;
using std::numeric_limits;

// requires full name, e.g., cell_instance/pin
pin *circuit::locateOrCreatePin(const string &pinName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = pin2id.find(pinName);
  if(it == pin2id.end()) {
    pin thePin;
    thePin.name = pinName;
    thePin.id = pins.size();
    pin2id.insert(make_pair(pinName, thePin.id));
    pins.push_back(thePin);
    return &pins[pins.size() - 1];
  }
  else
    return &pins[it->second];
}

cell *circuit::locateOrCreateCell(const string &cellName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = cell2id.find(cellName);
  if(it == cell2id.end()) {
    cell theCell;
    theCell.name = cellName;
    theCell.id = cells.size();
    cell2id.insert(make_pair(theCell.name, cells.size()));
    cells.push_back(theCell);
    return &cells[cells.size() - 1];
  }
  else
    return &cells[it->second];
}

macro *circuit::locateOrCreateMacro(const string &macroName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = macro2id.find(macroName);
  if(it == macro2id.end()) {
    macro theMacro;
    theMacro.name = macroName;
    macro2id.insert(make_pair(theMacro.name, macros.size()));
    macros.push_back(theMacro);
    return &macros[macros.size() - 1];
  }
  else
    return &macros[it->second];
}

net *circuit::locateOrCreateNet(const string &netName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = net2id.find(netName);
  if(it == net2id.end()) {
    net theNet;
    theNet.name = netName;
    net2id.insert(make_pair(theNet.name, nets.size()));
    nets.push_back(theNet);
    return &nets[nets.size() - 1];
  }
  else
    return &nets[it->second];
}

row *circuit::locateOrCreateRow(const string &rowName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = row2id.find(rowName);
  if(it == row2id.end()) {
    row theRow;
    theRow.name = rowName;
    row2id.insert(make_pair(theRow.name, prevrows.size()));
    prevrows.push_back(theRow);
    return &prevrows[prevrows.size() - 1];
  }
  else
    return &prevrows[it->second];
}

site *circuit::locateOrCreateSite(const string &siteName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = site2id.find(siteName);
  if(it == site2id.end()) {
    site theSite;
    theSite.name = siteName;
    site2id.insert(make_pair(theSite.name, sites.size()));
    sites.push_back(theSite);
    return &sites[sites.size() - 1];
  }
  else
    return &sites[it->second];
}

layer *circuit::locateOrCreateLayer(const string &layerName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = layer2id.find(layerName);
  if(it == layer2id.end()) {
    layer theLayer;
    theLayer.name = layerName;
    layer2id.insert(make_pair(theLayer.name, layers.size()));
    layers.push_back(theLayer);
    return &layers[layers.size() - 1];
  }
  else
    return &layers[it->second];
}

via *circuit::locateOrCreateVia(const string &viaName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = via2id.find(viaName);
  if(it == via2id.end()) {
    via theVia;
    theVia.name = viaName;
    via2id.insert(make_pair(theVia.name, vias.size()));
    vias.push_back(theVia);
    return &vias[vias.size() - 1];
  }
  else
    return &vias[it->second];
}

group *circuit::locateOrCreateGroup(const string &groupName) {
  OPENDP_HASH_MAP< string, unsigned >::iterator it = group2id.find(groupName);
  if(it == group2id.end()) {
    group theGroup;
    theGroup.name = groupName;

    group2id.insert(make_pair(theGroup.name, groups.size()));
    groups.push_back(theGroup);

    return &groups[groups.size() - 1];
  }
  else
    return &groups[it->second];
}

/* generic helper functions */
bool opendp::is_special_char(char c) {
  static const char specialChars[] = {'(', ')', ',', ':', ';', '/',  '#',
                                      '[', ']', '{', '}', '*', '\"', '\\'};

  for(unsigned i = 0; i < sizeof(specialChars); ++i) {
    if(c == specialChars[i]) return true;
  }

  return false;
}

bool opendp::read_line_as_tokens(istream &is, vector< string > &tokens) {
  tokens.clear();

  string line;
  getline(is, line);

  while(is && tokens.empty()) {
    string token = "";
    for(unsigned i = 0; i < line.size(); ++i) {
      char currChar = line[i];
      if(isspace(currChar) || is_special_char(currChar)) {
        if(!token.empty()) {
          // Add the current token to the list of tokens
          tokens.push_back(token);
          token.clear();
        }
        // else if the token is empty, simply skip the whitespace or special
        // char
      }
      else {
        // Add the char to the current token
        token.push_back(currChar);
      }
    }

    if(!token.empty()) tokens.push_back(token);

    if(tokens.empty())
      // Previous line read was empty. Read the next one.
      getline(is, line);
  }

  return !tokens.empty();
}

void opendp::get_next_token(ifstream &is, string &token, const char *beginComment) {
  do {
    is >> token;
    if(!strcmp(token.substr(0, strlen(beginComment)).c_str(), beginComment)) {
      getline(is, token);
      token = "";
    }
  } while(!is.eof() && (token.empty() || isspace(token[0])));
}

void opendp::get_next_n_tokens(ifstream &is, vector< string > &tokens,
                       const unsigned numTokens, const char *beginComment) {
  tokens.clear();
  string token;
  unsigned count = 0;
  do {
    is >> token;
    if(!strcmp(token.substr(0, strlen(beginComment)).c_str(), beginComment)) {
      getline(is, token);
      token = "";
    }
    if(!token.empty() && !isspace(token[0])) {
      tokens.push_back(token);
      ++count;
    }
  } while(!is.eof() && count < numTokens);
}

void pin::print() {
  cout << "|=== BEGIN PIN ===|  " << endl;
  cout << "name:                " << name << endl;
  cout << "id:                  " << id << endl;
  cout << "type:                " << type << endl;
  cout << "net:                 " << net << endl;
  cout << "pin owner:           " << owner << endl;
  cout << "isFixed?             " << (isFixed ? "yes" : "no") << endl;
  cout << "(x_coord,y_coord):   " << x_coord << ", " << y_coord << endl;
  cout << "(x_offset,y_offset): " << x_offset << ", " << y_offset << endl;
  cout << "|===  END  PIN ===|  " << endl;
}

void macro::print() {
  cout << "|=== BEGIN MACRO ===|" << endl;
  cout << "name:                " << name << endl;
  cout << "type:                " << type << endl;
  cout << "(xOrig,yOrig):       " << xOrig << ", " << yOrig << endl;
  cout << "[width,height]:      " << width << ", " << height << endl;
  for(unsigned i = 0; i < sites.size(); ++i) {
    cout << "sites[" << i << "]: " << sites[i] << endl;
  }
  for(OPENDP_HASH_MAP< string, macro_pin >::iterator it = pins.begin();
      it != pins.end(); it++) {
    cout << "pins: " << (*it).first << endl;
  }
  cout << "|=== BEGIN MACRO ===|" << endl;
}

void cell::print() {
  cout << "|=== BEGIN CELL ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "type:               " << type << endl;
  cout << "orient:             " << cellorient << endl;
  cout << "isFixed?            " << (isFixed ? "true" : "false") << endl;
  for(OPENDP_HASH_MAP< string, unsigned >::iterator it = ports.begin();
      it != ports.end(); it++)
    cout << "port: " << (*it).first << " - " << (*it).second << endl;
  cout << "(init_x,  init_y):  " << init_x_coord << ", " << init_y_coord
       << endl;
  cout << "(x_coord,y_coord):  " << x_coord << ", " << y_coord << endl;
  cout << "[width,height]:      " << width << ", " << height << endl;
  cout << "|===  END  CELL ===|" << endl;
}

void net::print() {
  cout << "|=== BEGIN NET ===|" << endl;
  cout << "name:              " << name << endl;
  cout << "source pin:        " << source << endl;
  for(unsigned i = 0; i < sinks.size(); ++i)
    cout << "sink pin [" << i << "]:      " << sinks[i] << endl;
  cout << "|===  END  NET ===|" << endl;
}

void row::print() {
  cout << "|=== BEGIN ROW ===|" << endl;
  cout << "name:              " << name << endl;
  cout << "site:              " << site << endl;
  cout << "(origX,origY):     " << origX << ", " << origY << endl;
  cout << "(stepX,stepY):     " << stepX << ", " << stepY << endl;
  cout << "numSites:          " << numSites << endl;
  cout << "orientation:       " << siteorient << endl;
  cout << "|===  END  ROW ===|" << endl;
}

void site::print() {
  cout << "|=== BEGIN SITE ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "width:              " << width << endl;
  cout << "height:             " << height << endl;
  cout << "type:               " << type << endl;
  for(vector< string >::iterator it = symmetries.begin();
      it != symmetries.end(); ++it)
    cout << "symmetries:         " << *it << endl;
  cout << "|===  END  SITE ===|" << endl;
}

void layer::print() {
  cout << "|=== BEGIN LAYER ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "type:               " << type << endl;
  cout << "direction:          " << direction << endl;
  cout << "[xPitch,yPitch]:    " << xPitch << ", " << yPitch << endl;
  cout << "[xOffset,yOffset]:  " << xOffset << ", " << yOffset << endl;
  cout << "width:              " << width << endl;
  cout << "|===  END  LAYER ===|" << endl;
}

void density_bin::print() {
  cout << "|=== BEGIN DENSITY_BIN ===|" << endl;
  cout << " area :        " << area << endl;
  cout << " m_util :      " << m_util << endl;
  cout << " f_util :      " << f_util << endl;
  cout << " free_space :  " << free_space << endl;
  cout << " overflow :    " << overflow << endl;
  cout << " density limit:" << density_limit << endl;
  cout << "|===  END  DENSITY_BIN ===|" << endl;
}

void circuit::print() {
  for(unsigned i = 0; i < layers.size(); ++i) layers[i].print();
  for(unsigned i = 0; i < sites.size(); ++i) sites[i].print();
  for(unsigned i = 0; i < rows.size(); ++i) rows[i].print();
  for(unsigned i = 0; i < pins.size(); ++i) pins[i].print();
  for(unsigned i = 0; i < cells.size(); ++i) cells[i].print();
  for(unsigned i = 0; i < nets.size(); ++i) nets[i].print();
}
