/*
CoSiM Demo Using the Firewall Module as DuT 
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
#include "systemc.h"
#include "mem_bus_if.h"
#include "firewall_with_mem.h"
#include "async_sc_cosim_int.h"
#include "receiver.h"

using namespace sc_core;
using namespace std;

int sc_main ( int argc, char *argv[])
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions( SC_ID_LOGIC_X_TO_BOOL_, SC_LOG);
  sc_report_handler::set_actions( SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_, SC_LOG);

sc_signal<bool> pkt_out0_ready;
  sc_signal<bool> pkt_out1_ready;
  sc_signal<bool> pkt_out2_ready;
  sc_signal<bool> pkt_out3_ready;
  sc_signal<bool> pkt_out0_done;
  sc_signal<bool> pkt_out1_done;
  sc_signal<bool> pkt_out2_done;
  sc_signal<bool> pkt_out3_done;
  sc_signal<unsigned int> pkt_out_0_addr;
  sc_signal<unsigned int> pkt_out_1_addr;
  sc_signal<unsigned int> pkt_out_2_addr;
  sc_signal<unsigned int> pkt_out_3_addr;

  sc_signal<bool> direct_pkt_out0_ready;
  sc_signal<bool> direct_pkt_out1_ready;
  sc_signal<bool> direct_pkt_out2_ready;
  sc_signal<bool> direct_pkt_out3_ready;
  sc_signal<bool> direct_pkt_out0_done;
  sc_signal<bool> direct_pkt_out1_done;
  sc_signal<bool> direct_pkt_out2_done;
  sc_signal<bool> direct_pkt_out3_done;
  sc_signal<unsigned int> direct_pkt_out_0_addr;
  sc_signal<unsigned int> direct_pkt_out_1_addr;
  sc_signal<unsigned int> direct_pkt_out_2_addr;
  sc_signal<unsigned int> direct_pkt_out_3_addr;

  mem_bus_chn<sc_uint<32> > bus_mem0("bus_mem0",0x40000000,0x40000FFF);
  mem_bus_chn<sc_uint<32> > bus_mem1("bus_mem1",0x40001000,0x40001FFF);
  mem_bus_chn<sc_uint<32> > bus_mem2("bus_mem2",0x40002000,0x40002FFF);
  mem_bus_chn<sc_uint<32> > bus_mem3("bus_mem3",0x40003000,0x40003FFF);

  sc_signal<bool> pkt_sw_reset;

  sc_signal<unsigned int> pkt_out0p;
  sc_signal<unsigned int> pkt_out0d;
  sc_signal<unsigned int> pkt_out0f;
  sc_signal<unsigned int> pkt_out1p;
  sc_signal<unsigned int> pkt_out1d;
  sc_signal<unsigned int> pkt_out1f;
  sc_signal<unsigned int> pkt_out2p;
  sc_signal<unsigned int> pkt_out2d;
  sc_signal<unsigned int> pkt_out2f;
  sc_signal<unsigned int> pkt_out3p;
  sc_signal<unsigned int> pkt_out3d;
  sc_signal<unsigned int> pkt_out3f;
  sc_signal<sc_uint<32> > fw_debug_signal[12];

  sc_signal<sc_uint<32> > setup_upper_s[4];
  sc_signal<sc_uint<32> > setup_lower_s[4];
  sc_signal<sc_uint<32> > setup_rule_s[4];

  sc_signal<sc_int<4> > id0, id1, id2, id3;

  // Set some SystemC time parameters (not necessarily needed).  
  sc_set_default_time_unit(1.0, SC_SEC);
  sc_set_time_resolution(1.0, SC_US);
  sc_clock clock("CLOCK1", 1, SC_SEC, 0.5, 0.0, SC_SEC);

  async_sc_cosim_int a_async_sc_cosim_int("ASYNC_CHANNEL");
  a_async_sc_cosim_int.CLK(clock);
  
  
  firewall_with_mem firewall_router("FW_ROUTER");
  firewall_router.CLK(clock);
  firewall_router.out_0(bus_mem0);
  firewall_router.out_1(bus_mem1);
  firewall_router.out_2(bus_mem2);
  firewall_router.out_3(bus_mem3);
  firewall_router.out_addr_to_read_0(pkt_out_0_addr);
  firewall_router.out_addr_to_read_1(pkt_out_1_addr);
  firewall_router.out_addr_to_read_2(pkt_out_2_addr);
  firewall_router.out_addr_to_read_3(pkt_out_3_addr);
  firewall_router.done_0(pkt_out0_done);
  firewall_router.done_1(pkt_out1_done);
  firewall_router.done_2(pkt_out2_done);
  firewall_router.done_3(pkt_out3_done);
  firewall_router.ready_0(pkt_out0_ready);
  firewall_router.ready_1(pkt_out1_ready);
  firewall_router.ready_2(pkt_out2_ready);
  firewall_router.ready_3(pkt_out3_ready);
  firewall_router.dir_mem_out_addr_to_read_0(direct_pkt_out_0_addr);
  firewall_router.dir_mem_out_addr_to_read_1(direct_pkt_out_1_addr);
  firewall_router.dir_mem_out_addr_to_read_2(direct_pkt_out_2_addr);
  firewall_router.dir_mem_out_addr_to_read_3(direct_pkt_out_3_addr);
  firewall_router.dir_mem_done_0(direct_pkt_out0_done);
  firewall_router.dir_mem_done_1(direct_pkt_out1_done);
  firewall_router.dir_mem_done_2(direct_pkt_out2_done);
  firewall_router.dir_mem_done_3(direct_pkt_out3_done);
  firewall_router.dir_mem_ready_0(direct_pkt_out0_ready);
  firewall_router.dir_mem_ready_1(direct_pkt_out1_ready);
  firewall_router.dir_mem_ready_2(direct_pkt_out2_ready);
  firewall_router.dir_mem_ready_3(direct_pkt_out3_ready);  
  firewall_router.reset(pkt_sw_reset);

  //connect the int to firewall
  a_async_sc_cosim_int.m_master_socket.bind(firewall_router.m_slave_socket);

  //connect the memory to firewall
  receiver receiver0("RECEIVER0");
  receiver0.pkt_in(bus_mem0);
  receiver0.out_addr_to_read(pkt_out_0_addr);
  receiver0.sink_id(id0);
  receiver0.count(pkt_out0p);
  receiver0.dropped(pkt_out0d);
  receiver0.firewalled(pkt_out0f);
  receiver0.ready(pkt_out0_ready);
  receiver0.done(pkt_out0_done);
  receiver0.direct_out_addr_to_read(direct_pkt_out_0_addr);
  receiver0.direct_ready(direct_pkt_out0_ready);
  receiver0.direct_done(direct_pkt_out0_done);  
  receiver0.fw_hreg(fw_debug_signal[0]);
  receiver0.fw_lreg(fw_debug_signal[4]);
  receiver0.fw_rreg(fw_debug_signal[8]);
  receiver0.CLK(clock);

  receiver receiver1("RECEIVER1");
  receiver1.pkt_in(bus_mem1);
  receiver1.out_addr_to_read(pkt_out_1_addr);
  receiver1.sink_id(id1);
  receiver1.count(pkt_out1p);
  receiver1.dropped(pkt_out1d);
  receiver1.firewalled(pkt_out1f);
  receiver1.ready(pkt_out1_ready);
  receiver1.done(pkt_out1_done);
  receiver1.direct_out_addr_to_read(direct_pkt_out_1_addr);
  receiver1.direct_ready(direct_pkt_out1_ready);
  receiver1.direct_done(direct_pkt_out1_done);  
  receiver1.fw_hreg(fw_debug_signal[1]);
  receiver1.fw_lreg(fw_debug_signal[5]);
  receiver1.fw_rreg(fw_debug_signal[9]);
  receiver1.CLK(clock);

  receiver receiver2("RECEIVER2");
  receiver2.pkt_in(bus_mem2);
  receiver2.out_addr_to_read(pkt_out_2_addr);
  receiver2.sink_id(id2);
  receiver2.count(pkt_out2p);
  receiver2.dropped(pkt_out2d);
  receiver2.firewalled(pkt_out2f);
  receiver2.ready(pkt_out2_ready);
  receiver2.done(pkt_out2_done);
  receiver2.direct_out_addr_to_read(direct_pkt_out_2_addr);
  receiver2.direct_ready(direct_pkt_out2_ready);
  receiver2.direct_done(direct_pkt_out2_done);
  receiver2.fw_hreg(fw_debug_signal[2]);
  receiver2.fw_lreg(fw_debug_signal[6]);
  receiver2.fw_rreg(fw_debug_signal[10]);
  receiver2.CLK(clock);

  receiver receiver3("RECEIVER3");
  // hooking up signals to ports by position
  receiver3.pkt_in(bus_mem3);
  receiver3.out_addr_to_read(pkt_out_3_addr);
  receiver3.sink_id(id3);
  receiver3.count(pkt_out3p);
  receiver3.dropped(pkt_out3d);
  receiver3.firewalled(pkt_out3f);
  receiver3.ready(pkt_out3_ready);
  receiver3.done(pkt_out3_done);
  receiver3.direct_out_addr_to_read(direct_pkt_out_3_addr);
  receiver3.direct_ready(direct_pkt_out3_ready);
  receiver3.direct_done(direct_pkt_out3_done);
  receiver3.fw_hreg(fw_debug_signal[3]);
  receiver3.fw_lreg(fw_debug_signal[7]);
  receiver3.fw_rreg(fw_debug_signal[11]);
  receiver3.CLK(clock);
  
  id0.write(0);
  id1.write(1);
  id2.write(2);
  id3.write(3);
  
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
  return (0);
}
