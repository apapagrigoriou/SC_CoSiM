////////////////////////////////////////////////////////////////////////////////
// Realtimify - A small tool for bringing SystemC simulations in synchronization
//              with real time.
// 
// Copyright (c) 2003-2005 by Digital Force / Mario Trams 
//                            Mario.Trams@digital-force.net
//                            http://www.digital-force.net
//
// File    : main.cpp
// Purpose : Some example code that makes use of the Realtimify module.
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

#include "systemc.h"
#include "counter.h"
#include "realtimify.h"

int sc_main(int ac, char *av[])
{

  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

  // Set some SystemC time parameters (not necessarily needed).  
  sc_set_default_time_unit(1.0, SC_SEC);
  sc_set_time_resolution(1.0, SC_US);

  // Our clock cycle (1s for 1Hz).
  sc_time clock_cycle( 1.0, SC_SEC ); 

  // Create a clock (50% duty cycle, cycle according clock_cycle).
  sc_clock clk("clk", clock_cycle, 0.5, clock_cycle);

  // Instantiate and connect the counter component.
  counter CNT("counter");
  CNT.clk(clk);

  // Instantiate the Realtimify module. The check/synchronize interval can
  // be specified as either double/sc_time_unit pair or as sc_time_object.
  realtimify REALTIME("realtimify", 1.0, SC_SEC);
  // Alternatively, and with the same functionality for this example:
  // realtimify REALTIME("realtimify", clock_cycle);

  // start the simulation
  sc_start(10,SC_SEC); 

  return 0;
}

