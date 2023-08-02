/*
CoSiM Interface Module
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

#include <time.h>

struct simple_async_packet{
    int32_t  id;
    uint8_t  command;
    uint8_t  status;
    uint32_t address;
    double systemc_start_time;
    double systemc_end_time;
    uint32_t data;
    struct timespec sender_out;
    struct timespec channel_in;
    struct timespec sc_in;
    struct timespec sc_out;
    struct timespec channel_out;
    struct timespec sender_in;
};
