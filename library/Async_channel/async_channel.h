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
#ifndef ASYNC_CHANNEL_H
#define ASYNC_CHANNEL_H

#include "async_thread_if.h"
#include "async_sysc_if.h"
#include <list>
#include <mutex>

// Implements a channel to interface from an OS thread to SystemC. To clarify
// notation of who does what, we use push/pull from the thread side and
// put/get from the SystemC side. Notice the interfaces separate the two
// sides (SystemC vs asynchronous thread).
struct async_channel
: sc_core::sc_prim_channel
, virtual async_sysc_if
, virtual async_thread_if
{
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // Constructors
  //............................................................................
  async_channel(const char* instance_name);
  ~async_channel(void);

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // OS thread/process calls
  //............................................................................
  // NOTE: Push can block waiting for mutex, but should be brief
  void             push         (async_packet_ptr  async_payload_ptr) override;
  void             wait_for_get (void) const override;
  void             wait_for_put (void) const override;
  bool             can_pull     (void) const override;
  bool             nb_pull      (async_packet_ptr& async_payload_ptr) override;

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // SystemC calls
  //............................................................................
  bool             can_get      (void) const override;
  bool             nb_get       (async_packet_ptr& async_payload_ptr) override;
  void             get          (async_packet_ptr& async_payload_ptr) override;
  async_packet_ptr& get          (void) override;
  // NOTE: Put never blocks because unbounded queue
  void             nb_put       (async_packet_ptr async_payload_ptr) override;
  const sc_core::sc_event& sysc_pushed_event(void) const override;
  const sc_core::sc_event& sysc_pulled_event(void) const override;
  const sc_core::sc_event& default_event(void) const override { return sysc_pushed_event(); }
  void update(void) override;

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // Internal
  //............................................................................
private:
  std::list<async_packet_ptr>    m_queue_to_sysc;   //< data from thread
  bool                          m_thread_did_push; //< indicates push to above
  std::list<async_packet_ptr>    m_queue_fm_sysc;   //< data from systemC
  bool                          m_sysc_did_put;    //< indicates push to above
  bool                          m_thread_did_pull; //< indicates get from above
  sc_core::sc_event             m_sysc_pushed_event;  //< indicates thread put
  sc_core::sc_event             m_sysc_pulled_event;  //< indicates thread put
  mutable std::mutex            m_mutex_to_sysc;   //< locks shared structures
  mutable std::mutex            m_mutex_fm_sysc;   //< locks shared structures
  mutable std::mutex            m_mutex_wait_get;  //< wait for this
  mutable std::mutex            m_mutex_wait_put;  //< wait for this
};

#endif /*ASYNC_CHANNEL_H*/
