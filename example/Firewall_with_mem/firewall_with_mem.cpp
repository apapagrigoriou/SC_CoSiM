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
#include "fw_packet.h"
#include "firewall_with_mem.h"

void firewall_with_mem::execute_write_thread(void)
{
  while(true) {
    wait(m_write_event); //wait for write event to execute code
    //do here whatever we want
//    cout << name() << ": HERE HANDLING Write CODE: "<< __func__ << endl;
    //direct access to BRAM Request
    if (m_write_address < D_M_R_1){
		out_address_0 = m_write_address;
		out_read_0 = false;
		dir_mem_out_fifo_0.write(m_register_write);
		continue;
	}else if (m_write_address < D_M_R_2){
		out_address_1 = m_write_address;
		out_read_1 = false;
		dir_mem_out_fifo_1.write(m_register_write);
		continue;
	}else if (m_write_address < D_M_R_3){
		out_address_2 = m_write_address;
		out_read_2 = false;
		dir_mem_out_fifo_2.write(m_register_write);
		continue;
	}else if (m_write_address < D_M_R_M){
		out_address_3 = m_write_address;
		out_read_3 = false;
		dir_mem_out_fifo_3.write(m_register_write);
		continue;
	}
	//for firewall requests
    //use case to m_write_address to find what to execute
	switch(m_write_address) {
		case D_W_P_0  :
//			cout << name() << ": ****FW ACCESSING READ PORT 0 **** : "<< __func__ << endl;
			in_0.write(m_register_write);
			break;
		case D_W_P_1  :
			in_1.write(m_register_write);
			break;
		case D_W_P_2  :
			in_2.write(m_register_write);
			break;
		case D_W_P_3  :
			in_3.write(m_register_write);
			break;
		case F_L_P_0  :
			setup_lower_range_0.write(m_register_write);
			//cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_L_P_1  :
			setup_lower_range_1.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_L_P_2  :
			setup_lower_range_2.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_L_P_3  :
			setup_lower_range_3.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_U_P_0  :
			setup_upper_range_0.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_U_P_1  :
			setup_upper_range_1.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_U_P_2  :
			setup_upper_range_2.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_U_P_3  :
			setup_upper_range_3.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_R_P_0  :
			setup_rule_range_0.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_R_P_1  :
			setup_rule_range_1.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_R_P_2  :
			setup_rule_range_2.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		case F_R_P_3  :
			setup_rule_range_3.write(m_register_write);
//			cout << "**** m_write_ended_event NOTIFY (SETUP)****" << endl;
            m_write_ended_event.notify();
			break;
		default : 
			cout << name() << __func__ << ": Unknown Address request: " << hex << m_read_address << dec << endl;
	}    
  }
}

void firewall_with_mem::execute_read_thread(void)
{
  while(true) {
    wait(m_read_event); //wait for write event to execute code
    //do here whatever we want
    cout << name() << ": HERE HANDLING Read CODE: "<< __func__ << endl;
    //direct access to BRAM Request
    if (m_read_address < D_M_R_1){
		out_address_0 = m_read_address;
		out_read_0 = true;
		dir_mem_out_fifo_0.write(m_register_write);
		wait(m_read_event_pending);
		m_register_read = dir_mem_responce_fifo_0.read();
		m_read_ended_event.notify();
		continue;
	}else if (m_read_address < D_M_R_2){
		out_address_1 = m_read_address;
		out_read_1 = true;
		dir_mem_out_fifo_1.write(m_register_write);
		wait(m_read_event_pending);
		m_register_read = dir_mem_responce_fifo_1.read();
		m_read_ended_event.notify();
		continue;
	}else if (m_read_address < D_M_R_3){
		out_address_2 = m_read_address;
		out_read_2 = true;
		dir_mem_out_fifo_2.write(m_register_write);
		wait(m_read_event_pending);
		m_register_read = dir_mem_responce_fifo_2.read();
		m_read_ended_event.notify();
		continue;
	}else if (m_read_address < D_M_R_M){
		out_address_3 = m_read_address;
		out_read_3 = true;
		dir_mem_out_fifo_3.write(m_register_write);
		wait(m_read_event_pending);
		m_register_read = dir_mem_responce_fifo_3.read();
		m_read_ended_event.notify();
		continue;
	}
	//for firewall requests
    //use case to m_read_address to find what to execute
	switch(m_read_address) {
		case D_R_P_0  :
//			cout << name() << __func__ << " Trying to Read from P0 " << endl;
			m_register_read = in_responce_0.read();
//			cout << name() << __func__ << " Data was Read from P0 " << m_register_read << endl;
//			m_read_ended_event.notify();
			break;
		case D_R_P_1  :
//			cout << name() << __func__ << " Trying to Read from P1 " << endl;
			m_register_read = in_responce_1.read();
//			cout << name() << __func__ << " Data was Read from P1 " << m_register_read << endl;
//			m_read_ended_event.notify();
			break;
		case D_R_P_2  :
//			cout << name() << __func__ << " Trying to Read from P2 " << endl;
			m_register_read = in_responce_2.read();
//			cout << name() << __func__ << " Data was Read from P2 " << m_register_read << endl;
//			m_read_ended_event.notify();
			break;
		case D_R_P_3  :
//			cout << name() << __func__ << " Trying to Read from P3 " << endl;
			m_register_read = in_responce_3.read();
//			cout << name() << __func__ << " Data was Read from P3 " << m_register_read << endl;
//			m_read_ended_event.notify();
			break;
		case D_T_P_0  :
			m_register_read = total_0.read();
			break;
		case D_T_P_1  :
			m_register_read = total_1.read();
			break;
		case D_T_P_2  :
			m_register_read = total_2.read();
			break;
		case D_T_P_3  :
			m_register_read = total_3.read();
			break;
		case D_D_P_0  :
			m_register_read = droped_0.read();
			break;
		case D_D_P_1  :
			m_register_read = droped_1.read();
			break;
		case D_D_P_2  :
			m_register_read = droped_2.read();
			break;
		case D_D_P_3  :
			m_register_read = droped_3.read();
			break;
		case D_X_P_0  :
			m_register_read = fw_rule_hit_counter_0.read();
			break;
		case D_X_P_1  :
			m_register_read = fw_rule_hit_counter_1.read();
			break;
		case D_X_P_2  :
			m_register_read = fw_rule_hit_counter_2.read();
			break;
		case D_X_P_3  :
			m_register_read = fw_rule_hit_counter_3.read();
			break;
		default : 
			cout << name() << __func__ << ": Unknown Address request: " << hex << m_read_address << dec << endl;
	}
    m_read_ended_event.notify();
  }
}

void firewall_with_mem::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
  tlm::tlm_command command = trans.get_command();
  sc_dt::uint64 address = trans.get_address();
  unsigned char* data_ptr = trans.get_data_ptr();
  unsigned int data_length = trans.get_data_length();
  unsigned char* byte_enables = trans.get_byte_enable_ptr();
  unsigned int streaming_width = trans.get_streaming_width();
  cout << name() << " Recieved data_length: " << data_length  << " |streaming_width: " << streaming_width << " |address: 0x" << hex << address << dec << endl;

  if (data_length != 4){
    SC_REPORT_ERROR("FIREWALL","ALERT: Interface Suport Only AXI4Lite Commands (length 4)");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
    return;
  }else if ((!((address >= 0x40000000) && (address <= 0x40004000))) && 
	  (!((address > 0x50000000) && (address < 0x50000070))) && 
      (!((address > 0x50001000) && (address < 0x50001070))) &&
	  (!((address > 0x80000000) && (address < 0x80004000)))){
    SC_REPORT_ERROR("FIREWALL","ALERT: Out of Bound Address request");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
    return;
  } else if (byte_enables != 0) {
    SC_REPORT_ERROR("FIREWALL","ALERT: No support for byte enables");
    trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
    return;
  }
  
  //implementing read and write commands
  if ( command == tlm::TLM_READ_COMMAND ) {
    m_read_address = address;
    m_read_event.notify();
    wait(m_read_ended_event);
    memcpy((void*) data_ptr, (void*) &m_register_read, data_length);
  } else if ( command == tlm::TLM_WRITE_COMMAND ) {
	m_write_address = address;
    memcpy((void*) &m_register_write, (void*) data_ptr, data_length);
    m_write_event.notify();
//	cout << name() << ": ****WAITING FOR m_write_ended_event Notify **** : "<< __func__ << endl;
    wait(m_write_ended_event);
  }
  // Obliged to set response status to indicate successful completion
  trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

unsigned int firewall_with_mem::transport_dbg(tlm::tlm_generic_payload& trans)
{
  int transferred = 0;
  tlm::tlm_command command = trans.get_command();
  sc_dt::uint64 address = trans.get_address();
  unsigned char* data_ptr = trans.get_data_ptr();
  unsigned int data_length = trans.get_data_length();
  unsigned char* byte_enables = trans.get_byte_enable_ptr();
  unsigned int streaming_width = trans.get_streaming_width();
  cout << name() << " Recieved data_length: " << data_length  << " |streaming_width: " << streaming_width << " |address: 0x" << hex << address << dec << endl;

  if (data_length != 4){
    SC_REPORT_ERROR("FIREWALL","ALERT: Interface Suport Only AXI4Lite Commands (length 4)");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
	return transferred;
  }else if ((!((address >= 0x40000000) && (address <= 0x40004000))) && 
	  (!((address > 0x50000000) && (address < 0x50000070))) && 
      (!((address > 0x50001000) && (address < 0x50001070))) &&
	  (!((address > 0x80000000) && (address < 0x80004000)))){
    SC_REPORT_ERROR("FIREWALL","ALERT: Out of Bound Address request");
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
	return transferred;
  } else if (byte_enables != 0) {
    SC_REPORT_ERROR("FIREWALL","ALERT: No support for byte enables");
    trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
	return transferred;
  }
  
  //implementing read and write commands
  if ( command == tlm::TLM_READ_COMMAND ) {
    m_read_address = address;
    m_read_event.notify();
    memcpy((void*) data_ptr, (void*) &m_register_read, data_length);
    transferred = data_length;
    wait(m_read_ended_event);
  } else if ( command == tlm::TLM_WRITE_COMMAND ) {
	m_write_address = address;
    memcpy((void*) &m_register_write, (void*) data_ptr, data_length);
    transferred = data_length;
    m_write_event.notify();
    wait(m_write_ended_event);
  }
  // Obliged to set response status to indicate successful completion
  trans.set_response_status( tlm::TLM_OK_RESPONSE );

  return transferred;
}


void firewall_with_mem::setupfw_rule()
{
  unsigned int precommand_0 = 0x0;
  unsigned int precommand_1 = 0x0;
  unsigned int precommand_2 = 0x0;
  unsigned int precommand_3 = 0x0;
  fw_rule_range_0.write(0x0);
  fw_rule_range_1.write(0x0);
  fw_rule_range_2.write(0x0);
  fw_rule_range_3.write(0x0);
  fw_upper_range_0.write(0x0);
  fw_upper_range_1.write(0x0);
  fw_upper_range_2.write(0x0);
  fw_upper_range_3.write(0x0);
  fw_lower_range_0.write(0x0);
  fw_lower_range_1.write(0x0);
  fw_lower_range_2.write(0x0);
  fw_lower_range_3.write(0x0);

  wait();
  while(true)
  {
    wait();
    sc_uint<32> tmp_read_p0_0 = setup_rule_range_0.read();
    if (tmp_read_p0_0.to_uint() != precommand_0){
      sc_uint<32> tmp_read_p0_1 = setup_lower_range_0.read();  
      sc_uint<32> tmp_read_p0_2 = setup_upper_range_0.read();  
      precommand_0 = tmp_read_p0_0.to_uint();
      fw_rule_range_0.write(tmp_read_p0_0);
      fw_lower_range_0.write(tmp_read_p0_1);
      fw_upper_range_0.write(tmp_read_p0_2);

      cout << name() << ".p0: Setting Up FW rule: upper: 0x" << hex << tmp_read_p0_2.to_uint() << dec
           << " lower: 0x" << hex << tmp_read_p0_1.to_uint() << dec
           << " rule: 0x" << hex << tmp_read_p0_0.to_uint() << dec << endl;

    }
    sc_uint<32> tmp_read_p1_0 = setup_rule_range_1.read();
    if (tmp_read_p1_0.to_uint() != precommand_1){
      sc_uint<32> tmp_read_p1_1 = setup_lower_range_1.read();  
      sc_uint<32> tmp_read_p1_2 = setup_upper_range_1.read();  
      precommand_1 = tmp_read_p1_0.to_uint();
      fw_rule_range_1.write(tmp_read_p1_0);
      fw_lower_range_1.write(tmp_read_p1_1);
      fw_upper_range_1.write(tmp_read_p1_2);

      cout << name() << ".p1: Setting Up FW rule: upper: 0x" << hex << tmp_read_p1_2.to_uint() << dec
           << " lower: 0x" << hex << tmp_read_p1_1.to_uint() << dec
           << " rule: 0x" << hex << tmp_read_p1_0.to_uint() << dec << endl;

    }
    sc_uint<32> tmp_read_p2_0 = setup_rule_range_2.read();
    if (tmp_read_p2_0.to_uint() != precommand_2){
      sc_uint<32> tmp_read_p2_1 = setup_lower_range_2.read();  
      sc_uint<32> tmp_read_p2_2 = setup_upper_range_2.read();  
      precommand_2 = tmp_read_p2_0.to_uint();
      fw_rule_range_2.write(tmp_read_p2_0);
      fw_lower_range_2.write(tmp_read_p2_1);
      fw_upper_range_2.write(tmp_read_p2_2);

      cout << name() << ".p2: Setting Up FW rule: upper: 0x" << hex << tmp_read_p2_2.to_uint() << dec
           << " lower: 0x" << hex << tmp_read_p2_1.to_uint() << dec
           << " rule: 0x" << hex << tmp_read_p2_0.to_uint() << dec << endl;

    }
    sc_uint<32> tmp_read_p3_0 = setup_rule_range_3.read();
    if (tmp_read_p3_0.to_uint() != precommand_3){
      sc_uint<32> tmp_read_p3_1 = setup_lower_range_3.read();  
      sc_uint<32> tmp_read_p3_2 = setup_upper_range_3.read();  
      precommand_3 = tmp_read_p3_0.to_uint();
      fw_rule_range_3.write(tmp_read_p3_0);
      fw_lower_range_3.write(tmp_read_p3_1);
      fw_upper_range_3.write(tmp_read_p3_2);

      cout << name() << ".p3: Setting Up FW rule: upper: 0x" << hex << tmp_read_p3_2.to_uint() << dec
           << " lower: 0x" << hex << tmp_read_p3_1.to_uint() << dec
           << " rule: 0x" << hex << tmp_read_p3_0.to_uint() << dec << endl;

    }
  }
}

void firewall_with_mem::interface_in_0()
{
  unsigned int preval0 = 0x0;
  unsigned int pre_read_r = 0x0;

  unsigned int fw_read_l = 0x0;
  unsigned int fw_read_h = 0x0;
  sc_uint<32> fw_read_r = 0x0;

  fw_hreg_p_0.write(0x0);
  fw_lreg_p_0.write(0x0);
  fw_rreg_p_0.write(0x0);

  count_total_0 = 0x0;
  rule_hit_counter_0 = 0x0;
  count_droped_0 = 0x0;
  total_0.write(0x0);
  droped_0.write(0x0);
  fw_rule_hit_counter_0.write(0x0);
  wait();
  while(true)
  {
    wait();
    sc_uint<32> new_fw0 = fw_rule_range_0.read();
    if (new_fw0.to_uint() != pre_read_r){
    	pre_read_r = new_fw0.to_uint();
    	fw_read_r = new_fw0;
    	fw_read_l = fw_lower_range_0.read().to_uint();
    	fw_read_h = fw_upper_range_0.read().to_uint();
    	//this is debug for verify
        fw_hreg_p_0.write(fw_read_h);
        fw_lreg_p_0.write(fw_read_l);
        fw_rreg_p_0.write(fw_read_r);
        continue;
    }
    sc_uint<32> new_data0;
    if (!in_0.nb_read(new_data0)){
		continue;
	}
    if (new_data0.to_uint() != preval0){
      preval0 = new_data0.to_uint();
      count_total_0 = count_total_0 + 1;
      total_0.write(count_total_0);

      cout << name() << ".SW1.p0: Received data " << count_total_0 << ": 0x" << hex << new_data0.to_uint() << dec << endl;

      //check for firewall rule
      //Rule register (bit active - stands for):
      //7 -> 1: Enable Rule
      //6 -> 1: Enabled NoC Mode // 0: Enable Compatibility Mode
      //5 -> 1: Enable Rule for Write OpCode
      //4 -> 1: Enable Rule for Read OpCode
      //3 -> 1: Enable Rule for Destination Port 3
      //2 -> 1: Enable Rule for Destination Port 2
      //1 -> 1: Enable Rule for Destination Port 1
      //0 -> 1: Enable Rule for Destination Port 0
      bool fw_hit = false;
      if (fw_read_r[7].to_bool()){
    	  if ((fw_read_r[5].to_bool() && (new_data0.range(21,20).to_uint() == 0x0)) || //firewall for writeOp
      		  (fw_read_r[4].to_bool() && (new_data0.range(21,20).to_uint() == 0x1))){ //firewall for ReadOp
    		  sc_uint<32> my_address = new_data0.range(31,22).to_uint();
    		  if (fw_read_r[6].to_bool()){ //NoC Mod
    			  if ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >=  fw_read_l)){ //the range of address is between
    				  if ((fw_read_r[3].to_bool() && new_data0.range(17,16).to_uint() == 0x3) ||
    					  (fw_read_r[2].to_bool() && new_data0.range(17,16).to_uint() == 0x2) ||
      					  (fw_read_r[1].to_bool() && new_data0.range(17,16).to_uint() == 0x1) ||
      					  (fw_read_r[0].to_bool() && new_data0.range(17,16).to_uint() == 0x0)){
      							fw_hit = true;
    				  }
    			  }
    		  }else{ //compatibility mode
    			  //physical address mode
    			  if (new_data0.range(17,16).to_uint() == 0x0){ //go to port 00 -> 0
    				  my_address += 0x40000000;
    			  }else if (new_data0.range(17,16).to_uint() == 0x1){ //go to port 01 -> 1
    				  my_address += 0x40001000;
    			  }else if (new_data0.range(17,16).to_uint() == 0x2){ //go to port 10 -> 2
    				  my_address += 0x40002000;
    			  }else if (new_data0.range(17,16).to_uint() == 0x3){ //go to port 11 -> 3
    				  my_address += 0x40003000;
    			  }else{
    				  fw_hit = true;

    				  cout << name() << ".SW1.p0: While Working in Physical Address mode no valid input address "<< endl;

    			  }

    			  cout << name() << ".SW1.p0: Checking if address: 0x"<< hex << my_address.to_uint() << dec << " is between 0x"
    				   << hex << fw_read_l << dec << " and 0x" << hex << fw_read_h << dec << endl;

				  if ((!fw_hit) && ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >= fw_read_l))){
					   fw_hit = true;
    		      }
    		  }
    	  }
      }
      if (fw_hit){
    	  if(new_data0.range(21,20).to_uint() == 0x1){ //if read operation set response to 0x0
    		  init_responce_fifo_0.write(0x00000000);
    	  }
    	  rule_hit_counter_0++;
    	  fw_rule_hit_counter_0.write(rule_hit_counter_0);

   	      cout << name() << ".SW1.p0: Requested Packet " << hex << new_data0.to_uint() << dec  << " Matched FW Rule [" << rule_hit_counter_0 << "]" << endl;
// 		  cout << "**** m_write_ended_event NOTIFY (FWRULED)****" << endl;
          m_write_ended_event.notify();
          wait(1); //nesseary for the fifos to be ready
    	  continue;
      }
      if ((responce_s0.read()[31].to_bool()) && (new_data0.range(21,20).to_uint() == 0x1)){
            count_droped_0 = count_droped_0 + 1;
            droped_0.write(count_droped_0);
            cout << name() << ".SW1.p0: Requested Write Op while pending result. Packet Dropped [" << count_droped_0 << "]: 0x" << hex << new_data0.to_uint() << dec << endl;
//			cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
			m_write_ended_event.notify();
      }else{
            if (in_fifo_0.num_free() > 0){
                if (new_data0.range(21,20).to_uint() == 0x1){ //for read operations
                    new_data0.range(19,18) = 0x0; //set source port
                    init_responce_fifo_0.write(0x80000000); //set 31 bit to 1 while the rest to 0 this mean expecting result

                }
                in_fifo_0.write(new_data0);
                cout << name() << ".SW1.p0: Packet accepted in in FIFO 0 [" << in_fifo_0.num_available() << "]: 0x" << hex << new_data0.to_uint() << dec << endl;

            }else{
                count_droped_0 = count_droped_0 + 1;
                droped_0.write(count_droped_0);
                cout << name() << ".SW1.p0: Packet Dropped [" << count_droped_0 << "]: 0x" << hex << new_data0.to_uint() << dec << endl;
//				cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
				m_write_ended_event.notify();
            }
      }
      wait(1); //nesseary for the fifos to be ready
    }

  }
}

void firewall_with_mem::interface_in_1()
{
  unsigned int preval = 0x0;
  unsigned int pre_read_r = 0x0;
  unsigned int fw_read_l = 0x0;
  unsigned int fw_read_h = 0x0;
  sc_uint<32> fw_read_r = 0x0;
  fw_hreg_p_1.write(0x0);
  fw_lreg_p_1.write(0x0);
  fw_rreg_p_1.write(0x0);

  count_total_1 = 0x0;
  rule_hit_counter_1 = 0x0;
  count_droped_1 = 0x0;
  total_1.write(0x0);
  droped_1.write(0x0);
  fw_rule_hit_counter_1.write(0x0);
  wait();
  while(true)
  {
    wait();
	sc_uint<32> new_fw = fw_rule_range_1.read();
	if (new_fw.to_uint() != pre_read_r){
	  pre_read_r = new_fw.to_uint();
	  fw_read_r = new_fw;
	  fw_read_l = fw_lower_range_1.read().to_uint();
	  fw_read_h = fw_upper_range_1.read().to_uint();
	  //this is debug for verify
	  fw_hreg_p_1.write(fw_read_h);
	  fw_lreg_p_1.write(fw_read_l);
	  fw_rreg_p_1.write(fw_read_r);
	  continue;
	}
	sc_uint<32> new_data;
    if (!in_1.nb_read(new_data)){
		continue;
	}
	if (new_data.to_uint() != preval){
	  preval = new_data.to_uint();
	  count_total_1 = count_total_1 + 1;
	  total_1.write(count_total_1);

      cout << name() << ".SW1.p1: Received data " << count_total_1 << ": 0x" << hex << new_data.to_uint() << dec << endl;

      //check for firewall rule
	  //Rule register (bit active - stands for):
	  //7 -> 1: Enable Rule
	  //6 -> 1: Enabled NoC Mode // 0: Enable Compatibility Mode
	  //5 -> 1: Enable Rule for Write OpCode
	  //4 -> 1: Enable Rule for Read OpCode
	  //3 -> 1: Enable Rule for Destination Port 3
	  //2 -> 1: Enable Rule for Destination Port 2
	  //1 -> 1: Enable Rule for Destination Port 1
	  //0 -> 1: Enable Rule for Destination Port 0
	  bool fw_hit = false;
	  if (fw_read_r[7].to_bool()){
	    if ((fw_read_r[5].to_bool() && (new_data.range(21,20).to_uint() == 0x0)) || //firewall for writeOp
	        (fw_read_r[4].to_bool() && (new_data.range(21,20).to_uint() == 0x1))){ //firewall for ReadOp
	      sc_uint<32> my_address = new_data.range(31,22).to_uint();
	      if (fw_read_r[6].to_bool()){ //NoC Mod
	        if ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >=  fw_read_l)){ //the range of address is between
	    	  if ((fw_read_r[3].to_bool() && new_data.range(17,16).to_uint() == 0x3) ||
	    		  (fw_read_r[2].to_bool() && new_data.range(17,16).to_uint() == 0x2) ||
	      		  (fw_read_r[1].to_bool() && new_data.range(17,16).to_uint() == 0x1) ||
	      		  (fw_read_r[0].to_bool() && new_data.range(17,16).to_uint() == 0x0)){
	      	    fw_hit = true;
	    	  }
	    	}
	      }else{ //compatibility mode
            //physical address mode
	    	if (new_data.range(17,16).to_uint() == 0x0){ //go to port 00 -> 0
	    	  my_address += 0x40000000;
	    	}else if (new_data.range(17,16).to_uint() == 0x1){ //go to port 01 -> 1
	    	  my_address += 0x40001000;
	    	}else if (new_data.range(17,16).to_uint() == 0x2){ //go to port 10 -> 2
	    	  my_address += 0x40002000;
	    	}else if (new_data.range(17,16).to_uint() == 0x3){ //go to port 11 -> 3
	    	  my_address += 0x40003000;
	    	}else{
	    	  fw_hit = true;
              cout << name() << ".SW1.p1: While Working in Physical Address mode no valid input address "<< endl;
	    	}
            cout << name() << ".SW1.p1: Checking if address: 0x"<< hex << my_address.to_uint() << dec << " is between 0x"
	    		   << hex << fw_read_l << dec << " and 0x" << hex << fw_read_h << dec << endl;
	        if ((!fw_hit) && ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >= fw_read_l))){
	               fw_hit = true;
	        }
          }
	    }
	  }
      if (fw_hit){
	    if(new_data.range(21,20).to_uint() == 0x1){ //if read operation set response to 0x0
	      init_responce_fifo_1.write(0x00000000);
	    }
	    rule_hit_counter_1++;
	    fw_rule_hit_counter_1.write(rule_hit_counter_1);

        cout << name() << ".SW1.p1: Requested Packet " << hex << new_data.to_uint() << dec  << " Matched FW Rule [" << rule_hit_counter_1 << "]" << endl;
//	    cout << "**** m_write_ended_event NOTIFY (FWRULED)****" << endl;
        m_write_ended_event.notify();
        wait(1); //nesseary for the fifos to be ready

	    continue;
	 }
	 if ((responce_s1.read()[31].to_bool()) && (new_data.range(21,20).to_uint() == 0x1)){
	   count_droped_1 = count_droped_1 + 1;
	   droped_1.write(count_droped_1);

       cout << name() << ".SW1.p1: Requested Write Op while pending result. Packet Dropped [" << count_droped_1 << "]: 0x" << hex << new_data.to_uint() << dec << endl;
//			cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
			m_write_ended_event.notify();	  

     }else{
	   if (in_fifo_1.num_free() > 0){
	     if (new_data.range(21,20).to_uint() == 0x1){ //for read operations
	       new_data.range(19,18) = 0x1; //set source port
	       init_responce_fifo_1.write(0x80000000); //set 31 bit to 1 while the rest to 0 this mean expecting result
	     }
	      in_fifo_1.write(new_data);

	      cout << name() << ".SW1.p1: Packet accepted in in FIFO 1 [" << in_fifo_1.num_available() << "]: 0x" << hex << new_data.to_uint() << dec << endl;

	   }else{
	     count_droped_1 = count_droped_1 + 1;
	     droped_1.write(count_droped_1);

	     cout << name() << ".SW1.p1: Packet Dropped [" << count_droped_1 << "]: 0x" << hex << new_data.to_uint() << dec << endl;
//			cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
			m_write_ended_event.notify();	  

	    }
	  }
      wait(1); //nesseary for the fifos to be ready
    }
  }
}

void firewall_with_mem::interface_in_2()
{
  unsigned int preval = 0x0;
  unsigned int pre_read_r = 0x0;
  unsigned int fw_read_l = 0x0;
  unsigned int fw_read_h = 0x0;
  sc_uint<32> fw_read_r = 0x0;
  fw_hreg_p_2.write(0x0);
  fw_lreg_p_2.write(0x0);
  fw_rreg_p_2.write(0x0);

  count_total_2 = 0x0;
  rule_hit_counter_2 = 0x0;
  count_droped_2 = 0x0;
  total_2.write(0x0);
  droped_2.write(0x0);
  fw_rule_hit_counter_2.write(0x0);
  wait();
  while(true)
  {
    wait();
	sc_uint<32> new_fw = fw_rule_range_2.read();
	if (new_fw.to_uint() != pre_read_r){
	  pre_read_r = new_fw.to_uint();
	  fw_read_r = new_fw;
	  fw_read_l = fw_lower_range_2.read().to_uint();
	  fw_read_h = fw_upper_range_2.read().to_uint();
	  //this is debug for verify
	  fw_hreg_p_2.write(fw_read_h);
	  fw_lreg_p_2.write(fw_read_l);
	  fw_rreg_p_2.write(fw_read_r);
	  continue;
	}
	sc_uint<32> new_data;
    if (!in_2.nb_read(new_data)){
		continue;
	}
	if (new_data.to_uint() != preval){
	  preval = new_data.to_uint();
	  count_total_2 = count_total_2 + 1;
	  total_2.write(count_total_2);

      cout << name() << ".SW2.p0: Received data " << count_total_2 << ": 0x" << hex << new_data.to_uint() << dec << endl;

      //check for firewall rule
	  //Rule register (bit active - stands for):
	  //7 -> 1: Enable Rule
	  //6 -> 1: Enabled NoC Mode // 0: Enable Compatibility Mode
	  //5 -> 1: Enable Rule for Write OpCode
	  //4 -> 1: Enable Rule for Read OpCode
	  //3 -> 1: Enable Rule for Destination Port 3
	  //2 -> 1: Enable Rule for Destination Port 2
	  //1 -> 1: Enable Rule for Destination Port 1
	  //0 -> 1: Enable Rule for Destination Port 0
	  bool fw_hit = false;
	  if (fw_read_r[7].to_bool()){
	    if ((fw_read_r[5].to_bool() && (new_data.range(21,20).to_uint() == 0x0)) || //firewall for writeOp
	        (fw_read_r[4].to_bool() && (new_data.range(21,20).to_uint() == 0x1))){ //firewall for ReadOp
	      sc_uint<32> my_address = new_data.range(31,22).to_uint();
	      if (fw_read_r[6].to_bool()){ //NoC Mod
	        if ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >=  fw_read_l)){ //the range of address is between
	    	  if ((fw_read_r[3].to_bool() && new_data.range(17,16).to_uint() == 0x3) ||
	    		  (fw_read_r[2].to_bool() && new_data.range(17,16).to_uint() == 0x2) ||
	      		  (fw_read_r[1].to_bool() && new_data.range(17,16).to_uint() == 0x1) ||
	      		  (fw_read_r[0].to_bool() && new_data.range(17,16).to_uint() == 0x0)){
	      	    fw_hit = true;
	    	  }
	    	}
	      }else{ //compatibility mode
            //physical address mode
	    	if (new_data.range(17,16).to_uint() == 0x0){ //go to port 00 -> 0
	    	  my_address += 0x40000000;
	    	}else if (new_data.range(17,16).to_uint() == 0x1){ //go to port 01 -> 1
	    	  my_address += 0x40001000;
	    	}else if (new_data.range(17,16).to_uint() == 0x2){ //go to port 10 -> 2
	    	  my_address += 0x40002000;
	    	}else if (new_data.range(17,16).to_uint() == 0x3){ //go to port 11 -> 3
	    	  my_address += 0x40003000;
	    	}else{
	    	  fw_hit = true;

              cout << name() << ".SW2.p0: While Working in Physical Address mode no valid input address "<< endl;

	    	}

            cout << name() << ".SW2.p0: Checking if address: 0x"<< hex << my_address.to_uint() << dec << " is between 0x"
				 << hex << fw_read_l << dec << " and 0x" << hex << fw_read_h << dec << endl;
	        if ((!fw_hit) && ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >= fw_read_l))){
	             fw_hit = true;
            }
	      }
	    }
	  }
      if (fw_hit){
	    if(new_data.range(21,20).to_uint() == 0x1){ //if read operation set response to 0x0
	      init_responce_fifo_2.write(0x00000000);
	    }
	    rule_hit_counter_2++;
	    fw_rule_hit_counter_2.write(rule_hit_counter_2);

        cout << name() << ".SW2.p0: Requested Packet " << hex << new_data.to_uint() << dec  << " Matched FW Rule [" << rule_hit_counter_2 << "]" << endl;
//	    cout << "**** m_write_ended_event NOTIFY (FWRULED)****" << endl;
        m_write_ended_event.notify();
        wait(1); //nesseary for the fifos to be ready

	    continue;
	 }
	 if ((responce_s2.read()[31].to_bool()) && (new_data.range(21,20).to_uint() == 0x1)){
	   count_droped_2 = count_droped_2 + 1;
	   droped_2.write(count_droped_2);

       cout << name() << ".SW2.p0: Requested Write Op while pending result. Packet Dropped [" << count_droped_2 << "]: 0x" << hex << new_data.to_uint() << dec << endl;
//			cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
			m_write_ended_event.notify();	  

     }else{
	   if (in_fifo_2.num_free() > 0){
	     if (new_data.range(21,20).to_uint() == 0x1){ //for read operations
	       new_data.range(19,18) = 0x2; //set source port
	       init_responce_fifo_2.write(0x80000000); //set 31 bit to 1 while the rest to 0 this mean expecting result
	     }
	      in_fifo_2.write(new_data);

	      cout << name() << ".SW2.p0: Packet accepted in in FIFO 1 [" << in_fifo_2.num_available() << "]: 0x" << hex << new_data.to_uint() << dec << endl;

	   }else{
	     count_droped_2 = count_droped_2 + 1;
	     droped_2.write(count_droped_2);

	     cout << name() << ".SW2.p0: Packet Dropped [" << count_droped_2 << "]: 0x" << hex << new_data.to_uint() << dec << endl;
//			cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
			m_write_ended_event.notify();	  

	    }
	  }
      wait(1); //nesseary for the fifos to be ready
    }
  }
}

void firewall_with_mem::interface_in_3()
{
  unsigned int preval = 0x0;
  unsigned int pre_read_r = 0x0;
  unsigned int fw_read_l = 0x0;
  unsigned int fw_read_h = 0x0;
  sc_uint<32> fw_read_r = 0x0;
  fw_hreg_p_3.write(0x0);
  fw_lreg_p_3.write(0x0);
  fw_rreg_p_3.write(0x0);

  count_total_3 = 0x0;
  rule_hit_counter_3 = 0x0;
  count_droped_3 = 0x0;
  total_3.write(0x0);
  droped_3.write(0x0);
  fw_rule_hit_counter_3.write(0x0);
  wait();
  while(true)
  {
    wait();
	sc_uint<32> new_fw = fw_rule_range_3.read();
	if (new_fw.to_uint() != pre_read_r){
	  pre_read_r = new_fw.to_uint();
	  fw_read_r = new_fw;
	  fw_read_l = fw_lower_range_3.read().to_uint();
	  fw_read_h = fw_upper_range_3.read().to_uint();
	  //this is debug for verify
	  fw_hreg_p_3.write(fw_read_h);
	  fw_lreg_p_3.write(fw_read_l);
	  fw_rreg_p_3.write(fw_read_r);
	  continue;
	}
	sc_uint<32> new_data;
    if (!in_3.nb_read(new_data)){
		continue;
	}
	if (new_data.to_uint() != preval){
	  preval = new_data.to_uint();
	  count_total_3 = count_total_3 + 1;
	  total_3.write(count_total_3);

      cout << name() << ".SW2.p1: Received data " << count_total_3 << ": 0x" << hex << new_data.to_uint() << dec << endl;

      //check for firewall rule
	  //Rule register (bit active - stands for):
	  //7 -> 1: Enable Rule
	  //6 -> 1: Enabled NoC Mode // 0: Enable Compatibility Mode
	  //5 -> 1: Enable Rule for Write OpCode
	  //4 -> 1: Enable Rule for Read OpCode
	  //3 -> 1: Enable Rule for Destination Port 3
	  //2 -> 1: Enable Rule for Destination Port 2
	  //1 -> 1: Enable Rule for Destination Port 1
	  //0 -> 1: Enable Rule for Destination Port 0
	  bool fw_hit = false;
	  if (fw_read_r[7].to_bool()){
	    if ((fw_read_r[5].to_bool() && (new_data.range(21,20).to_uint() == 0x0)) || //firewall for writeOp
	        (fw_read_r[4].to_bool() && (new_data.range(21,20).to_uint() == 0x1))){ //firewall for ReadOp
	      sc_uint<32> my_address = new_data.range(31,22).to_uint();
	      if (fw_read_r[6].to_bool()){ //NoC Mod
	        if ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >=  fw_read_l)){ //the range of address is between
	    	  if ((fw_read_r[3].to_bool() && new_data.range(17,16).to_uint() == 0x3) ||
	    		  (fw_read_r[2].to_bool() && new_data.range(17,16).to_uint() == 0x2) ||
	      		  (fw_read_r[1].to_bool() && new_data.range(17,16).to_uint() == 0x1) ||
	      		  (fw_read_r[0].to_bool() && new_data.range(17,16).to_uint() == 0x0)){
	      	    fw_hit = true;
	    	  }
	    	}
	      }else{ //compatibility mode
            //physical address mode
	    	if (new_data.range(17,16).to_uint() == 0x0){ //go to port 00 -> 0
	    	  my_address += 0x40000000;
	    	}else if (new_data.range(17,16).to_uint() == 0x1){ //go to port 01 -> 1
	    	  my_address += 0x40001000;
	    	}else if (new_data.range(17,16).to_uint() == 0x2){ //go to port 10 -> 2
	    	  my_address += 0x40002000;
	    	}else if (new_data.range(17,16).to_uint() == 0x3){ //go to port 11 -> 3
	    	  my_address += 0x40003000;
	    	}else{
	    	  fw_hit = true;

              cout << name() << ".SW2.p1: While Working in Physical Address mode no valid input address "<< endl;

	    	}

            cout << name() << ".SW2.p1: Checking if address: 0x"<< hex << my_address.to_uint() << dec << " is between 0x"
      		     << hex << fw_read_l << dec << " and 0x" << hex << fw_read_h << dec << endl;

	        if ((!fw_hit) && ((fw_read_h >= my_address.to_uint()) && (my_address.to_uint() >= fw_read_l))){
	             fw_hit = true;
	        }
          }
	    }
	  }
      if (fw_hit){
	    if(new_data.range(21,20).to_uint() == 0x1){ //if read operation set response to 0x0
	      init_responce_fifo_3.write(0x00000000);
	    }
	    rule_hit_counter_3++;
	    fw_rule_hit_counter_3.write(rule_hit_counter_3);

        cout << name() << ".SW2.p1: Requested Packet " << hex << new_data.to_uint() << dec  << " Matched FW Rule [" << rule_hit_counter_3 << "]" << endl;
//	    cout << "**** m_write_ended_event NOTIFY (FWRULED)****" << endl;
        m_write_ended_event.notify();
        wait(1); //nesseary for the fifos to be ready

	    continue;
	 }
	 if ((responce_s3.read()[31].to_bool()) && (new_data.range(21,20).to_uint() == 0x1)){
	   count_droped_3 = count_droped_3 + 1;
	   droped_3.write(count_droped_3);

       cout << name() << ".SW2.p1: Requested Write Op while pending result. Packet Dropped [" << count_droped_3 << "]: 0x" << hex << new_data.to_uint() << dec << endl;
//			cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
			m_write_ended_event.notify();	  

     }else{
	   if (in_fifo_3.num_free() > 0){
	     if (new_data.range(21,20).to_uint() == 0x1){ //for read operations
	       new_data.range(19,18) = 0x3; //set source port
	       init_responce_fifo_3.write(0x80000000); //set 31 bit to 1 while the rest to 0 this mean expecting result
	     }
	      in_fifo_3.write(new_data);

	      cout << name() << ".SW2.p1: Packet accepted in in FIFO 1 [" << in_fifo_3.num_available() << "]: 0x" << hex << new_data.to_uint() << dec << endl;

	   }else{
	     count_droped_3 = count_droped_3 + 1;
	     droped_3.write(count_droped_3);

	     cout << name() << ".SW2.p1: Packet Dropped [" << count_droped_3 << "]: 0x" << hex << new_data.to_uint() << dec << endl;
//			cout << "**** m_write_ended_event NOTIFY (FWDROPED)****" << endl;
			m_write_ended_event.notify();	  

	    }
	  }
      wait(1); //nesseary for the fifos to be ready
    }
  }
}


void firewall_with_mem::interface_out_0()
{

  preready[0] = false;
  bool is_write_op = false;
  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data0;
    if (!done_0.read() && !preready[0]){
      if (out_fifo_0.nb_read(out_data0)){
              unsigned int new_address = 0x40000000;
              new_address += out_data0.range(31,22).to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_data0.range(21,20).to_uint() == 0x1){
				is_write_op = false;
                new_data = out_0->read(new_address);
                new_data.range(31,22) = out_data0.range(31,22).to_uint();
                new_data.range(21,20) = 0x2;
                new_data.range(19,18) = out_data0.range(17,16);
                new_data.range(17,16) = out_data0.range(19,18);
                responce_fifo_0.write(new_data);
              }else{
				is_write_op = true;
                new_data.range(15,0) = out_data0.range(15,0);
                out_0->write(new_address, &new_data);
                cout << name() << "PORT 0 PCKT OUT: 0x" << hex <<  out_data0 << dec << endl;
                out_addr_to_read_0.write(new_address);
                ready_0.write(true);
                preready[0] = true;
              }
      }
    }else if (done_0.read() && preready[0]){
      ready_0.write(false);
      preready[0] = false;
      if (is_write_op){
//		cout << "**** m_write_ended_event NOTIFY (FWWRITE)****" << endl;
		m_write_ended_event.notify();
	  }
    }

  }
}

void firewall_with_mem::interface_out_1()
{

  preready[1] = false;
bool is_write_op = false;
  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data1;

    if (!done_1.read() && !preready[1]){

      if (out_fifo_1.nb_read(out_data1)){
              unsigned int new_address = 0x40001000;
              new_address += out_data1.range(31,22).to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_data1.range(21,20).to_uint() == 0x1){
				  is_write_op = false;
                new_data = out_1->read(new_address);
                new_data.range(31,22) = out_data1.range(31,22).to_uint();
                new_data.range(21,20) = 0x2;
                new_data.range(19,18) = out_data1.range(17,16);
                new_data.range(17,16) = out_data1.range(19,18);
                responce_fifo_1.write(new_data);
              }else{
				  is_write_op = true;
                new_data.range(15,0) = out_data1.range(15,0);
                out_1->write(new_address, &new_data);
                cout << name() << "PORT 1 PCKT OUT: 0x" << hex <<  out_data1 << dec << endl;
                out_addr_to_read_1.write(new_address);
                ready_1.write(true);
                preready[1] = true;

              }

      }
    }else if (done_1.read() && preready[1]){
              ready_1.write(false);
              preready[1] = false;
      if (is_write_op){
//		cout << "**** m_write_ended_event NOTIFY (FWWRITE)****" << endl;
		m_write_ended_event.notify();
	  }

    }
  }
}

void firewall_with_mem::interface_out_2()
{

  preready[2] = false;
bool is_write_op = false;

  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data2;

    if (!done_2.read() && !preready[2]){

      if (out_fifo_2.nb_read(out_data2)){
              unsigned int new_address = 0x40002000;
              new_address += out_data2.range(31,22).to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_data2.range(21,20).to_uint() == 0x1){
				  is_write_op = false;
                new_data = out_2->read(new_address);
                new_data.range(31,22) = out_data2.range(31,22).to_uint();
                new_data.range(21,20) = 0x2;
                new_data.range(19,18) = out_data2.range(17,16);
                new_data.range(17,16) = out_data2.range(19,18);
                responce_fifo_2.write(new_data);
              }else{
				  is_write_op = true;
                new_data.range(15,0) = out_data2.range(15,0);
                out_2->write(new_address, &new_data);

                cout << name() << "PORT 2 PCKT OUT: 0x" << hex <<  out_data2 << dec << endl;


                out_addr_to_read_2.write(new_address);
                ready_2.write(true);
                preready[2] = true;

              }

      }
    }else if (done_2.read() && preready[2]){
              ready_2.write(false);
              preready[2] = false;
      if (is_write_op){
//		cout << "**** m_write_ended_event NOTIFY (FWWRITE)****" << endl;
		m_write_ended_event.notify();
	  }

    }
  }
}

void firewall_with_mem::interface_out_3()
{

  preready[3] = false;
bool is_write_op = false;

  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data3;

    if (!done_3.read() && !preready[3]){

      if (out_fifo_3.nb_read(out_data3)){
              unsigned int new_address = 0x40003000;
              new_address += out_data3.range(31,22).to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_data3.range(21,20).to_uint() == 0x1){
        				  is_write_op = false;
				new_data = out_3->read(new_address);
                new_data.range(31,22) = out_data3.range(31,22).to_uint();
                new_data.range(21,20) = 0x2;
                new_data.range(19,18) = out_data3.range(17,16);
                new_data.range(17,16) = out_data3.range(19,18);
                responce_fifo_3.write(new_data);
              }else{
 				  is_write_op = true;
               new_data.range(15,0) = out_data3.range(15,0);
                out_3->write(new_address, &new_data);

                cout << name() << "PORT 3 PCKT OUT: 0x" << hex <<  out_data3 << dec << endl;


                out_addr_to_read_3.write(new_address);
                ready_3.write(true);
                preready[3] = true;

              }

      }
    }else if (done_3.read() && preready[3]){
              ready_3.write(false);
              preready[3] = false;
      if (is_write_op){
//		cout << "**** m_write_ended_event NOTIFY (FWWRITE)****" << endl;
		m_write_ended_event.notify();
	  }

    }
  }
}

void firewall_with_mem::entry_sw_1(){
  wait();
  while(true)
  {
    wait();
    sc_uint<32> new_data0;
    if (in_fifo_0.nb_read(new_data0)){
      pkt new_packet;
      new_packet.addr_offset = new_data0.range(31,22).to_uint();
      new_packet.opcode = new_data0.range(21,20).to_uint();
      new_packet.src = new_data0.range(19,18).to_uint();
      new_packet.dest = new_data0.range(17,16).to_uint();
      new_packet.data = new_data0.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data0[17] = new_data0[16];
      new_data0[16] = next_position;
      if (!next_position){
        out_fifo_s1_0.nb_write(new_data0);

        cout << name() << ".SW1: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_s1_1.nb_write(new_data0);

        cout << name() << ".SW1: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
      wait();
    }
    sc_uint<32> new_data1;
    if (in_fifo_1.nb_read(new_data1)){
      pkt new_packet;
      new_packet.addr_offset = new_data1.range(31,22).to_uint();
      new_packet.opcode = new_data1.range(21,20).to_uint();
      new_packet.src = new_data1.range(19,18).to_uint();
      new_packet.dest = new_data1.range(17,16).to_uint();
      new_packet.data = new_data1.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data1[17] = new_data1[16];
      new_data1[16] = next_position;
      if (!next_position){
        out_fifo_s1_0.nb_write(new_data1);

        cout << name() << ".SW1: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_s1_1.nb_write(new_data1);

        cout << name() << ".SW1: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
    }
  }
}

void firewall_with_mem::entry_sw_2(){
  wait();
  while(true)
  {
    wait();
    sc_uint<32> new_data0;
    if (in_fifo_2.nb_read(new_data0)){
      pkt new_packet;
      new_packet.addr_offset = new_data0.range(31,22).to_uint();
      new_packet.opcode = new_data0.range(21,20).to_uint();
      new_packet.src = new_data0.range(19,18).to_uint();
      new_packet.dest = new_data0.range(17,16).to_uint();
      new_packet.data = new_data0.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data0[17] = new_data0[16];
      new_data0[16] = next_position;
      if (!next_position){
        out_fifo_s2_0.nb_write(new_data0);

        cout << name() << ".SW2: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_s2_1.nb_write(new_data0);

        cout << name() << ".SW2: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
      wait();
    }
    sc_uint<32> new_data1;
    if (in_fifo_3.nb_read(new_data1)){
      pkt new_packet;
      new_packet.addr_offset = new_data1.range(31,22).to_uint();
      new_packet.opcode = new_data1.range(21,20).to_uint();
      new_packet.src = new_data1.range(19,18).to_uint();
      new_packet.dest = new_data1.range(17,16).to_uint();
      new_packet.data = new_data1.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data1[17] = new_data1[16];
      new_data1[16] = next_position;
      if (!next_position){
        out_fifo_s2_0.nb_write(new_data1);

        cout << name() << ".SW2: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_s2_1.nb_write(new_data1);

        cout << name() << ".SW2: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
    }
  }
}

void firewall_with_mem::entry_sw_3(){
  wait();
  while(true)
  {
    wait();
    sc_uint<32> new_data0;
    if (out_fifo_s1_0.nb_read(new_data0)){
      pkt new_packet;
      new_packet.addr_offset = new_data0.range(31,22).to_uint();
      new_packet.opcode = new_data0.range(21,20).to_uint();
      new_packet.src = new_data0.range(19,18).to_uint();
      new_packet.dest = new_data0.range(17,16).to_uint();
      new_packet.data = new_data0.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data0[17] = new_data0[16];
      new_data0[16] = next_position;
      if (!next_position){
        out_fifo_0.nb_write(new_data0);

        cout << name() << ".SW3: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_1.nb_write(new_data0);

        cout << name() << ".SW3: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
      wait();
    }
    sc_uint<32> new_data1;
    if (out_fifo_s2_0.nb_read(new_data1)){
      pkt new_packet;
      new_packet.addr_offset = new_data1.range(31,22).to_uint();
      new_packet.opcode = new_data1.range(21,20).to_uint();
      new_packet.src = new_data1.range(19,18).to_uint();
      new_packet.dest = new_data1.range(17,16).to_uint();
      new_packet.data = new_data1.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data1[17] = new_data1[16];
      new_data1[16] = next_position;
      if (!next_position){
        out_fifo_0.nb_write(new_data1);

        cout << name() << ".SW3: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_1.nb_write(new_data1);

        cout << name() << ".SW3: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
    }
  }
}

void firewall_with_mem::entry_sw_4(){
  wait();
  while(true)
  {
    wait();
    sc_uint<32> new_data0;
    if (out_fifo_s1_1.nb_read(new_data0)){
      pkt new_packet;
      new_packet.addr_offset = new_data0.range(31,22).to_uint();
      new_packet.opcode = new_data0.range(21,20).to_uint();
      new_packet.src = new_data0.range(19,18).to_uint();
      new_packet.dest = new_data0.range(17,16).to_uint();
      new_packet.data = new_data0.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data0[17] = new_data0[16];
      new_data0[16] = next_position;
      if (!next_position){
        out_fifo_2.nb_write(new_data0);

        cout << name() << ".SW4: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_3.nb_write(new_data0);

        cout << name() << ".SW4: Packet " << "[0x" << hex << new_data0.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
      wait();
    }
    sc_uint<32> new_data1;
    if (out_fifo_s2_1.nb_read(new_data1)){
      pkt new_packet;
      new_packet.addr_offset = new_data1.range(31,22).to_uint();
      new_packet.opcode = new_data1.range(21,20).to_uint();
      new_packet.src = new_data1.range(19,18).to_uint();
      new_packet.dest = new_data1.range(17,16).to_uint();
      new_packet.data = new_data1.range(15,0).to_uint();
      //find next exit port and rotate
      bool next_position = new_packet.dest[1].to_bool();
      new_data1[17] = new_data1[16];
      new_data1[16] = next_position;
      if (!next_position){
        out_fifo_2.nb_write(new_data1);

        cout << name() << ".SW4: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 0 [" << out_fifo_0.num_available() << "]" << endl;

      }else{
        out_fifo_3.nb_write(new_data1);

        cout << name() << ".SW4: Packet " << "[0x" << hex << new_data1.to_uint() << dec << "] Moved from In fifo 0 to out fifo 1 [" << out_fifo_1.num_available() << "]" << endl;

      }
    }
  }
}

void firewall_with_mem::entry_response(){
  responce0 = 0x0;
  responce1 = 0x0;
  responce2 = 0x0;
  responce3 = 0x0;
  responce_s0.write(0x0);
  responce_s1.write(0x0);
  responce_s2.write(0x0);
  responce_s3.write(0x0);
  wait();
  while(true)
  {
    wait();
    sc_uint<32> new_data0;
    if (init_responce_fifo_0.nb_read(new_data0)){
      responce0 = new_data0.to_uint();
      responce_s0.write(responce0);
      in_responce_0.write(responce0);

      cout << name() << ".SW1.p0: Init Response Packet Was written" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce0.to_uint() << dec << endl;

      wait();
    }else if (responce_fifo_0.nb_read(new_data0)){
      responce0 = new_data0.to_uint();
      responce0[31] = false;
      if (responce0.range(17,16) == 0x0){
        responce_s0.write(responce0);
        in_responce_0.write(responce0);

        cout << name() << ".SW1.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce0.to_uint() << dec << endl;

      }else if (responce0.range(17,16) == 0x1){
        responce_s1.write(responce0);
        in_responce_1.write(responce0);

        cout << name() << ".SW1.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce0.to_uint() << dec << endl;

      }else if (responce0.range(17,16) == 0x2){
        responce_s2.write(responce0);
        in_responce_2.write(responce0);

        cout << name() << ".SW2.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce0.to_uint() << dec << endl;

      }else if (responce0.range(17,16) == 0x3){
        responce_s3.write(responce0);
        in_responce_3.write(responce0);

        cout << name() << ".SW2.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce0.to_uint() << dec << endl;

      }
	  m_write_ended_event.notify();
      wait();
    }
    sc_uint<32> new_data1;
    if (init_responce_fifo_1.nb_read(new_data1)){
      responce1 = new_data1.to_uint();
      responce_s1.write(responce1);
      in_responce_1.write(responce1);

      cout << name() << ".SW1.p1: Init Response Packet Was written" << "[0x" << hex << new_data1.to_uint() << dec << "]: 0x" << hex << responce1.to_uint() << dec << endl;

      wait();
    }else if (responce_fifo_1.nb_read(new_data1)){
      responce1 = new_data1.to_uint();
      responce1[31] = false;
      if (responce1.range(17,16) == 0x0){
        responce_s0.write(responce1);
        in_responce_0.write(responce1);

        cout << name() << ".SW1.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce1.to_uint() << dec << endl;

      }else if (responce1.range(17,16) == 0x1){
        responce_s1.write(responce1);
        in_responce_1.write(responce1);

        cout << name() << ".SW1.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce1.to_uint() << dec << endl;

      }else if (responce1.range(17,16) == 0x2){
        responce_s2.write(responce1);
        in_responce_2.write(responce1);

        cout << name() << ".SW2.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce1.to_uint() << dec << endl;

      }else if (responce1.range(17,16) == 0x3){
        responce_s3.write(responce1);
        in_responce_3.write(responce1);

        cout << name() << ".SW2.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce1.to_uint() << dec << endl;

      }
	  m_write_ended_event.notify();
      wait();
    }
    sc_uint<32> new_data2;
    if (init_responce_fifo_2.nb_read(new_data2)){
      responce2 = new_data2.to_uint();
      responce_s2.write(responce2);
      in_responce_2.write(responce2);

      cout << name() << ".SW2.p0: Init Response Packet Was written" << "[0x" << hex << new_data2.to_uint() << dec << "]: 0x" << hex << responce2.to_uint() << dec << endl;

      wait();
    }else if (responce_fifo_2.nb_read(new_data2)){
      responce2 = new_data2.to_uint();
      responce2[31] = false;
      if (responce2.range(17,16) == 0x0){
        responce_s0.write(responce2);
        in_responce_0.write(responce2);

        cout << name() << ".SW1.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce2.to_uint() << dec << endl;

      }else if (responce2.range(17,16) == 0x1){
        responce_s1.write(responce2);
        in_responce_1.write(responce2);

        cout << name() << ".SW1.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce2.to_uint() << dec << endl;

      }else if (responce2.range(17,16) == 0x2){
        responce_s2.write(responce2);
        in_responce_2.write(responce2);

        cout << name() << ".SW2.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce2.to_uint() << dec << endl;

      }else if (responce2.range(17,16) == 0x3){
        responce_s3.write(responce2);
        in_responce_3.write(responce2);

        cout << name() << ".SW2.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce2.to_uint() << dec << endl;

      }
	  m_write_ended_event.notify();
      wait();
    }
    sc_uint<32> new_data3;
    if (init_responce_fifo_3.nb_read(new_data3)){
      responce3 = new_data3.to_uint();
      responce_s3.write(responce3);
      in_responce_3.write(responce3);

      cout << name() << ".SW2.p1: Init Response Packet Was written" << "[0x" << hex << new_data3.to_uint() << dec << "]: 0x" << hex << responce3.to_uint() << dec << endl;

      wait();
    }else if (responce_fifo_3.nb_read(new_data3)){
      responce3 = new_data3.to_uint() & 0x7FFFFFFF;
      responce3[31] = false;
      if (responce3.range(17,16) == 0x0){
        responce_s0.write(responce3);
        in_responce_0.write(responce3);

        cout << name() << ".SW1.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce3.to_uint() << dec << endl;

      }else if (responce3.range(17,16) == 0x1){
        responce_s1.write(responce3);
        in_responce_1.write(responce3);

        cout << name() << ".SW1.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce3.to_uint() << dec << endl;

      }else if (responce3.range(17,16) == 0x2){
        responce_s2.write(responce3);
        in_responce_2.write(responce3);

        cout << name() << ".SW2.p0: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce3.to_uint() << dec << endl;

      }else if (responce3.range(17,16) == 0x3){
        responce_s3.write(responce3);
        in_responce_3.write(responce3);

        cout << name() << ".SW2.p1: Response Packet Was prepared" << "[0x" << hex << new_data0.to_uint() << dec << "]: 0x" << hex << responce3.to_uint() << dec << endl;

      }
	  m_write_ended_event.notify();
      wait();
    }
  }
}




void firewall_with_mem::direct_mem_out_0()
{
  dir_mem_preready[0] = false;
  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data0;
    if (!dir_mem_done_0.read() && !dir_mem_preready[0]){
      if (dir_mem_out_fifo_0.nb_read(out_data0)){
              unsigned int new_address = out_address_0.to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_read_0){
                new_data = out_0->read(new_address);
                dir_mem_responce_fifo_0.write(new_data);
           		m_read_event_pending.notify();
              }else{
                new_data.range(15,0) = out_data0.range(15,0);
                out_0->write(new_address, &new_data);
                cout << name() << " DIRECT PORT 0 PCKT OUT: 0x" << hex <<  out_data0 << dec << endl;
                dir_mem_out_addr_to_read_0.write(new_address);
                dir_mem_ready_0.write(true);
                dir_mem_preready[0] = true;
              }
      }
    }else if (dir_mem_done_0.read() && dir_mem_preready[0]){
//			  cout << "**** m_write_ended_event NOTIFY (DIRECT )****" << endl;
			  m_write_ended_event.notify();
              dir_mem_ready_0.write(false);
              dir_mem_preready[0] = false;
    }
  }
}

void firewall_with_mem::direct_mem_out_1()
{
  dir_mem_preready[1] = false;
  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data1;
    if (!dir_mem_done_1.read() && !dir_mem_preready[1]){
      if (dir_mem_out_fifo_1.nb_read(out_data1)){
              unsigned int new_address = out_address_1.to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_read_1){
                new_data = out_1->read(new_address);
                dir_mem_responce_fifo_1.write(new_data);
           		m_read_event_pending.notify();
              }else{
                new_data.range(15,0) = out_data1.range(15,0);
                out_1->write(new_address, &new_data);
                cout << name() << " DIRECT PORT 1 PCKT OUT: 0x" << hex <<  out_data1 << dec << endl;
                dir_mem_out_addr_to_read_1.write(new_address);
                dir_mem_ready_1.write(true);
                dir_mem_preready[1] = true;
              }
      }
    }else if (dir_mem_done_1.read() && dir_mem_preready[1]){
//			  cout << "**** m_write_ended_event NOTIFY (DIRECT )****" << endl;
			  m_write_ended_event.notify();
              dir_mem_ready_1.write(false);
              dir_mem_preready[1] = false;
    }
  }
}

void firewall_with_mem::direct_mem_out_2()
{
  dir_mem_preready[2] = false;
  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data2;
    if (!dir_mem_done_2.read() && !dir_mem_preready[2]){
      if (dir_mem_out_fifo_2.nb_read(out_data2)){
              unsigned int new_address = out_address_2.to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_read_2){
                new_data = out_2->read(new_address);
                dir_mem_responce_fifo_2.write(new_data);
           		m_read_event_pending.notify();
              }else{
                new_data.range(15,0) = out_data2.range(15,0);
                out_2->write(new_address, &new_data);
                cout << name() << " DIRECT PORT 2 PCKT OUT: 0x" << hex <<  out_data2 << dec << endl;
                dir_mem_out_addr_to_read_2.write(new_address);
                dir_mem_ready_2.write(true);
                dir_mem_preready[2] = true;
              }
      }
    }else if (dir_mem_done_2.read() && dir_mem_preready[2]){
//			  cout << "**** m_write_ended_event NOTIFY (DIRECT )****" << endl;
			  m_write_ended_event.notify();
              dir_mem_ready_2.write(false);
              dir_mem_preready[2] = false;
    }
  }
}

void firewall_with_mem::direct_mem_out_3()
{
  dir_mem_preready[3] = false;
  wait();
  while(true)
  {
    wait();
    sc_uint<32> out_data3;
    if (!dir_mem_done_3.read() && !dir_mem_preready[3]){
      if (dir_mem_out_fifo_3.nb_read(out_data3)){
              unsigned int new_address = out_address_3.to_uint();
              sc_uint<32> new_data = 0x0;
              if (out_read_3){
                new_data = out_3->read(new_address);
                dir_mem_responce_fifo_3.write(new_data);
           		m_read_event_pending.notify();
              }else{
                new_data.range(15,0) = out_data3.range(15,0);
                out_3->write(new_address, &new_data);
                cout << name() << " DIRECT PORT 3 PCKT OUT: 0x" << hex <<  out_data3 << dec << endl;
                dir_mem_out_addr_to_read_3.write(new_address);
                dir_mem_ready_3.write(true);
                dir_mem_preready[3] = true;
              }
      }
    }else if (dir_mem_done_3.read() && dir_mem_preready[3]){
//			  cout << "**** m_write_ended_event NOTIFY (DIRECT )****" << endl;
			  m_write_ended_event.notify();
              dir_mem_ready_3.write(false);
              dir_mem_preready[3] = false;
    }
  }
}
