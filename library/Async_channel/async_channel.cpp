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
#include "async_channel.h"
#include <thread>
#include <mutex>

using namespace std;
using namespace sc_core;
using namespace sc_dt;

// Constructor
async_channel::async_channel(const char* instance_name)
: m_thread_did_push(false)
, m_sysc_did_put(false)
, m_thread_did_pull(false)
{
  m_mutex_wait_get.lock();
  m_mutex_wait_put.lock();
//  cout << name() << ":: "<< __func__ << ":: m_mutex_wait_put LOCKED in thread..." << endl;
}

// Destructor
async_channel::~async_channel(void)
{
}

void async_channel::push(async_packet_ptr async_payload_ptr)
{
  // Lockdown and place in queue
  std::lock_guard<std::mutex> protect(m_mutex_to_sysc);
  m_queue_to_sysc.push_front(async_payload_ptr);
  // Notify SystemC
  m_thread_did_push = true;
  async_request_update();
}

bool async_channel::can_get(void) const
{
  // Lockdown and determine status of queue
  std::lock_guard<std::mutex> protect(m_mutex_to_sysc);
  return not m_queue_to_sysc.empty();
}

void async_channel::get(async_packet_ptr& async_payload_ptr)
{
  while (not nb_get(async_payload_ptr)) wait(m_sysc_pulled_event);
}

async_packet_ptr& async_channel::get(void)
{
  async_packet_ptr* async_payload_ptr = new async_packet_ptr{0};
  get(*async_payload_ptr);
  return *async_payload_ptr;
}


bool async_channel::nb_get(async_packet_ptr& async_payload_ptr)
{
  // Lockdown and obtain from queue
  std::lock_guard<std::mutex> protect(m_mutex_to_sysc);
  if (m_queue_to_sysc.empty()) return false;
  async_payload_ptr = m_queue_to_sysc.back();
  m_queue_to_sysc.pop_back();
  // Release waiting threads
  m_mutex_wait_get.unlock();
//  m_mutex_wait_get.lock();
  return true;
}

void async_channel::wait_for_get(void) const
{
  m_mutex_wait_get.lock();
//  m_mutex_wait_get.unlock();
}

void async_channel::nb_put(async_packet_ptr async_payload_ptr)
{
  // Lockdown and push onto queue
  std::lock_guard<std::mutex> protect(m_mutex_fm_sysc);
  m_queue_fm_sysc.push_front(async_payload_ptr);
//  async_payload_ptr->print("DEBUG: ");
  // Notify thread
  m_sysc_did_put = true;
  m_mutex_wait_put.unlock();
//  m_mutex_wait_put.lock();
}

void async_channel::wait_for_put(void) const
{
//  cout << name() << ":: "<< __func__ << ":: Setting m_mutex_wait_put LOCKED..." << endl;
  m_mutex_wait_put.lock();
//  cout << name() << ":: "<< __func__ << ":: m_mutex_wait_put LOCKED in thread..." << endl;
//  m_mutex_wait_put.unlock();
//  cout << name() << ":: "<< __func__ << ":: m_mutex_wait_put UNlocked in thread..." << endl;
}

bool async_channel::can_pull(void) const
{
  // Lockdown and determine status of queue
  std::lock_guard<std::mutex> protect(m_mutex_fm_sysc);
  return not m_queue_fm_sysc.empty();
}

bool async_channel::nb_pull(async_packet_ptr& async_payload_ptr)
{
  // Determine if anything available and wait if need be
  // Lockdown and obtain from queue
  std::lock_guard<std::mutex> protect(m_mutex_fm_sysc);
  if (m_queue_fm_sysc.empty()) return false;
  async_payload_ptr = m_queue_fm_sysc.back();
//  async_payload_ptr->print("DEBUG: ");
  m_queue_fm_sysc.pop_back();
  // Notify SystemC
  m_thread_did_pull = true;
  m_sysc_did_put = false;
  async_request_update();
  return true;
}

const sc_core::sc_event& async_channel::sysc_pushed_event(void) const
{
  return m_sysc_pushed_event;
}

const sc_core::sc_event& async_channel::sysc_pulled_event(void) const
{
  return m_sysc_pulled_event;
}

void async_channel::update(void)
{
  { // Handle push
    std::lock_guard<std::mutex> protect(m_mutex_to_sysc);
    if (m_thread_did_push) {
      m_sysc_pushed_event.notify(SC_ZERO_TIME);
      m_thread_did_push = false;
    }
  }
  { // Handle get
    std::lock_guard<std::mutex> protect(m_mutex_fm_sysc);
    if (m_thread_did_pull) {
      m_sysc_pulled_event.notify(SC_ZERO_TIME);
      m_thread_did_pull = false;
    }
  }
}
