#include "async_packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef UNIT_TEST
int main(int argc, char* argv[])
{
  async_packet*   p1 = 0;
  uint8_t        d1[ASYNC_MAX_DATA_LEN];
  async_packet*   p2 = 0;
  uint8_t        d2[ASYNC_MAX_DATA_LEN];
  async_command_t command;
  uint64_t       address;
  uint16_t       data_len;
  char           buffer[ASYNC_MAX_BUFFER];
  int            test_len[] = { 1, 2, 8, 16, 0, ASYNC_MAX_DATA_LEN, ASYNC_MAX_DATA_LEN-1 };
  srandom(1);
  for (int i=0; i!=20; ++i) {
	printf("LALALALALALALALAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALALALALALALALALALALALAL");
    command = async_command_t(random()%(ASYNC_EXIT+1));
    address = random();
    if (random()%4<3 && address & 0x80800000) address |= (random() << 32);
    if (i < sizeof(test_len)) data_len = test_len[i];
    else                      data_len = random()%ASYNC_MAX_DATA_LEN+1;
    bzero(d1,ASYNC_MAX_DATA_LEN);
    for (int i=0; i!= data_len; ++i) {
      d1[i] = random();
    }
    p1 = new async_packet( command, address, data_len, &d1[0] );
    if (i < sizeof(test_len)) p1->status  = async_status_t(random()%(ASYNC_GENERIC_ERROR_RESPONSE+1));
    p1->print("p1");
    bzero(buffer,ASYNC_MAX_BUFFER);
    p1->pack(buffer);
    memset(d2,0xFFu,ASYNC_MAX_DATA_LEN);
    p2 = new async_packet( ASYNC_IGNORE, 0, 0, &d2[0], 500, 42 );
    p2->print("new p2");
    p2->unpack(buffer);
    p2->print("p2 unpacked from p1");
    if (not(*p1 == *p2)) fprintf(stderr,"ERROR: async_packet mismatch!\n");
    delete p1;
    delete p2;
  }
}
#endif
