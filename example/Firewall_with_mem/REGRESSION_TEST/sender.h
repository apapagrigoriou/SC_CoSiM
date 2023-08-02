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
#ifndef SENDER_H_INCLUDED
#define SENDER_H_INCLUDED

#include "systemc.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "fw_packet.h"

struct sender: sc_module {

  tlm_utils::simple_initiator_socket<sender> m_master_socket;
  sc_in<sc_int<4> > source_id;
  sc_in_clk CLK;

  SC_CTOR(sender)
    {
      SC_CTHREAD(entry, CLK.pos());
     }
  void entry();
};
#endif // SENDER_H_INCLUDED
