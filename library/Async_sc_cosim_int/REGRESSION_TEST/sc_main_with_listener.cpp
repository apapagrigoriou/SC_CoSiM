/*
CoSiM Interface Module
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
#include "sc_listener.h"
#include "async_sc_cosim_int.h"
using namespace sc_core;
using namespace std;

int sc_main ( int argc, char *argv[])
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

  // Set some SystemC time parameters (not necessarily needed).  
  sc_set_default_time_unit(1.0, SC_SEC);
  sc_set_time_resolution(1.0, SC_US);
  sc_clock clock("CLOCK1", 1, SC_SEC, 0.5, 0.0, SC_SEC);

  // Elaborate all modules
  async_sc_cosim_int* a_async_sc_cosim_int;
  try {
    a_async_sc_cosim_int = new async_sc_cosim_int("async_sc_cosim_int");
  } catch (std::exception& e) {
    SC_REPORT_ERROR("MAIN", (string(e.what())+" Please fix elaboration errors and retry.").c_str());
    return 1;
  } catch (...) {
    SC_REPORT_ERROR("MAIN", "Caught exception during elaboration");
    return 1;
  }
  listener* a_listener;
  try {
    a_listener = new listener("listener", 32, sc_time(10,SC_NS));
  } catch (std::exception& e) {
    SC_REPORT_ERROR("MAIN", (string(e.what())+" Please fix elaboration errors and retry.").c_str());
    return 1;
  } catch (...) {
    SC_REPORT_ERROR("MAIN", "Caught exception during elaboration");
    return 1;
  }
  //connect the ports
  a_async_sc_cosim_int->m_master_socket.bind(a_listener->m_slave_socket);
  a_async_sc_cosim_int->CLK(clock);
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
  //clear mem
  delete a_async_sc_cosim_int;
  delete a_listener;
  return (0);
}
