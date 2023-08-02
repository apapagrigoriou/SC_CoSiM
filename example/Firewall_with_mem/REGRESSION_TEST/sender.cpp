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
#include <stdlib.h>
#include <time.h>

#include <iostream>
using namespace std;

#include "sender.h"

#define D_W_P_0   0x80000014 // Write Packet to Port 0
#define D_W_P_1   0x80001014 // Write Packet to Port 1
#define D_W_P_2   0x80002014 // Write Packet to Port 2
#define D_W_P_3   0x80003014 // Write Packet to Port 3
#define D_R_P_0   0x8000001C // Read from Port 0
#define D_R_P_1   0x8000101C // Read from Port 1
#define D_R_P_2   0x8000201C // Read from Port 2
#define D_R_P_3   0x8000301C // Read from Port 3
#define F_L_P_3   0x5000002C // Setup Firewall Lower Address Range Register for port 3
#define F_U_P_3   0x5000004C // Setup Firewall Upper Address Range Register for port 3
#define F_R_P_3   0x5000006C // Setup Firewall Rule Register for port 3

void sender:: entry()
{
	pkt pkt_data;
	sc_uint<4> dest;
	int count = 0;
	int pkt_count = 0;
	int readcount = 0;
	wait(10);
	srand((100+10*source_id.read().to_uint()));
	tlm::tlm_generic_payload tlm2_trans;
	sc_time delay(SC_ZERO_TIME);  
	uint8_t data_ptr[4];
	uint32_t data_ptr32;
	while(true){
		wait(10);
		sc_uint<32> new_data;
		if (count < 2){
			switch(source_id.read().to_uint()) {
				case 0  :
					tlm2_trans.set_address(D_W_P_0);
					break;
				case 1  :
					tlm2_trans.set_address(D_W_P_1);
					break;
				case 2  :
					tlm2_trans.set_address(D_W_P_2);
					break;
				case 3  :
					tlm2_trans.set_address(D_W_P_3);
					break;
				default : 
					cout << name() << __func__ << ": Unknown Source ID: " << source_id.read().to_uint() << endl;
			}
			new_data.range(31,22) = pkt_count*10;
			new_data.range(21,20) = 0x0;
			new_data.range(19,18) = source_id.read().to_uint();
			new_data.range(17,16) = source_id.read().to_uint();
			new_data.range(15,0) = rand()%65535;
			data_ptr32 = new_data.to_uint();
			bzero(data_ptr, 4); 
			memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
			delay = SC_ZERO_TIME;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Packet: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			m_master_socket->b_transport(tlm2_trans,delay);
			wait(delay);
		}else if ((count >= 2) && (count < 4)){
			switch(source_id.read().to_uint()) {
				case 0  :
					tlm2_trans.set_address(D_W_P_0);
					break;
				case 1  :
					tlm2_trans.set_address(D_W_P_1);
					break;
				case 2  :
					tlm2_trans.set_address(D_W_P_2);
					break;
				case 3  :
					tlm2_trans.set_address(D_W_P_3);
					break;
				default : 
					cout << name() << __func__ << ": Unknown Source ID: " << source_id.read().to_uint() << endl;
			}
			new_data.range(31,22) = readcount*10;
			new_data.range(21,20) = 0x1;
			new_data.range(19,18) = source_id.read().to_uint();
			new_data.range(17,16) = source_id.read().to_uint();
			new_data.range(15,0) = 0x0;
			data_ptr32 = new_data.to_uint();
			bzero(data_ptr, 4); 
			memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
			delay = SC_ZERO_TIME;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Packet: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			m_master_socket->b_transport(tlm2_trans,delay);
			wait(delay);

			wait(1000);

			switch(source_id.read().to_uint()) {
				case 0  :
					tlm2_trans.set_address(D_R_P_0);
					break;
				case 1  :
					tlm2_trans.set_address(D_R_P_1);
					break;
				case 2  :
					tlm2_trans.set_address(D_R_P_2);
					break;
				case 3  :
					tlm2_trans.set_address(D_R_P_3);
					break;
				default : 
					cout << name() << __func__ << ": Unknown Source ID: " << source_id.read().to_uint() << endl;
			}
			bzero(data_ptr, 4); 
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_READ_COMMAND); 
			delay = SC_ZERO_TIME;
			m_master_socket->b_transport(tlm2_trans,delay);
			memcpy((void*) &data_ptr32, (void*) &data_ptr, 4);
			new_data = data_ptr32;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Respond: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			wait(delay);
			readcount++;
		}else if (count == 4){
			//setup a comp firewall rule for write ops
			if (source_id.read().to_uint() == 0x3){
				cout << ".........................." << endl;
				//0xA0 only write
				//0xB0 write && read
				cout << "Setup ("<< source_id <<") Comp Firewall rule u: 0x40003FFF, l: 0x40003000, rule: 0xB0" << endl;
				tlm2_trans.set_address(F_R_P_3);
				data_ptr32 = 0x00000000;
				memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
				tlm2_trans.set_data_ptr(data_ptr);
				tlm2_trans.set_data_length(4);
				tlm2_trans.set_streaming_width(4);
				tlm2_trans.set_byte_enable_ptr(nullptr);
				tlm2_trans.set_dmi_allowed(false);
				tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
				tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
				delay = SC_ZERO_TIME;
				m_master_socket->b_transport(tlm2_trans,delay);
				wait(delay);
				tlm2_trans.set_address(F_L_P_3);
				data_ptr32 = 0x40000000;
				memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
				tlm2_trans.set_data_ptr(data_ptr);
				tlm2_trans.set_data_length(4);
				tlm2_trans.set_streaming_width(4);
				tlm2_trans.set_byte_enable_ptr(nullptr);
				tlm2_trans.set_dmi_allowed(false);
				tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
				tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
				delay = SC_ZERO_TIME;
				m_master_socket->b_transport(tlm2_trans,delay);
				wait(delay);
				tlm2_trans.set_address(F_U_P_3);
				data_ptr32 = 0x40000FFF;
				memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
				tlm2_trans.set_data_ptr(data_ptr);
				tlm2_trans.set_data_length(4);
				tlm2_trans.set_streaming_width(4);
				tlm2_trans.set_byte_enable_ptr(nullptr);
				tlm2_trans.set_dmi_allowed(false);
				tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
				tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
				delay = SC_ZERO_TIME;
				m_master_socket->b_transport(tlm2_trans,delay);
				wait(delay);
				tlm2_trans.set_address(F_R_P_3);
				data_ptr32 = 0xB0;
				memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
				tlm2_trans.set_data_ptr(data_ptr);
				tlm2_trans.set_data_length(4);
				tlm2_trans.set_streaming_width(4);
				tlm2_trans.set_byte_enable_ptr(nullptr);
				tlm2_trans.set_dmi_allowed(false);
				tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
				tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
				delay = SC_ZERO_TIME;
				m_master_socket->b_transport(tlm2_trans,delay);
				wait(delay);
			}
			pkt_count = 0;
			readcount = 0;
		}else if (count == 5){
			switch(source_id.read().to_uint()) {
				case 0  :
					tlm2_trans.set_address(D_W_P_0);
					break;
				case 1  :
					tlm2_trans.set_address(D_W_P_1);
					break;
				case 2  :
					tlm2_trans.set_address(D_W_P_2);
					break;
				case 3  :
					tlm2_trans.set_address(D_W_P_3);
					break;
				default : 
					cout << name() << __func__ << ": Unknown Source ID: " << source_id.read().to_uint() << endl;
			}
			new_data.range(31,22) = pkt_count*10;
			new_data.range(21,20) = 0x0;
			new_data.range(19,18) = source_id.read().to_uint();   //source
			new_data.range(17,16) = 3-source_id.read().to_uint(); //destination
			new_data.range(15,0) = rand()%65535;
			data_ptr32 = new_data.to_uint();
			bzero(data_ptr, 4); 
			memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
			delay = SC_ZERO_TIME;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Packet: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			m_master_socket->b_transport(tlm2_trans,delay);
			wait(delay);
		}else if (count == 6) {
			switch(source_id.read().to_uint()) {
				case 0  :
					tlm2_trans.set_address(D_W_P_0);
					break;
				case 1  :
					tlm2_trans.set_address(D_W_P_1);
					break;
				case 2  :
					tlm2_trans.set_address(D_W_P_2);
					break;
				case 3  :
					tlm2_trans.set_address(D_W_P_3);
					break;
				default : 
					cout << name() << __func__ << ": Unknown Source ID: " << source_id.read().to_uint() << endl;
			}
			new_data.range(31,22) = readcount*10;
			new_data.range(21,20) = 0x1;
			new_data.range(19,18) = source_id.read().to_uint();   //source
			new_data.range(17,16) = 3-source_id.read().to_uint(); //destination
			new_data.range(15,0) = 0x0;
			data_ptr32 = new_data.to_uint();
			bzero(data_ptr, 4); 
			memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
			delay = SC_ZERO_TIME;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Packet: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			m_master_socket->b_transport(tlm2_trans,delay);
			wait(delay);
			wait(1000);
			switch(source_id.read().to_uint()) {
				case 0  :
					tlm2_trans.set_address(D_R_P_0);
					break;
				case 1  :
					tlm2_trans.set_address(D_R_P_1);
					break;
				case 2  :
					tlm2_trans.set_address(D_R_P_2);
					break;
				case 3  :
					tlm2_trans.set_address(D_R_P_3);
					break;
				default : 
					cout << name() << __func__ << ": Unknown Source ID: " << source_id.read().to_uint() << endl;
			}
			bzero(data_ptr, 4); 
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_READ_COMMAND); 
			delay = SC_ZERO_TIME;
			m_master_socket->b_transport(tlm2_trans,delay);
			memcpy((void*) &data_ptr32, (void*) &data_ptr, 4);
			new_data = data_ptr32;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Respond: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			wait(delay);
			readcount++;
			
		}else if (count == 7){
			tlm2_trans.set_address(0x4000200a);
			data_ptr32 = 0x100;
			bzero(data_ptr, 4); 
			memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
			delay = SC_ZERO_TIME;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Packet: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			m_master_socket->b_transport(tlm2_trans,delay);
			wait(delay);

			wait(1000);

			tlm2_trans.set_address(0x4000200a);
			bzero(data_ptr, 4); 
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_READ_COMMAND); 
			delay = SC_ZERO_TIME;
			m_master_socket->b_transport(tlm2_trans,delay);
			memcpy((void*) &data_ptr32, (void*) &data_ptr, 4);
			new_data = data_ptr32;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Respond: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			wait(delay);
		}else if (count == 8){
			tlm2_trans.set_address(0x40000000);
			bzero(data_ptr, 4); 
			data_ptr32 = 0x000;
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
			delay = SC_ZERO_TIME;
			m_master_socket->b_transport(tlm2_trans,delay);
			memcpy((void*) &data_ptr32, (void*) &data_ptr, 4);
			new_data = data_ptr32;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Respond: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			wait(delay);
			wait(1000);

			tlm2_trans.set_address(0x40000000);
			bzero(data_ptr, 4); 
			data_ptr32 = 0x000;
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_READ_COMMAND); 
			delay = SC_ZERO_TIME;
			m_master_socket->b_transport(tlm2_trans,delay);
			memcpy((void*) &data_ptr32, (void*) &data_ptr, 4);
			new_data = data_ptr32;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Respond: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			wait(delay);
			wait(1000);

			tlm2_trans.set_address(0x80000014);
			data_ptr32 = 0x100;
			bzero(data_ptr, 4); 
			memcpy((void*) &data_ptr, (void*) &data_ptr32, 4);
			tlm2_trans.set_data_ptr(data_ptr);
			tlm2_trans.set_data_length(4);
			tlm2_trans.set_streaming_width(4);
			tlm2_trans.set_byte_enable_ptr(nullptr);
			tlm2_trans.set_dmi_allowed(false);
			tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
			delay = SC_ZERO_TIME;
			cout << ".........................." << endl;
			cout << "New Packet Sent: " <<  count << endl;
			cout << "Sender ID: " << (unsigned int)source_id.read() << endl;
			cout << "UINT Packet: 0x" << hex << new_data << dec << endl;
			cout << ".........................." << endl;
			m_master_socket->b_transport(tlm2_trans,delay);
			wait(delay);
			wait(1000);

			break;
		}
		/////wait for 1 to 3 clock periods/////////////////////
		wait(10);
		count++;
		pkt_count++;
	}
	if (source_id.read().to_uint() == 0x3){
		wait(100000);
		sc_stop();
	}
}
