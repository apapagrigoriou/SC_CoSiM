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
#ifndef RECEIVER_H_INCLUDED
#define RECEIVER_H_INCLUDED

#include "systemc.h"
#include "mem_bus_if.h"
#include "tlm.h"
using namespace tlm;
#include "fw_packet.h"

struct receiver: sc_module {
  mem_bus_port<sc_uint<32> >  pkt_in;
  sc_in<sc_int<4> >   sink_id;
  sc_in<unsigned int> count;
  sc_in<unsigned int> dropped;
  sc_in<unsigned int> firewalled;
  sc_in<bool>         ready;
  sc_out<bool>        done;
  sc_in<unsigned int> out_addr_to_read;
  sc_in<bool>         direct_ready;
  sc_out<bool>        direct_done;
  sc_in<unsigned int> direct_out_addr_to_read;
  sc_in<sc_uint<32> > fw_hreg;
  sc_in<sc_uint<32> > fw_lreg;
  sc_in<sc_uint<32> > fw_rreg;
  sc_in_clk           CLK;

  SC_CTOR(receiver):pkt_in ("pkt_in") {
      SC_CTHREAD(entry, CLK.pos());
      SC_CTHREAD(directentry, CLK.pos());
      
    }

 void entry();
 void directentry();

};
#endif // RECEIVER_H_INCLUDED
