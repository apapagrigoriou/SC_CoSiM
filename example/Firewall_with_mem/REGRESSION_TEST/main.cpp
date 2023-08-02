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
#include "systemc.h"
#include "mem_bus_if.h"
#include "fw_packet.h"
#include "firewall_with_mem.h"
#include "sender.h"
#include "receiver.h"

int sc_main(int, char *[])
{

  // Remove simulation warnings
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

  sc_signal<sc_int<4> > id0, id1, id2, id3;

  sc_clock clock1("CLOCK1", 75, SC_NS, 0.5, 0.0, SC_NS);

  firewall_with_mem switch1("SWITCH");
  switch1.CLK(clock1);
  switch1.out_0(bus_mem0);
  switch1.out_1(bus_mem1);
  switch1.out_2(bus_mem2);
  switch1.out_3(bus_mem3);
  switch1.out_addr_to_read_0(pkt_out_0_addr);
  switch1.out_addr_to_read_1(pkt_out_1_addr);
  switch1.out_addr_to_read_2(pkt_out_2_addr);
  switch1.out_addr_to_read_3(pkt_out_3_addr);
  switch1.done_0(pkt_out0_done);
  switch1.done_1(pkt_out1_done);
  switch1.done_2(pkt_out2_done);
  switch1.done_3(pkt_out3_done);
  switch1.ready_0(pkt_out0_ready);
  switch1.ready_1(pkt_out1_ready);
  switch1.ready_2(pkt_out2_ready);
  switch1.ready_3(pkt_out3_ready);
  switch1.dir_mem_out_addr_to_read_0(direct_pkt_out_0_addr);
  switch1.dir_mem_out_addr_to_read_1(direct_pkt_out_1_addr);
  switch1.dir_mem_out_addr_to_read_2(direct_pkt_out_2_addr);
  switch1.dir_mem_out_addr_to_read_3(direct_pkt_out_3_addr);
  switch1.dir_mem_done_0(direct_pkt_out0_done);
  switch1.dir_mem_done_1(direct_pkt_out1_done);
  switch1.dir_mem_done_2(direct_pkt_out2_done);
  switch1.dir_mem_done_3(direct_pkt_out3_done);
  switch1.dir_mem_ready_0(direct_pkt_out0_ready);
  switch1.dir_mem_ready_1(direct_pkt_out1_ready);
  switch1.dir_mem_ready_2(direct_pkt_out2_ready);
  switch1.dir_mem_ready_3(direct_pkt_out3_ready);
  switch1.reset(pkt_sw_reset);

  sender sender3("SENDER3");
  sender3.m_master_socket.bind(switch1.m_slave_socket);
  sender3.source_id(id3);
  sender3.CLK(clock1);

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
  receiver0.CLK(clock1);

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
  receiver1.CLK(clock1);

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
  receiver2.CLK(clock1);

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
  receiver3.CLK(clock1);

  id0.write(0);
  id1.write(1);
  id2.write(2);
  id3.write(3);

  pkt_sw_reset.write(true);
  sc_start(1000, SC_NS);
  pkt_sw_reset.write(false);

  sc_start();
  return 0;

}
