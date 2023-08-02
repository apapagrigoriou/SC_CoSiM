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
#include <iostream>
using namespace std;

#include "receiver.h"


void receiver:: entry()
{
  unsigned int new_address;
  sc_uint<32> temp_val1;
  sc_uint<32> temp_val2;
  sc_uint<32> temp_val3;
  sc_uint<32> temp_val4;
  sc_uint<32> temp_val5;
  sc_uint<32> temp_val6;
  bool indone = false;
  while(true)
    {
	   wait();
	   if (ready.read()) {
		   if (!indone){
			   new_address = out_addr_to_read.read();
			   temp_val1 = pkt_in->read(new_address);
			   temp_val2 = count.read();
			   temp_val3 = dropped.read();
			   temp_val4 = fw_hreg.read();
			   temp_val5 = fw_lreg.read();
			   temp_val6 = fw_rreg.read();
			   cout << "                                  ......."<< name() <<"....." << endl;
			   cout << "                                  New Packet Received" << endl;
			   cout << "                                  Receiver ID: " << (unsigned int)sink_id.read() + 1 << endl;
			   cout << "                                  Packet Address Offset: 0x" << hex << new_address << dec << endl;
			   cout << "                                  Packet Value: " << temp_val1.to_string(SC_HEX) << endl;
			   cout << "                                  Count: " << (unsigned int) temp_val2 << endl;
			   cout << "                                  Dropped: " << (unsigned int) temp_val3 << endl;
			   cout << "                                  FW H Reg: 0x" << hex << temp_val4 << dec << endl;
			   cout << "                                  FW L Reg: 0x" << hex <<  temp_val5 << dec << endl;
			   cout << "                                  FW R Reg: 0x" << hex <<  temp_val6 << dec << endl;
			   cout << "                                  .........................." << endl;
			   indone = true;
			   done.write(true);
		   }
	   }else{
		   indone = false;
		   done.write(false);
	   }
    }
}


void receiver:: directentry()
{
  unsigned int new_address;
  sc_uint<32> temp_val1;
  bool indone = false;
  while(true)
    {
	   wait();
	   if (direct_ready.read()) {
		   if (!indone){
			   new_address = direct_out_addr_to_read.read();
			   temp_val1 = pkt_in->read(new_address);
			   cout << "                                  ......."<< name() <<"....." << endl;
			   cout << "                                  New DIRECT Packet Received" << endl;
			   cout << "                                  Receiver ID: " << (unsigned int)sink_id.read() + 1 << endl;
			   cout << "                                  Packet Address Offset: 0x" << hex << new_address << dec << endl;
			   cout << "                                  Packet Value: " << temp_val1.to_string(SC_HEX) << endl;
			   cout << "                                  .........................." << endl;
			   indone = true;
			   direct_done.write(true);
		   }
	   }else{
		   indone = false;
		   direct_done.write(false);
	   }
    }
}
