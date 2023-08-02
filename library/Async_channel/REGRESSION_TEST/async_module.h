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
#ifndef ASYNC_MODULE_H
#define ASYNC_MODULE_H

#include <systemc>
#include <thread>
#include <mutex>
#ifdef _REALTIME
#include "realtimify.h"
#endif

#include "async_channel.h"

struct async_module: sc_core::sc_module
{
public:
  // Constructor
  async_module(sc_core::sc_module_name instance_name);
  // Destructor
  virtual ~async_module(void);
  
  SC_HAS_PROCESS(async_module);
  
  // Channel to comunicate with OS thread
  async_channel m_async_channel;
  
  // Keep alive signal
  sc_core::sc_buffer<bool> m_keep_alive_signal;
  
  // SC_MODULE callbacks
  void before_end_of_elaboration(void) override;
  void end_of_elaboration(void) override;
  void start_of_simulation(void) override;
  void end_of_simulation(void) override;
  
  // SystemC processes
  void asysc_sysc_thread_process(void);
  void keep_alive_process(void);
  
private:
  // External OS thread
  void async_os_thread(async_channel& channel);
#ifdef _REALTIME
  realtimify *m_realtimify;
#endif
  // Signal handler
  typedef void (*sig_t) (int);
  static void sighandler(int sig);

  // Module attributes/local data
  std::mutex   m_allow_pthread; //< must be declared before m_lock_permission
  std::unique_ptr<std::lock_guard<std::mutex>> m_lock_permission; //< must be declared before m_pthread
  std::thread  m_pthread;
  int   s_stop_requests;

};

#endif //ASYNC_MODULE_H
