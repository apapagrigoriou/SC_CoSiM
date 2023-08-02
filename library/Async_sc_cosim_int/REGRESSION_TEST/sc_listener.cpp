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
#include <iomanip>

listener::listener(sc_module_name instance_name, uint64_t register_count, sc_core::sc_time latency): sc_module(instance_name), m_slave_socket("slave_port")
{
  m_register_count = register_count;
  m_byte_width = m_slave_socket.get_bus_width()/8;
  m_register = new int[m_register_count];
  m_latency = latency;
  // Register methods
  m_slave_socket.register_b_transport(this, &listener::b_transport);
  m_slave_socket.register_transport_dbg(this, &listener::transport_dbg);
  // Register processes
  SC_HAS_PROCESS(listener);
  SC_THREAD(execute_write_thread);
  SC_THREAD(execute_read_thread);
  cout << name() << ":: "<< __func__ << endl;
}

listener::~listener(void)
{
  delete [] m_register;
  cout << name() << ":: "<< __func__ << endl;
}

void listener::execute_write_thread(void)
{
  while(true) {
    wait(m_register_write_event); //wait for write event to execute code
    //do here whatever we want
    cout << name() << ": HERE HANDLING Write CODE: "<< __func__ << endl;
  }
}

void listener::execute_read_thread(void)
{
  while(true) {
    wait(m_register_read_event); //wait for write event to execute code
    //do here whatever we want
    cout << name() << ": HERE HANDLING Read CODE: "<< __func__ << endl;
  }
}

void listener::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
  tlm::tlm_command command = trans.get_command();
  sc_dt::uint64 address = trans.get_address();
  unsigned char* data_ptr = trans.get_data_ptr();
  unsigned int data_length = trans.get_data_length();
  unsigned char* byte_enables = trans.get_byte_enable_ptr();
  unsigned int streaming_width = trans.get_streaming_width();

  cout << "data_length: " << data_length << " |m_byte_width: " << m_byte_width << " |streaming_width: " << streaming_width << " |address: " << address << " |m_byte_width: " << m_byte_width << " |m_register_count: " << m_register_count << endl;

  if (address >= sc_dt::uint64(m_register_count) * m_byte_width) {
    SC_REPORT_ERROR("LISTENER","ALERT: Out of Bound Address request : 0x");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
    return;
  } else if (address % m_byte_width) {
    SC_REPORT_ERROR("LISTENER","ALERT: Misaligned address: 0x");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
    return;
  } else if (byte_enables != 0) {
    SC_REPORT_ERROR("LISTENER","ALERT: No support for byte enables");
    trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
    return;
  } else if ((data_length % m_byte_width) != 0 || streaming_width < data_length || data_length == 0
      || (address+data_length)/m_byte_width > sc_dt::uint64(m_register_count)) {
    SC_REPORT_ERROR("LISTENER","ALERT: Only allow word-multiple transfers within memory size");
    trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
    return;
  }

  
  //implementing read and write commands
  if ( command == tlm::TLM_READ_COMMAND ) {
    memcpy((void*) data_ptr, (void*) &m_register[address/m_byte_width], data_length);
    m_register_read_event.notify();
  } else if ( command == tlm::TLM_WRITE_COMMAND ) {
    memcpy((void*) &m_register[address/m_byte_width], (void*) data_ptr, data_length);
    m_register_write_event.notify();
  }

  // Memory access time per bus value
  delay += (m_latency * data_length/m_byte_width);

  // Obliged to set response status to indicate successful completion
  trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

unsigned int listener::transport_dbg(tlm::tlm_generic_payload& trans)
{
  int              transferred = 0;
  tlm::tlm_command command = trans.get_command();
  sc_dt::uint64    address = trans.get_address();
  //unsigned char*   data_ptr = trans.get_data_ptr();
  unsigned int     data_length = trans.get_data_length();

  if (address >= sc_dt::uint64(m_register_count) * m_byte_width) {
    SC_REPORT_ERROR("LISTENER","ALERT: Out of Bound Address request : 0x");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
    return 0;
  } else if (address % m_byte_width) {
    SC_REPORT_ERROR("LISTENER","ALERT: Misaligned address: 0x");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
    return 0;
  }

  //implementing read and write commands
  if ( command == tlm::TLM_READ_COMMAND ) {
//    memcpy((void*) data_ptr, (void*) &m_register[address/m_byte_width], data_length);
    transferred = data_length;
  } else if ( command == tlm::TLM_WRITE_COMMAND ) {
//    memcpy((void*) &m_register[address/m_byte_width], (void*) data_ptr, data_length);
    transferred = data_length;
  }

  // Obliged to set response status to indicate successful completion
  trans.set_response_status( tlm::TLM_OK_RESPONSE );
  return transferred;
}

