////////////////////////////////////////////////////////////////////////////////
// Realtimify - A small tool for bringing SystemC simulations in synchronization
//              with real time.
// 
// Copyright (c) 2003-2005 by Digital Force / Mario Trams 
//                            Mario.Trams@digital-force.net
//                            http://www.digital-force.net
//
// File    : realtimify.h
// Purpose : The header file of the Realtimify module.
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

SC_MODULE( realtimify ) {

  // Declaration of public module constructors.
  SC_HAS_PROCESS( realtimify );
  realtimify(sc_module_name name_, double interval_time, sc_time_unit interval_unit);
  realtimify(sc_module_name name_, sc_time interval_);

 protected:

  // Declaration of the actual synchronization function.
  void process();

  // Some additional helper function...
  void init();

  // interval stores the synchronization interval (resolution)  
  sc_time interval;

  // start_real_time_usecs stores the real time when the simulation has been started 
  unsigned long long start_real_time_usecs;

  // scale stores a scalar used to convert the result of sc_simulation_time into
  // micro seconds
  double scale;
  
};
