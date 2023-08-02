/*
CoSiM Async Channel
Copyright (C) TEI Crete

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifdef _REALTIME
#include "systemc.h"
#endif

#include "async_module.h"
using namespace sc_core;
using namespace std;

int sc_main ( int argc, char *argv[])
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

  // Elaborate
  async_module* async_m;
  try {
    async_m = new async_module("async_module");
  } catch (std::exception& e) {
    SC_REPORT_ERROR("MAIN", (string(e.what())+" Please fix elaboration errors and retry.").c_str());
    return 1;
  } catch (...) {
    SC_REPORT_ERROR("MAIN", "Caught exception during elaboration");
    return 1;
  }

  // Simulate
  try {
    SC_REPORT_INFO("MAIN", "Starting kernal");
    sc_start();
    SC_REPORT_INFO("MAIN", "Exited kernal");
  } catch (std::exception& e) {
    SC_REPORT_WARNING("MAIN",(string("Caught exception ")+e.what()).c_str());
  } catch (...) {
    SC_REPORT_ERROR("MAIN","Caught exception during simulation.");
  }
  if (not sc_end_of_simulation_invoked()) {
    SC_REPORT_INFO("MAIN","ERROR: Simulation stopped without explicit sc_stop()");
    sc_stop();
  }
  delete async_m;
  return (0);
}
