/*
CoSiM Async Channel
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
#include "sighandler.h"
#include <iomanip>
#include <string>


#ifdef _REALTIME
#include "systemc.h"
#endif

#include "async_module.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;


async_module::async_module( sc_module_name instance_name): sc_module(instance_name), m_async_channel("m_async_channel"), m_keep_alive_signal("m_keep_alive_signal"), 
m_lock_permission(new std::lock_guard<std::mutex>(m_allow_pthread)), m_pthread(&async_module::async_os_thread,this,std::ref(m_async_channel))
{
  Sighandler::init();
  //----------------------------------------------------------------------------
  // Register processes
  //----------------------------------------------------------------------------
  this->s_stop_requests = 0;
  SC_HAS_PROCESS(async_module);
  SC_THREAD(asysc_sysc_thread_process);
  SC_THREAD(keep_alive_process);
#ifdef _REALTIME
  int rt_time = (int)_RT_TIME;
  sc_core::sc_time_unit rt_unit = (sc_core::sc_time_unit) _RT_UNIT;
  m_realtimify = new realtimify("realtimify", rt_time, rt_unit);
#endif
  m_pthread.detach(); //releasing the thread -> this fix the segfault error in the end
  cout << name() << ":: "<< __func__ << endl;
}

// Destructor 
async_module::~async_module(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

// Callbacks
void async_module::before_end_of_elaboration(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

void async_module::end_of_elaboration(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

void async_module::start_of_simulation(void) {
  cout << name() << ":: "<< __func__ << endl;
  m_keep_alive_signal.write(true);
  m_lock_permission.reset(nullptr);
}

void async_module::end_of_simulation(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

// External OS thread
void async_module::async_os_thread(async_channel& channel)
{
 
  cout << name() << ":: "<< __func__ << ":: Starting..." << endl;

  // Holding place for data
  uint8_t data_ptr[ASYNC_MAX_DATA_LEN];
  //----------------------------------------------------------------------------
  uint16_t i=1;
  
  cout << name() << ":: "<< __func__ << ":: Begining Event loop..." << endl;
  for(;;) {
    sleep(1);
    cout << name() << ":: "<< __func__ << "--------------------------------------" << endl;
    cout << name() << ":: "<< __func__ << ":: Time:" << time(NULL) << endl;

    //create fake data  for packet
    bzero(data_ptr,ASYNC_MAX_DATA_LEN); //< clear to aid debugging
    for (uint16_t j=0;j<i;j++) data_ptr[j] = 'A';

    //load the data to the packet
    async_command_t cmd = ASYNC_WRITE;
    if (i==10) cmd = ASYNC_EXIT;
    async_packet_ptr async_trans_ptr(new async_packet( cmd, (uint64_t)i, (i+1), data_ptr ));

    cout << name() << ":: "<< __func__ << ":: Sending to systemC: " << async_trans_ptr->str() << endl;

    // Send request to SystemC
    cout << name() << ":: "<< __func__ << ":: Pushing to async_channel..." << endl;
    channel.push(async_trans_ptr);

    // Wait for channel to pass payload to initiator_sysc_thread_process & return results
    cout << name() << ":: "<< __func__ << ":: Waiting for async_channel ..." << endl;
    channel.wait_for_put();

    // Pull response from SystemC
    cout << name() << ":: "<< __func__ << ":: Pulling from async_channel ..." << endl;
    if (not channel.nb_pull(async_trans_ptr)) {
      cout << "[CRITICAL ERROR] " << name() << ":: "<< __func__ << ":: Missing response" << endl;
      return;
    }
    cout << name() << ":: "<< __func__ << ":: Response from SystemC: " << async_trans_ptr->str() << endl;

    // Check for errors and adjust
    if (async_trans_ptr->status != ASYNC_OK_RESPONSE) {
      cout << "[ERROR] " << name() << ":: "<< __func__ << ":: Expecting ASYNC_OK_RESPONSE but obtained " << async_status_to_str(async_status_t(async_trans_ptr->status)) << endl;
    }

    // Exit if commanded
    if (async_trans_ptr->command == ASYNC_EXIT) {
      cout << name() << ":: "<< __func__ << ":: Exiting due to ASYNC_EXIT..." << endl;
      break;
    }
    i++;
  }

  cout << name() << ":: "<< __func__ << ":: Closing down ..." << endl;
  return;
}

// Processes <<
void async_module::asysc_sysc_thread_process(void)  {
  cout << name() << ":: "<< __func__ << endl;

  // Holding place for data
  uint8_t data_ptr[ASYNC_MAX_DATA_LEN];
  for(;;) {
    // Wait for data to arrive from remote
    m_keep_alive_signal.write(true); //< this could be removed iff we know for a certainty there is other traffic/computations
    wait(m_async_channel.sysc_pushed_event());
    cout << name() << ":: "<< __func__ << ":: Received sysc_pushed_event..." << endl;
    m_keep_alive_signal.write(false);
    // Setup place to receive incoming payload
    bzero(data_ptr,ASYNC_MAX_DATA_LEN); //< clear to aid debugging
    async_packet_ptr async_trans_ptr(new async_packet( ASYNC_IGNORE, 0, 0, data_ptr ));
    cout << name() << ":: "<< __func__ << ":: Getting Packet..." << endl;
    // Lockdown and obtain from incoming queue
    if (not m_async_channel.nb_get(async_trans_ptr)) {
      cout << "[ERROR] " << name() << ":: "<< __func__ << ":: Missing response" << endl;
    }
    //print the packet inside systemC
    cout << name() << ":: "<< __func__ << ":: Received in systemC: " << async_trans_ptr->str() << endl;

    // Update async_packet
    async_trans_ptr->status = ASYNC_OK_RESPONSE;

    // Lockdown and place in outgoing queue
    m_async_channel.nb_put(async_trans_ptr);
    cout << name() << ":: "<< __func__ << ":: Packet was put in Channel by systemC: " << async_trans_ptr->str() << endl;
    // Exit if commanded
    if (async_trans_ptr->command == ASYNC_EXIT) {
      cout << name() << ":: "<< __func__ << ":: Exiting due to ASYNC_EXIT..." << endl;
      break;
    }

  }
  wait(2,SC_SEC);
  cout << name() << ":: "<< __func__ << ":: Exiting asysc_sysc_thread_process..." << endl;
  sc_stop();
}//end async_module::initiator_sysc_thread_process()

// In the event there is nothing else happening, this process will keep the
// simulator from starving.
void async_module::keep_alive_process(void)  {
  cout << name() << ":: "<< __func__ << endl;
  for(;;) {
    if ( Sighandler::stop_requests() > 0 ) break;  // by apapa 
    if (!m_keep_alive_signal.read()) {
      wait(m_keep_alive_signal.default_event());
    }
    wait(SC_ZERO_TIME);
  }
  wait(1,SC_SEC);
  cout << name() << ":: "<< __func__ << ":: Exiting due to stop request ..." << endl;
  sc_stop();
  wait(1,SC_SEC);
}

