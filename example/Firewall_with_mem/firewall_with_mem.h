/*
CoSiM Firewall Module in SystemC with external Memory 
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
#ifndef FIREWALL_WITH_MEM_H_INCLUDED
#define FIREWALL_WITH_MEM_H_INCLUDED

#include <stdint.h>
#include <systemc>
#include <tlm>

#include <sysc/kernel/sc_spawn.h>
#include <sysc/kernel/sc_boost.h>
#include <tlm_utils/simple_target_socket.h>
using namespace tlm;

#include "mem_bus_if.h"
#include "fw_packet.h"


#define INTERNAL_FIFO_SIZE 10
#define _BYTE_WIDTH 8

#define ACCESS_PORT_1_BASE_ADDR 0x80000000
#define ACCESS_PORT_2_BASE_ADDR 0x80001000
#define ACCESS_PORT_3_BASE_ADDR 0x80002000
#define ACCESS_PORT_4_BASE_ADDR 0x80003000
#define SETUP_BASE_ADDR 0x50000000
#define DEBUG_BASE_ADDR 0x50001000


#define D_W_P_0   0x80000014 // Write Packet to Port 0
#define D_W_P_1   0x80001014 // Write Packet to Port 1
#define D_W_P_2   0x80002014 // Write Packet to Port 2
#define D_W_P_3   0x80003014 // Write Packet to Port 3
#define D_R_P_0   0x8000001C // Read from Port 0
#define D_R_P_1   0x8000101C // Read from Port 1
#define D_R_P_2   0x8000201C // Read from Port 2
#define D_R_P_3   0x8000301C // Read from Port 3
#define D_T_P_0   0x80000024 // Read total packets passed from Port 0
#define D_T_P_1   0x80001024 // Read total packets passed from Port 1
#define D_T_P_2   0x80002024 // Read total packets passed from Port 2
#define D_T_P_3   0x80003024 // Read total packets passed from Port 3
#define D_D_P_0   0x8000002C // Read dropped packets (due to Fifo full error) from Port 0
#define D_D_P_1   0x8000102C // Read dropped packets (due to Fifo full error) from Port 1
#define D_D_P_2   0x8000202C // Read dropped packets (due to Fifo full error) from Port 2
#define D_D_P_3   0x8000302C // Read dropped packets (due to Fifo full error) from Port 3
#define D_X_P_0   0x80000034 // Read droped packets (due firewall rule) from Port 0
#define D_X_P_1   0x80001034 // Read droped packets (due firewall rule) from Port 1
#define D_X_P_2   0x80002034 // Read droped packets (due firewall rule) from Port 2
#define D_X_P_3   0x80003034 // Read droped packets (due firewall rule) from Port 3
#define F_L_P_0   0x50000014 // Setup Firewall Lower Address Range Register for port 0
#define F_L_P_1   0x5000001C // Setup Firewall Lower Address Range Register for port 1
#define F_L_P_2   0x50000024 // Setup Firewall Lower Address Range Register for port 2
#define F_L_P_3   0x5000002C // Setup Firewall Lower Address Range Register for port 3
#define F_U_P_0   0x50000034 // Setup Firewall Upper Address Range Register for port 0
#define F_U_P_1   0x5000003C // Setup Firewall Upper Address Range Register for port 1
#define F_U_P_2   0x50000044 // Setup Firewall Upper Address Range Register for port 2
#define F_U_P_3   0x5000004C // Setup Firewall Upper Address Range Register for port 3
#define F_R_P_0   0x50000054 // Setup Firewall Rule Register for port 0
#define F_R_P_1   0x5000005C // Setup Firewall Rule Register for port 1
#define F_R_P_2   0x50000064 // Setup Firewall Rule Register for port 2
#define F_R_P_3   0x5000006C // Setup Firewall Rule Register for port 3
#define D_M_R_0   0x40000000 // Read from Port 0
#define D_M_R_1   0x40001000 // Read from Port 1
#define D_M_R_2   0x40002000 // Read from Port 2
#define D_M_R_3   0x40003000 // Read from Port 3
#define D_M_R_M   0x40004000 // MAX ADDRESS for Direct BRAM ACCESS

SC_MODULE (firewall_with_mem) {

  //tlm interface
  tlm_utils::simple_target_socket<firewall_with_mem> m_slave_socket;
	
  //Memory Interface
  mem_bus_port<sc_uint<32> >  out_0;
  mem_bus_port<sc_uint<32> >  out_1;
  mem_bus_port<sc_uint<32> >  out_2;
  mem_bus_port<sc_uint<32> >  out_3;

  //Clock
  sc_in <bool>      CLK;
  //reset
  sc_in<bool>       reset;


  sc_out<unsigned int>  out_addr_to_read_0;
  sc_out<unsigned int>  out_addr_to_read_1;
  sc_out<unsigned int>  out_addr_to_read_2;
  sc_out<unsigned int>  out_addr_to_read_3;
  sc_in<bool>       done_0;
  sc_in<bool>       done_1;
  sc_in<bool>       done_2;
  sc_in<bool>       done_3;
  sc_out<bool>      ready_0;
  sc_out<bool>      ready_1;
  sc_out<bool>      ready_2;
  sc_out<bool>      ready_3;
  bool preready[4];
  sc_out<unsigned int>  dir_mem_out_addr_to_read_0;
  sc_out<unsigned int>  dir_mem_out_addr_to_read_1;
  sc_out<unsigned int>  dir_mem_out_addr_to_read_2;
  sc_out<unsigned int>  dir_mem_out_addr_to_read_3;
  sc_in<bool>       dir_mem_done_0;
  sc_in<bool>       dir_mem_done_1;
  sc_in<bool>       dir_mem_done_2;
  sc_in<bool>       dir_mem_done_3;
  sc_out<bool>      dir_mem_ready_0;
  sc_out<bool>      dir_mem_ready_1;
  sc_out<bool>      dir_mem_ready_2;
  sc_out<bool>      dir_mem_ready_3;
  bool dir_mem_preready[4];
  
  uint32_t  m_register_read;
  uint32_t  m_register_write;
  
  sc_dt::uint64 m_read_address;
  sc_dt::uint64 m_write_address;

  //SW Inside Fifos/ports
  sc_fifo<sc_uint<32> > in_fifo_0;
  sc_fifo<sc_uint<32> > in_fifo_1;
  sc_fifo<sc_uint<32> > in_fifo_2;
  sc_fifo<sc_uint<32> > in_fifo_3;
  sc_fifo<sc_uint<32> > out_fifo_s1_0;
  sc_fifo<sc_uint<32> > out_fifo_s1_1;
  sc_fifo<sc_uint<32> > out_fifo_s2_0;
  sc_fifo<sc_uint<32> > out_fifo_s2_1;
  sc_fifo<sc_uint<32> > out_fifo_s3_0;
  sc_fifo<sc_uint<32> > out_fifo_s3_1;
  sc_fifo<sc_uint<32> > out_fifo_s4_0;
  sc_fifo<sc_uint<32> > out_fifo_s4_1;
  sc_fifo<sc_uint<32> > out_fifo_0;
  sc_fifo<sc_uint<32> > out_fifo_1;
  sc_fifo<sc_uint<32> > out_fifo_2;
  sc_fifo<sc_uint<32> > out_fifo_3;
  sc_fifo<sc_uint<32> > responce_fifo_0;
  sc_fifo<sc_uint<32> > responce_fifo_1;
  sc_fifo<sc_uint<32> > responce_fifo_2;
  sc_fifo<sc_uint<32> > responce_fifo_3;
  sc_fifo<sc_uint<32> > init_responce_fifo_0;
  sc_fifo<sc_uint<32> > init_responce_fifo_1;
  sc_fifo<sc_uint<32> > init_responce_fifo_2;
  sc_fifo<sc_uint<32> > init_responce_fifo_3;

  sc_fifo<sc_uint<32> > dir_mem_out_fifo_0;
  sc_fifo<sc_uint<32> > dir_mem_out_fifo_1;
  sc_fifo<sc_uint<32> > dir_mem_out_fifo_2;
  sc_fifo<sc_uint<32> > dir_mem_out_fifo_3;
  sc_fifo<sc_uint<32> > dir_mem_responce_fifo_0;
  sc_fifo<sc_uint<32> > dir_mem_responce_fifo_1;
  sc_fifo<sc_uint<32> > dir_mem_responce_fifo_2;
  sc_fifo<sc_uint<32> > dir_mem_responce_fifo_3;


  //SW In/Out Register  Ports
  sc_fifo<sc_uint<32> > in_0;
  sc_fifo<sc_uint<32> > in_1;
  sc_fifo<sc_uint<32> > in_2;
  sc_fifo<sc_uint<32> > in_3;
  sc_signal<sc_uint<32> >  in_responce_0;
  sc_signal<sc_uint<32> >  in_responce_1;
  sc_signal<sc_uint<32> >  in_responce_2;
  sc_signal<sc_uint<32> >  in_responce_3;
  //firewall setup interface
  sc_signal<sc_uint<32> >   setup_lower_range_0;
  sc_signal<sc_uint<32> >   setup_lower_range_1;
  sc_signal<sc_uint<32> >   setup_lower_range_2;
  sc_signal<sc_uint<32> >   setup_lower_range_3;
  sc_signal<sc_uint<32> >   setup_upper_range_0;
  sc_signal<sc_uint<32> >   setup_upper_range_1;
  sc_signal<sc_uint<32> >   setup_upper_range_2;
  sc_signal<sc_uint<32> >   setup_upper_range_3;
  sc_signal<sc_uint<32> >   setup_rule_range_0;
  sc_signal<sc_uint<32> >   setup_rule_range_1;
  sc_signal<sc_uint<32> >   setup_rule_range_2;
  sc_signal<sc_uint<32> >   setup_rule_range_3;
  //stats interface
  sc_signal<unsigned int>   total_0;
  sc_signal<unsigned int>   total_1;
  sc_signal<unsigned int>   total_2;
  sc_signal<unsigned int>   total_3;
  sc_signal<unsigned int>   droped_0;
  sc_signal<unsigned int>   droped_1;
  sc_signal<unsigned int>   droped_2;
  sc_signal<unsigned int>   droped_3;
  sc_signal<unsigned int>   fw_rule_hit_counter_0;
  sc_signal<unsigned int>   fw_rule_hit_counter_1;
  sc_signal<unsigned int>   fw_rule_hit_counter_2;
  sc_signal<unsigned int>   fw_rule_hit_counter_3;
  sc_signal<sc_uint<32> >   fw_hreg_p_0;
  sc_signal<sc_uint<32> >   fw_hreg_p_1;
  sc_signal<sc_uint<32> >   fw_hreg_p_2;
  sc_signal<sc_uint<32> >   fw_hreg_p_3;
  sc_signal<sc_uint<32> >   fw_lreg_p_0;
  sc_signal<sc_uint<32> >   fw_lreg_p_1;
  sc_signal<sc_uint<32> >   fw_lreg_p_2;
  sc_signal<sc_uint<32> >   fw_lreg_p_3;
  sc_signal<sc_uint<32> >   fw_rreg_p_0;
  sc_signal<sc_uint<32> >   fw_rreg_p_1;
  sc_signal<sc_uint<32> >   fw_rreg_p_2;
  sc_signal<sc_uint<32> >   fw_rreg_p_3;

  
  //internal used variables
  sc_signal<sc_uint<32> > responce_s0;
  sc_signal<sc_uint<32> > responce_s1;
  sc_signal<sc_uint<32> > responce_s2;
  sc_signal<sc_uint<32> > responce_s3;
  sc_uint<32> responce0;
  sc_uint<32> responce1;
  sc_uint<32> responce2;
  sc_uint<32> responce3;
  sc_signal<sc_uint<32> > fw_lower_range_0;
  sc_signal<sc_uint<32> > fw_lower_range_1;
  sc_signal<sc_uint<32> > fw_lower_range_2;
  sc_signal<sc_uint<32> > fw_lower_range_3;
  sc_signal<sc_uint<32> > fw_upper_range_0;
  sc_signal<sc_uint<32> > fw_upper_range_1;
  sc_signal<sc_uint<32> > fw_upper_range_2;
  sc_signal<sc_uint<32> > fw_upper_range_3;
  sc_signal<sc_uint<32> > fw_rule_range_0;
  sc_signal<sc_uint<32> > fw_rule_range_1;
  sc_signal<sc_uint<32> > fw_rule_range_2;
  sc_signal<sc_uint<32> > fw_rule_range_3;
  sc_uint<32> count_total_0;
  sc_uint<32> count_total_1;
  sc_uint<32> count_total_2;
  sc_uint<32> count_total_3;
  sc_uint<32> count_droped_0;
  sc_uint<32> count_droped_1;
  sc_uint<32> count_droped_2;
  sc_uint<32> count_droped_3;
  sc_uint<32> rule_hit_counter_0;
  sc_uint<32> rule_hit_counter_1;
  sc_uint<32> rule_hit_counter_2;
  sc_uint<32> rule_hit_counter_3;

  sc_uint<32> out_address_0;
  sc_uint<32> out_address_1;
  sc_uint<32> out_address_2;
  sc_uint<32> out_address_3;
  bool out_read_0;
  bool out_read_1;
  bool out_read_2;
  bool out_read_3;
  
  // TLM Events
  sc_core::sc_event m_write_event;  
  sc_core::sc_event m_read_event;  
  sc_core::sc_event m_read_event_pending;  
  sc_core::sc_event m_write_event_pending;  
  sc_core::sc_event m_write_ended_event;  
  sc_core::sc_event m_read_ended_event;  

  // TLM Processes
  void execute_write_thread(void);
  void execute_read_thread(void);
  void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
  unsigned transport_dbg(tlm::tlm_generic_payload& trans);

  //input handler method
  void interface_in_0();
  void interface_in_1();
  void interface_in_2();
  void interface_in_3();
  void interface_out_0();
  void interface_out_1();
  void interface_out_2();
  void interface_out_3();
  void entry_sw_1();
  void entry_sw_2();
  void entry_sw_3();
  void entry_sw_4();
  void entry_response();
  void setupfw_rule();
  void direct_mem_out_0();
  void direct_mem_out_1();
  void direct_mem_out_2();
  void direct_mem_out_3();

  //Constructor
  SC_CTOR(firewall_with_mem)
  :out_0 ("out_0"), out_1 ("out_1"), out_2 ("out_2"), out_3 ("out_3"),
   in_fifo_0(INTERNAL_FIFO_SIZE),in_fifo_1(INTERNAL_FIFO_SIZE),
   in_fifo_2(INTERNAL_FIFO_SIZE),in_fifo_3(INTERNAL_FIFO_SIZE),
   out_fifo_s1_0(INTERNAL_FIFO_SIZE),out_fifo_s1_1(INTERNAL_FIFO_SIZE),
   out_fifo_s2_0(INTERNAL_FIFO_SIZE),out_fifo_s2_1(INTERNAL_FIFO_SIZE),
   out_fifo_s3_0(INTERNAL_FIFO_SIZE),out_fifo_s3_1(INTERNAL_FIFO_SIZE),
   out_fifo_s4_0(INTERNAL_FIFO_SIZE),out_fifo_s4_1(INTERNAL_FIFO_SIZE),
   out_fifo_0(INTERNAL_FIFO_SIZE),out_fifo_1(INTERNAL_FIFO_SIZE),
   out_fifo_2(INTERNAL_FIFO_SIZE),out_fifo_3(INTERNAL_FIFO_SIZE),
   responce_fifo_0(INTERNAL_FIFO_SIZE),responce_fifo_1(INTERNAL_FIFO_SIZE),
   responce_fifo_2(INTERNAL_FIFO_SIZE),responce_fifo_3(INTERNAL_FIFO_SIZE),
   init_responce_fifo_0(INTERNAL_FIFO_SIZE),init_responce_fifo_1(INTERNAL_FIFO_SIZE),
   init_responce_fifo_2(INTERNAL_FIFO_SIZE),init_responce_fifo_3(INTERNAL_FIFO_SIZE),
   dir_mem_out_fifo_0(INTERNAL_FIFO_SIZE),dir_mem_out_fifo_1(INTERNAL_FIFO_SIZE),
   dir_mem_out_fifo_2(INTERNAL_FIFO_SIZE),dir_mem_out_fifo_3(INTERNAL_FIFO_SIZE),
   dir_mem_responce_fifo_0(INTERNAL_FIFO_SIZE),dir_mem_responce_fifo_1(INTERNAL_FIFO_SIZE),
   dir_mem_responce_fifo_2(INTERNAL_FIFO_SIZE),dir_mem_responce_fifo_3(INTERNAL_FIFO_SIZE)
  {
  m_slave_socket.register_b_transport(this, &firewall_with_mem::b_transport);
  m_slave_socket.register_transport_dbg(this, &firewall_with_mem::transport_dbg);
  SC_THREAD(execute_write_thread);
  SC_THREAD(execute_read_thread);

  SC_CTHREAD(interface_in_0, CLK.pos());
  SC_CTHREAD(interface_in_1, CLK.pos());
  SC_CTHREAD(interface_in_2, CLK.pos());
  SC_CTHREAD(interface_in_3, CLK.pos());
  SC_CTHREAD(setupfw_rule, CLK.pos());
  SC_CTHREAD(interface_out_0, CLK.pos());
  SC_CTHREAD(interface_out_1, CLK.pos());
  SC_CTHREAD(interface_out_2, CLK.pos());
  SC_CTHREAD(interface_out_3, CLK.pos());
  SC_CTHREAD(entry_sw_1, CLK.pos());
  SC_CTHREAD(entry_sw_2, CLK.pos());
  SC_CTHREAD(entry_sw_3, CLK.pos());
  SC_CTHREAD(entry_sw_4, CLK.pos());
  SC_CTHREAD(entry_response, CLK.pos());
  SC_CTHREAD(direct_mem_out_0, CLK.pos());
  SC_CTHREAD(direct_mem_out_1, CLK.pos());
  SC_CTHREAD(direct_mem_out_2, CLK.pos());
  SC_CTHREAD(direct_mem_out_3, CLK.pos());
  
  }

};

#endif //FIREWALL_WITH_MEM_H_INCLUDED
