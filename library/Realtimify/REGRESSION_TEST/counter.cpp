////////////////////////////////////////////////////////////////////////////////
// Realtimify - A small tool for bringing SystemC simulations in synchronization
//              with real time.
// 
// Copyright (c) 2003-2005 by Digital Force / Mario Trams 
//                            Mario.Trams@digital-force.net
//                            http://www.digital-force.net
//
// File    : counter.cpp
// Purpose : The implementation file of a simple clocked counter module.
// History : - February 13th 2005:
//             Initial Release 0.1 
//
// Realtimify is free software; you can redistribute it and/or modify it under 
// the terms of the GNU Lesser General Public License as published by the Free 
// Software Foundation; either version 2.1 of the License, or (at your option) 
// any later version.
//
// Realtimify is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
// A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
// details.
//
// You should have received a copy of the GNU Lesser General Public License 
// along with this library; if not, write to the Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
////////////////////////////////////////////////////////////////////////////////
#include <time.h>

#include "systemc.h"
#include "counter.h"

//
// The constructor of the counter module.
//
counter::counter(sc_module_name name_): sc_module(name_) {

  SC_METHOD( process );
  sensitive_pos << clk;

  content = 0;

}

//
// The counter process.
//
void counter::process() {

  // Write out the current counter value.
  cout << sc_simulation_time() << ": " << content << " || " << time(NULL) << " : " << sc_time_stamp() << endl;

  // Increase the counter value.
  content++;

  return;

}
