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
#ifndef ASYNC_THREAD_IF_H
#define ASYNC_THREAD_IF_H

#include "async_packet.h"

struct async_thread_if
{
  virtual void push(async_packet_ptr  async_payload_ptr) = 0;
  virtual bool can_pull(void) const = 0;
  virtual bool nb_pull(async_packet_ptr& async_payload_ptr) = 0;
  virtual void wait_for_get (void) const = 0;
  virtual void wait_for_put (void) const = 0;
};

#endif /*ASYNC_THREAD_IF_H*/
