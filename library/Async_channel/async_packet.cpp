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
#include "async_packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

using namespace std;

int32_t async_packet::s_id = 0;

const char* async_command_to_str(async_command_t s) {
  switch(s) {
    case ASYNC_IGNORE        : return "ASYNC_IGNORE";
    case ASYNC_READ          : return "ASYNC_READ";
    case ASYNC_WRITE         : return "ASYNC_WRITE";
    case ASYNC_DEBUG_READ    : return "ASYNC_DEBUG_READ";
    case ASYNC_DEBUG_WRITE   : return "ASYNC_DEBUG_WRITE";
    case ASYNC_MUTEX_LOCK    : return "ASYNC_DEBUG_WRITE";
    case ASYNC_MUTEX_TRYLOCK : return "ASYNC_MUTEX_TRYLOCK";
    case ASYNC_MUTEX_UNLOCK  : return "ASYNC_MUTEX_UNLOCK";
    case ASYNC_INTERRUPT     : return "ASYNC_INTERRUPT";
    case ASYNC_EXIT          : return "ASYNC_EXIT";
    default                 : return "*UNKNOWN_STATUS_ERROR*";
  }
}

const char* async_status_to_str(async_status_t s) {
  switch(s) {
    case ASYNC_INCOMPLETE_RESPONSE    : return "ASYNC_INCOMPLETE_RESPONSE";
    case ASYNC_OK_RESPONSE            : return "ASYNC_OK_RESPONSE";
    case ASYNC_ADDRESS_ERROR_RESPONSE : return "ASYNC_ADDRESS_ERROR_RESPONSE";
    case ASYNC_GENERIC_ERROR_RESPONSE : return "ASYNC_GENERIC_ERROR_RESPONSE";
    default                          : return "*UNKNOWN_STATUS_ERROR*";
  }
}

// Constructor
async_packet::async_packet
( uint8_t  command 
, uint64_t address 
, uint16_t data_len
, uint8_t* data_ptr
, uint64_t delay_ns
, uint64_t time_ns 
, uint16_t strm_wid
)
{
  this->id       = s_id++;
  this->command  = command;
  this->address  = address;
  this->delay_ns = delay_ns;
  this->time_ns  = time_ns;
  this->strm_wid = strm_wid;
  this->status   = ASYNC_INCOMPLETE_RESPONSE;
  this->data_len = data_len;
  //assert( payload_ptr->data_len == 0 || payload_ptr->data_ptr != nullptr );
  this->data_ptr = data_ptr;
}

// Copy constructor
async_packet::async_packet(const async_packet& rhs)
{
  this->id       = rhs.id;
  this->command  = rhs.command;
  this->address  = rhs.address;
  this->delay_ns = rhs.delay_ns;
  this->time_ns  = rhs.time_ns;
  this->strm_wid = rhs.strm_wid;
  this->status   = rhs.status;
  this->data_len = rhs.data_len;
  this->data_ptr = rhs.data_ptr;
}

// Destructor
async_packet::~async_packet(void)
{
}

// Comparison
bool async_packet::operator==(const async_packet& rhs) const
{
  if ( id       != rhs.id       ) return false;
  if ( command  != rhs.command  ) return false;
  if ( status   != rhs.status   ) return false;
  if ( address  != rhs.address  ) return false;
  if ( delay_ns != rhs.delay_ns ) return false;
  if ( time_ns  != rhs.time_ns  ) return false;
  if ( strm_wid != rhs.strm_wid ) return false;
  if ( data_len != rhs.data_len ) return false;
  assert( data_len == 0 || data_ptr != nullptr );
  for (int i=0; i!=data_len; ++i) {
    if (data_ptr[i] != rhs.data_ptr[i]) return false;
  }
  return true;
}

// Assignment
async_packet& async_packet::operator=(const async_packet& rhs)
{
  this->id       = rhs.id;
  this->command  = rhs.command;
  this->address  = rhs.address;
  this->delay_ns = rhs.delay_ns;
  this->time_ns  = rhs.time_ns;
  this->strm_wid = rhs.strm_wid;
  this->status   = rhs.status;
  this->data_len = rhs.data_len;
  this->data_ptr = rhs.data_ptr;
  return *this;
}

int async_packet::pack(char* buffer)
{
  memcpy( &buffer[ ASYNC_ID_INDEX       ], &id      , ASYNC_PACKET_ID_SIZE       );
  memcpy( &buffer[ ASYNC_COMMAND_INDEX  ], &command , ASYNC_PACKET_COMMAND_SIZE  );
  memcpy( &buffer[ ASYNC_STATUS_INDEX   ], &status  , ASYNC_PACKET_STATUS_SIZE   );
  memcpy( &buffer[ ASYNC_ADDRESS_INDEX  ], &address , ASYNC_PACKET_ADDRESS_SIZE  );
  memcpy( &buffer[ ASYNC_DELAY_NS_INDEX ], &delay_ns, ASYNC_PACKET_DELAY_NS_SIZE );
  memcpy( &buffer[ ASYNC_TIME_NS_INDEX  ], &time_ns , ASYNC_PACKET_TIME_NS_SIZE  );
  memcpy( &buffer[ ASYNC_STRM_WID_INDEX ], &strm_wid, ASYNC_PACKET_STRM_WID_SIZE );
  memcpy( &buffer[ ASYNC_DATA_LEN_INDEX ], &data_len, ASYNC_PACKET_DATA_LEN_SIZE );
  assert( data_len == 0 || data_ptr != nullptr );
  if (data_len != 0) {
    memcpy( &buffer[ASYNC_DATA_PTR_INDEX], data_ptr, data_len  );
  }
  return ASYNC_DATA_PTR_INDEX + data_len;
}

int async_packet::unpack(char* buffer)
{
  memcpy( &id       , &buffer[ ASYNC_ID_INDEX       ], ASYNC_PACKET_ID_SIZE        );
  memcpy( &command  , &buffer[ ASYNC_COMMAND_INDEX  ], ASYNC_PACKET_COMMAND_SIZE   );
  memcpy( &status   , &buffer[ ASYNC_STATUS_INDEX   ], ASYNC_PACKET_STATUS_SIZE    );
  memcpy( &address  , &buffer[ ASYNC_ADDRESS_INDEX  ], ASYNC_PACKET_ADDRESS_SIZE   );
  memcpy( &delay_ns , &buffer[ ASYNC_DELAY_NS_INDEX ], ASYNC_PACKET_DELAY_NS_SIZE  );
  memcpy( &time_ns  , &buffer[ ASYNC_TIME_NS_INDEX  ], ASYNC_PACKET_TIME_NS_SIZE   );
  memcpy( &strm_wid , &buffer[ ASYNC_STRM_WID_INDEX ], ASYNC_PACKET_STRM_WID_SIZE  );
  memcpy( &data_len , &buffer[ ASYNC_DATA_LEN_INDEX ], ASYNC_PACKET_DATA_LEN_SIZE  );
  assert( data_len == 0 || data_ptr != nullptr );
  if (data_len != 0) {
    memcpy(   data_ptr, &buffer[ASYNC_DATA_PTR_INDEX], data_len  );
  }
  return ASYNC_DATA_PTR_INDEX + data_len;
}

#include <sstream>
#include <iomanip>
std::string async_packet::str(void) const
{
  using namespace std;
  ostringstream os;
  os << "async_packet{"
     << this->id << ","
     << async_command_to_str(async_command_t(this->command)) << ","
     << async_status_to_str(async_status_t(this->status)) << ","
     << "addr=0x" << hex << this->address << ","
     << "dlen="   << dec << this->data_len
     ;
  if (strm_wid) os << ",strm_wid="   << dec << this->strm_wid;
  assert( this->data_len == 0 || this->data_ptr != nullptr );
  if (this->data_len > 0) os << ",data=";
  for(int i=0;i!=this->data_len;++i) {
    if (i < 16) os << "0x" << hex << int(this->data_ptr[i]) << " ";
    else if (i == this->data_len-1) os << "0x" << hex << int(this->data_ptr[i]);
    else if (i == 16) os << "... ";
  }
  if (time_ns) os << "@" << dec << time_ns << "ns";
  if (delay_ns) os << ":" << dec << delay_ns << "ns";
  os << "}" << ends;
  return os.str();
}

void async_packet::print(const char* message) const
{
  if (message != 0) printf("%s ",message);
  printf
  ( "async_packet{%d,%s,%s,addr=0x%lx,dlen=%d"
  , this->id      
  , async_command_to_str(async_command_t(this->command))
  , async_status_to_str(async_status_t(this->status))
  , this->address 
  , this->data_len
  );
  assert( this->data_len == 0 || this->data_ptr != nullptr );
  if (this->data_len > 0) printf(",data=");
  for(int i=0;i!=this->data_len;++i) {
    if (i < 16) printf("%02x ",(int)this->data_ptr[i]);
    else if (i == this->data_len-1) printf("%02x",(int)this->data_ptr[i]);
    else if (i == 16) printf("... ");
  }
  if (strm_wid) printf("strm_wid=%hu",strm_wid);
  if (time_ns)  printf("@%luns",time_ns);
  if (delay_ns) printf(":%luns",delay_ns);
  printf("}\n");
}

int get_async_data_len(char buffer[])
{
  int data_len;
  memcpy( &data_len, &buffer[ ASYNC_DATA_LEN_INDEX ], ASYNC_PACKET_DATA_LEN_SIZE  );
  return data_len;
}

