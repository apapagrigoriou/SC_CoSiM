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
#ifndef LISTENER_H
#define LISTENER_H

#include <systemc>
#include "tlm_utils/simple_target_socket.h"
#include <stdint.h>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

typedef int32_t Data_t;


class listener: sc_core::sc_module
{
public:
  // Ports
  tlm_utils::simple_target_socket<listener> m_slave_socket;
  // Constructor
  listener( sc_core::sc_module_name instance_name, uint64_t register_count = 16, sc_core::sc_time latency = sc_time(10,SC_NS) );
  // Destructor
  virtual ~listener(void);
  // SystemC Processes
  void execute_write_thread(void);
  void execute_read_thread(void);
  // TLM-2 forward methods
  void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
  unsigned transport_dbg(tlm::tlm_generic_payload& trans);
private:
  sc_core::sc_event m_register_write_event;  
  sc_core::sc_event m_register_read_event;  
  int m_register_count;
  sc_dt::uint64 m_byte_width;
  volatile Data_t*  m_register;
  sc_core::sc_time  m_latency;
};
#endif /*LISTENER_H*/
