////////////////////////////////////////////////////////////////////////////////
// Realtimify - A small tool for bringing SystemC simulations in synchronization
//              with real time.
// 
// Copyright (c) 2003-2005 by Digital Force / Mario Trams 
//                            Mario.Trams@digital-force.net
//                            http://www.digital-force.net
//
// File    : realtimify.cpp
// Purpose : The implementation of the Realtimify module.
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

#include <unistd.h>
#include <sys/time.h>
#include "systemc.h"
#include "realtimify.h"

//#define DEBUG

//
// Realtimify module constructor - checking interval given as double/sc_time_unit pair
//
realtimify::realtimify(sc_module_name name_, double interval_time, sc_time_unit interval_unit): 
  sc_module(name_) {

  // Convert the double/sc_time_unit pair into an sc_time object.
  interval = sc_time(interval_time, interval_unit);

  // Call the actual initialization function.
  init();

}

//
// Realtimify module constructor - checking interval given as single sc_time object 
//
realtimify::realtimify(sc_module_name name_, sc_time interval_): 
  sc_module(name_), interval(interval_) {

  // Call the actual initialization function.
  init();

}

//
// The actual initialization function...
//
void realtimify::init() {

  // In case the specified interval for performing a real time check and bringing
  // the application in sync (if needed) is smaller then one microsecond, we issue
  // a warning. 
  if (interval.to_seconds() < 1e-6) {
    cerr << "Realtimify Warning: The specified Realtimify interval (" << interval << ")" << endl;
    cerr << "                    is below micro second resolution (proceeding anyways)!" << endl;
  }

  // Register the actual Realtimify SystemC process (as SC_METHOD). The method is 
  // self-timed (see below) and is not sensitive to any signal.
  SC_METHOD( process );

  // Later, start_real_time_usecs holds the real time when the simulation has been
  // started. A value of zero means that this starting time has not yet been taken.
  // Note: We do not take the starting time right now, i.e. during the elaboration
  //       phase. Instead, it is taken at the first simulation cycle. This is, because
  //       depending on the simulation model the elaboration phase might take a 
  //       significant amount of time. 
  start_real_time_usecs = 0;

  // Get the default time resolution and convert it to micro seconds.
  // This is needed, because we have to qualify the result of sc_simulation_time(),
  // as this is unit-less.
  sc_time default_time = sc_get_default_time_unit();
  double default_time_usecs = default_time.to_seconds() * 1000000;

  // Determine the scale value used to convert default time units into micro seconds.
  // Note: This is not done by a simple division as this can introduce significant 
  //       rounding errors. Instead, we do trial&error ...
  // Note: A check for "< 0.5" means actually "< 1" and a check for "> 5" means 
  //       actually "> 1". This is because due to rounding issues the value that
  //       is being checked might never have a value of exactly 1 (e.g. 0.999999...
  //       or 1.000000000...1).  

  scale = 1;
  if (default_time_usecs <= 1) {
    while ( (scale * default_time_usecs) < 0.5 ) {
      scale *= 10;
    }
  } else {
    while ( (scale * default_time_usecs) > 5 ) {
      scale /= 10;
    }
  }

  return;
  
}

//
// The process (SC_METHOD) that is regularly synchronizing simulation time and 
// spent real time.
//
void realtimify::process() {

//  unsigned long long current_time_usecs;
  struct timeval tv;
  unsigned long long current_real_time_usecs;
  double current_simulation_time_usecs;
  long long time_difference_usecs;

  // Determine the current time in micro seconds.
  gettimeofday( &tv, NULL );
  current_real_time_usecs = (1000000 * tv.tv_sec + tv.tv_usec) - start_real_time_usecs;

  if ( !start_real_time_usecs ) {
    // This is the very first invocation of Realtimify. So we take the current 
    // real time which becomes the starting time.
    start_real_time_usecs = current_real_time_usecs;
  } else {
    // This is not the very first invocation. So we have to see whether there
    // have to be taken some actions.

    // First determine the current simulation time in micro seconds.
    current_simulation_time_usecs = sc_simulation_time() / scale;

    // Next calculate the difference between simulation and real time. 
    time_difference_usecs = (long long) (current_simulation_time_usecs - current_real_time_usecs);

    if ( time_difference_usecs >= 0 ) {
      // When the difference is positive this means the simulation time is ahead
      // of the real time. In this case we have to issue an according usleep() call.
      usleep( time_difference_usecs );
    } else {
      // When the difference is negative this means the simulation time is behind
      // the real time. In other words, the model can't be simulated in real time.
      // Well, in this case we can't do much from here....
#ifdef DEBUG
      cerr << "Realtimify Warning: Real time violation!" << endl;
#endif
    }

  }

  // Schedule the next synchronization task according the application-specific
  // setting.
  next_trigger(interval);

  return;

}
