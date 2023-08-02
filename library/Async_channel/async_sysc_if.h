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
#ifndef ASYNC_SYSC_IF_H
#define ASYNC_SYSC_IF_H

#include "async_packet.h"
#include <systemc>

struct async_sysc_if : sc_core::sc_interface
{
  virtual bool nb_get(async_packet_ptr& async_payload_ptr)         = 0;
  virtual bool can_get(void) const                               = 0;
  virtual void get(async_packet_ptr& async_payload_ptr)            = 0;
  virtual async_packet_ptr& get(void)                             = 0;
  virtual void nb_put(async_packet_ptr async_payload_ptr)          = 0;
  virtual const sc_core::sc_event& default_event(void) const     = 0;
  virtual const sc_core::sc_event& sysc_pushed_event(void) const = 0;
  virtual const sc_core::sc_event& sysc_pulled_event(void) const = 0;
};

#endif /*ASYNC_SYSC_IF_H*/
