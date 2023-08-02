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
#ifndef ASYNC_PACKET_H
#define ASYNC_PACKET_H

#include <stdint.h>

typedef enum 
{ ASYNC_IGNORE
, ASYNC_READ
, ASYNC_WRITE
, ASYNC_DEBUG_READ
, ASYNC_DEBUG_WRITE
, ASYNC_MUTEX_LOCK
, ASYNC_MUTEX_TRYLOCK //< (data[0] == 0) implies failure
, ASYNC_MUTEX_UNLOCK  
, ASYNC_INTERRUPT
, ASYNC_EXIT /*< must be last - this command is NOT TLM 2.0, but used to close the connection */
} async_command_t;
const char* async_command_to_str(async_command_t s);

typedef enum
{ ASYNC_INCOMPLETE_RESPONSE
, ASYNC_OK_RESPONSE
, ASYNC_ADDRESS_ERROR_RESPONSE
, ASYNC_GENERIC_ERROR_RESPONSE /*< must be last */
} async_status_t;

const char* async_status_to_str(async_status_t s);

// REMINDER: If you modify fields in the following, you must also
// carefully modify the _SIZE & _INDEX constants (macros). Also update
// the pack/unpack routines in respective .c and .cpp implementations.
#ifndef __cplusplus
typedef struct {
#else
#include <string>
struct async_packet
{
  async_packet //< Constructor
  ( uint8_t  command 
  , uint64_t address 
  , uint16_t data_len
  , uint8_t* data_ptr
  , uint64_t delay_ns = 0UL
  , uint64_t time_ns = 0UL
  , uint16_t strm_wid = 0
  );
  async_packet(const async_packet& rhs); // Copy constructor
  ~async_packet(void); //< Destructor
  bool operator==(const async_packet& rhs) const;
  async_packet& operator=(const async_packet& rhs);
  int  pack(char* buffer);
  int  unpack(char* buffer);
  std::string str(void) const;
  void print(const char* message) const;
#endif
  int32_t  id;      
  uint8_t  command; 
  uint8_t  status;  
  uint64_t address; 
  uint64_t delay_ns; 
  uint64_t time_ns;  
  uint16_t strm_wid;
  uint16_t data_len;
  uint8_t* data_ptr;
#ifndef __cplusplus
} async_packet;
#else
private:
  static int32_t s_id;
};

#include <memory>
typedef std::shared_ptr<async_packet> async_packet_ptr;
#endif

#define ASYNC_PACKET_ID_SIZE       4
#define ASYNC_PACKET_COMMAND_SIZE  1
#define ASYNC_PACKET_STATUS_SIZE   1
#define ASYNC_PACKET_ADDRESS_SIZE  8
#define ASYNC_PACKET_DELAY_NS_SIZE 8
#define ASYNC_PACKET_TIME_NS_SIZE  8
#define ASYNC_PACKET_STRM_WID_SIZE 8
#define ASYNC_PACKET_DATA_LEN_SIZE 2

#define ASYNC_ID_INDEX       0
#define ASYNC_COMMAND_INDEX  (ASYNC_ID_INDEX       + ASYNC_PACKET_ID_SIZE)      
#define ASYNC_STATUS_INDEX   (ASYNC_COMMAND_INDEX  + ASYNC_PACKET_COMMAND_SIZE) 
#define ASYNC_ADDRESS_INDEX  (ASYNC_STATUS_INDEX   + ASYNC_PACKET_STATUS_SIZE)  
#define ASYNC_DELAY_NS_INDEX (ASYNC_ADDRESS_INDEX  + ASYNC_PACKET_ADDRESS_SIZE)
#define ASYNC_TIME_NS_INDEX  (ASYNC_DELAY_NS_INDEX + ASYNC_PACKET_DELAY_NS_SIZE)
#define ASYNC_DATA_LEN_INDEX (ASYNC_TIME_NS_INDEX  + ASYNC_PACKET_TIME_NS_SIZE)
#define ASYNC_STRM_WID_INDEX (ASYNC_DATA_LEN_INDEX + ASYNC_PACKET_TIME_NS_SIZE)
#define ASYNC_DATA_PTR_INDEX (ASYNC_STRM_WID_INDEX + ASYNC_PACKET_DATA_LEN_SIZE)
#define ASYNC_MAX_DATA_LEN   ((1<<(8*ASYNC_PACKET_DATA_LEN_SIZE))-1)
#define ASYNC_MAX_BUFFER     (ASYNC_DATA_PTR_INDEX + ASYNC_MAX_DATA_LEN)

#ifndef __cplusplus
async_packet* new_async_packet
( uint8_t  command 
, uint64_t address 
, uint16_t data_len
, uint8_t* data_ptr
, uint64_t delay_ns 
, uint64_t time_ns 
, uint64_t strm_wid 
);
void init_async_packet(async_packet* payload_ptr);
async_packet* clone_async_packet(async_packet* rhs);
void delete_async_packet(async_packet* payload_ptr);
int  compare_async_packet(async_packet* p1, async_packet* p2);
int  pack_async(char buffer[], async_packet* payload);
int  unpack_async(async_packet* payload_ptr, char buffer[]);
void print_async(async_packet* payload_ptr, const char* message);
#endif
int  get_async_data_len(char buffer[]);

#endif /*ASYNC_PACKET_H*/
