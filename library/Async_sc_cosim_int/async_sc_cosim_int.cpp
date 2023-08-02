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
#include <string.h>
#include <fcntl.h>  
#include <unistd.h> 
#include <sys/types.h>
#include <fcntl.h>     /*  Defines  O_* constants  */
#include <sys/stat.h>  /*  Defines  mode  constants  */
#ifdef _SHM
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#endif
#ifdef _POSIX
#include <sys/mman.h>
#include <semaphore.h>
#endif
#include <sys/times.h> //measure time
#include <errno.h>
#include <iomanip>
#include <string>
#ifdef _TCP
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#endif

#include "sighandler.h"
#include "async_sc_cosim_int.h"

using namespace std;

using namespace sc_core;
using namespace sc_dt;


async_sc_cosim_int::async_sc_cosim_int( sc_module_name instance_name): sc_module(instance_name), m_master_socket("master_port"), m_async_channel("m_async_channel"), m_keep_alive_signal("m_keep_alive_signal"), 
m_lock_permission(new std::lock_guard<std::mutex>(m_allow_pthread)), m_pthread(&async_sc_cosim_int::async_os_thread,this,std::ref(m_async_channel))
{
  Sighandler::init();
  //----------------------------------------------------------------------------
  // Register processes
  //----------------------------------------------------------------------------
  this->s_stop_requests = 0;
  SC_HAS_PROCESS(async_sc_cosim_int);
  SC_THREAD(asysc_sysc_thread_process);
  SC_THREAD(keep_alive_process);
    sensitive << CLK.pos();
#ifdef _REALTIME
  m_realtimify = new realtimify("realtimify", _RT_TIME, _RT_UNIT);
#endif
  m_pthread.detach(); //releasing the thread -> this fix the segfault error in the end
  cout << name() << ":: "<< __func__ << endl;
}

// Destructor 
async_sc_cosim_int::~async_sc_cosim_int(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

// Callbacks
void async_sc_cosim_int::before_end_of_elaboration(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

void async_sc_cosim_int::end_of_elaboration(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

void async_sc_cosim_int::start_of_simulation(void) {
  cout << name() << ":: "<< __func__ << endl;
  m_keep_alive_signal.write(true);
  m_lock_permission.reset(nullptr);
}

void async_sc_cosim_int::end_of_simulation(void)
{
  cout << name() << ":: "<< __func__ << endl;
}

void async_sc_cosim_int::asysc_sysc_thread_process(void)  {
  cout << name() << ":: "<< __func__ << endl;

  // Holding place for data
  uint8_t data_ptr[ASYNC_MAX_DATA_LEN];
  //TLM Data payload
  tlm::tlm_generic_payload tlm2_trans;
  sc_time delay(SC_ZERO_TIME);  
  while(true) {
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
    cout << name() << ":: "<< __func__ << ":: (" << time(NULL) << " : " << sc_time_stamp() << ") Received in systemC: " << async_trans_ptr->str() << endl;

    // Setup TLM generic payload
    tlm2_trans.set_address(async_trans_ptr->address);
    tlm2_trans.set_data_ptr(async_trans_ptr->data_ptr);
    tlm2_trans.set_data_length(async_trans_ptr->data_len);
    tlm2_trans.set_streaming_width(async_trans_ptr->data_len);
    tlm2_trans.set_byte_enable_ptr(nullptr);
    tlm2_trans.set_dmi_allowed(false);
    tlm2_trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    switch(async_trans_ptr->command) {
      case ASYNC_IGNORE:
        tlm2_trans.set_command(tlm::TLM_IGNORE_COMMAND);
        break;
      case ASYNC_WRITE:
        tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND); 
        break;
      case ASYNC_DEBUG_WRITE:
        tlm2_trans.set_command(tlm::TLM_WRITE_COMMAND);
        break;
      case ASYNC_READ:
        tlm2_trans.set_command(tlm::TLM_READ_COMMAND);
        break;
      case  ASYNC_DEBUG_READ:
        tlm2_trans.set_command(tlm::TLM_READ_COMMAND);
        break;
      case  ASYNC_EXIT:
        SC_REPORT_WARNING("ASYNC_TLM_SC_INT", "Received ASYNC_EXIT - ignored as TLM Command");
        break;
      default:
        SC_REPORT_WARNING("ASYNC_TLM_SC_INT", "Unknown TLM command - ignored");
        break;
    }
    
    // Exit if commanded
    if (async_trans_ptr->command == ASYNC_EXIT) {
      cout << name() << ":: "<< __func__ << ":: Exiting due to ASYNC_EXIT..." << endl;
      break;
    }
    // Initiate appropriate transport
    int transferred = 0;
    switch(async_trans_ptr->command) {
      case ASYNC_DEBUG_READ:
      case ASYNC_DEBUG_WRITE:
        transferred = m_master_socket->transport_dbg(tlm2_trans);
        if (transferred != async_trans_ptr->data_len) 
          SC_REPORT_WARNING("ASYNC_TLM_SC_INT", "transport_dbg returned ");
        break;
      default :
        delay = SC_ZERO_TIME;
        m_master_socket->b_transport(tlm2_trans,delay);
        wait(delay);
        break;
    }

    // Update async_packet
    switch (tlm2_trans.get_response_status()) {
      case tlm::TLM_OK_RESPONSE:
        async_trans_ptr->status = ASYNC_OK_RESPONSE;
        break;
      case tlm::TLM_ADDRESS_ERROR_RESPONSE:
        async_trans_ptr->status = ASYNC_ADDRESS_ERROR_RESPONSE;
        break;
      case tlm::TLM_INCOMPLETE_RESPONSE:
        async_trans_ptr->status = ASYNC_INCOMPLETE_RESPONSE;
        break;
      default:
        async_trans_ptr->status = ASYNC_GENERIC_ERROR_RESPONSE;
        break;
    }

    // Lockdown and place in outgoing queue
    m_async_channel.nb_put(async_trans_ptr);
    cout << name() << ":: "<< __func__ << ":: (" << time(NULL) << " : " << sc_time_stamp() << ") Packet was put in Channel by systemC: " << async_trans_ptr->str() << endl;

  }
  wait(2,SC_SEC);
  cout << name() << ":: "<< __func__ << ":: Exiting asysc_sysc_thread_process..." << endl;
  sc_stop();
}

void async_sc_cosim_int::keep_alive_process(void)  {
  cout << name() << ":: "<< __func__ << endl;
  while(true) {
    wait();
    if ( Sighandler::stop_requests() > 0 ) break;  // by apapa 
    if (!m_keep_alive_signal.read()) {
      wait(m_keep_alive_signal.default_event());
    }
  }
  wait(1,SC_SEC);
  cout << name() << ":: "<< __func__ << ":: Exiting due to stop request ..." << endl;
  sc_stop();
  wait(1,SC_SEC);
}


#ifdef _POSIX
// External OS thread based on Posix share memory
void async_sc_cosim_int::async_os_thread(async_channel& channel)
{
  sem_t *sharedsemA, * sharedsemB;
  const char *semnameA = "sharedsemA";
  const char *semnameB = "sharedsemB";
  const char *shmem_name = "sc_cosim.posix.key";
  int shmem_fd;
  int oflags=O_RDWR | O_CREAT | O_TRUNC; //for server
  //int oflags=O_RDWR; //for client
  off_t shmem_length = sizeof(struct simple_async_packet);
  
  //timings structs
  struct tms tb;
  double t1;

  cout << name() << ":: "<< __func__ << ":: Initializing Posix Share Memory Interface..." << endl;
  shmem_fd = shm_open(shmem_name, oflags, 0644 );
  if (shmem_fd == -1){
    cout << "Unable to get Open sc_cosim.posix.key for Share - Memory: " << strerror(errno) << endl;
    sc_stop();
  }
  int ftrunc_res = ftruncate(shmem_fd, shmem_length); //init size of shmem
  if (ftrunc_res == -1){
      cout << "Unable to set size for Share - Memory: " << strerror(errno) << endl;
      sc_stop();
  }else{
	  fprintf(stderr,"Posix Shared Mem Descriptor: fd=%d\n", shmem_fd);	  
  }
  m_shptr = (struct simple_async_packet *) mmap(NULL, shmem_length, PROT_READ|PROT_WRITE, MAP_SHARED, shmem_fd, 0);
  if ((void *)m_shptr == MAP_FAILED){
      cout << "Unable to get virual mem descriptor: " << strerror(errno) << endl;
      sc_stop();
  }
  
  cout << name() << ":: "<< __func__ << ":: Initializing Semaphores..." << endl;
  sharedsemA = sem_open(semnameA, O_CREAT | O_EXCL, 0666, 0); //real, initial value is 0
  if (sharedsemA == SEM_FAILED){
      if (errno == EEXIST){
        sharedsemA = sem_open(semnameA, O_EXCL, 0666);
        if (sharedsemA == SEM_FAILED){
            cout << "Unable to bind to SEMA: " << strerror(errno) << endl;
            sc_stop();
        }
        int sem_value = 0;
        int sem_get_val = sem_getvalue(sharedsemA, &sem_value);
        if (sem_get_val == 0){
            cout << "value for SEMA: " << sem_value << endl;
            while (sem_value > 0){
                sem_wait(sharedsemA);
                sem_getvalue(sharedsemA, &sem_value);
                cout << "value for SEMA: " << sem_value << endl;
            }
        }else{
            cout << "Unable to get value for SEMA: " << strerror(errno) << endl;
            sc_stop();
        }
      }else{
	cout << "Unable to open SEMA: " << strerror(errno) << endl;
        sc_stop();
      }
  }
  sharedsemB = sem_open(semnameB, O_CREAT | O_EXCL, 0666, 0); //virtual , initial value is 0
  if (sharedsemB == SEM_FAILED){
      if (errno == EEXIST){
        sharedsemB = sem_open(semnameB, O_EXCL, 0666);
        if (sharedsemB == SEM_FAILED){
            cout << "Unable to bind to SEMB: " << strerror(errno) << endl;
            sc_stop();
        }
        int sem_value = 0;
        int sem_get_val = sem_getvalue(sharedsemB, &sem_value);
        if (sem_get_val == 0){
            cout << "value for SEMB: " << sem_value << endl;
            while (sem_value > 0){
                sem_wait(sharedsemB);
                sem_getvalue(sharedsemB, &sem_value);
                cout << "value for SEMB: " << sem_value << endl;
            }
        }else{
            cout << "Unable to get value for SEMB: " << strerror(errno) << endl;
            sc_stop();
        }
      }else{
	cout << "Unable to open SEMB: " << strerror(errno) << endl;
        sc_stop();
      }
  }
  cout << "SEMA: " << sharedsemA << " | SEMB: " << sharedsemB << endl;
  cout << name() << ":: "<< __func__ << ":: Begining Event loop..." << endl;
  while (true) {
	cout << name() << ":: "<< __func__ << ":: Waiting for new Data..." << endl;
	sem_wait(sharedsemA);
    clock_gettime(CLOCK_REALTIME, &(m_shptr->channel_in));
	cout << name() << ":: "<< __func__ << ":: New data received: ID: " << m_shptr->id << " Command: " << async_command_to_str((async_command_t) m_shptr->command) 
	     << " Address: " <<  hex << m_shptr->address << dec << " Data: " <<  hex << m_shptr->data << dec << endl;
    t1=(double)times(&tb);
    m_shptr->systemc_start_time = t1;

    //load the data to the packet
    uint8_t payload[4];
    memcpy(payload, &m_shptr->data, sizeof(uint32_t));
    /*
    payload[0] = (m_shptr->data >> 24) & 0xFF;
	payload[1] = (m_shptr->data >> 16) & 0xFF;
	payload[2] = (m_shptr->data >> 8) & 0xFF;
	payload[3] = m_shptr->data & 0xFF;
	*/
    async_packet_ptr async_trans_ptr(new async_packet( (async_command_t)m_shptr->command, (uint64_t)(m_shptr->address), 4, payload ));

    cout << name() << ":: "<< __func__ << ":: Sending @ " << time(NULL) << " to systemC: " << async_trans_ptr->str() << endl;

    // Send request to SystemC
    cout << name() << ":: "<< __func__ << ":: Pushing to async_channel..." << endl;
    clock_gettime(CLOCK_REALTIME, &(m_shptr->sc_in));
    channel.push(async_trans_ptr);

    // Wait for channel to pass payload to initiator_sysc_thread_process & return results
    cout << name() << ":: "<< __func__ << ":: Waiting for async_channel ..." << endl;
    channel.wait_for_put();
    clock_gettime(CLOCK_REALTIME, &(m_shptr->sc_out));

    // Pull response from SystemC
    cout << name() << ":: "<< __func__ << ":: Pulling from async_channel ..." << endl;
    if (not channel.nb_pull(async_trans_ptr)) {
      cout << "[CRITICAL ERROR] " << name() << ":: "<< __func__ << ":: Missing response" << endl;
	  m_shptr->status = ASYNC_INCOMPLETE_RESPONSE;
	  sem_post(sharedsemB); 
	  continue;
    }
    cout << name() << ":: "<< __func__ << ":: Response from SystemC received: " << time(NULL) << " : " << async_trans_ptr->str() << endl;
    
    // Seting responce status
    m_shptr->status = async_trans_ptr->status;
    //Check for errors and adjust
    if (async_trans_ptr->status != ASYNC_OK_RESPONSE) {
      cout << "[ERROR] " << name() << ":: "<< __func__ << ":: Expecting ASYNC_OK_RESPONSE but obtained " << async_status_to_str(async_status_t(async_trans_ptr->status)) << endl;
    }else{
	  memcpy((&m_shptr->data), async_trans_ptr->data_ptr, sizeof(uint32_t));
      cout << name() << ":: "<< __func__ << ":: Got Data:" << hex << m_shptr->data << dec << ". Sending Ok Responce to Client" << endl;
    }

    t1=(double)times(&tb);
    m_shptr->systemc_end_time = t1;
    
    // Inform that procces has been complited
    clock_gettime(CLOCK_REALTIME, &(m_shptr->channel_out));
	sem_post(sharedsemB); 

    // Exit if commanded
    if (async_trans_ptr->command == ASYNC_EXIT) {
      cout << name() << ":: "<< __func__ << ":: Exiting due to ASYNC_EXIT..." << endl;
	  // Inform that procces has been complited
      break;
    }
  }
  cout << name() << ":: "<< __func__ << ":: Cleaning Share Memory and semaphores..." << endl;
  munmap((void *)m_shptr , shmem_length);
  close(shmem_fd);
  sem_close(sharedsemA);
  sem_close(sharedsemB);
  cout << name() << ":: "<< __func__ << ":: Closing down ..." << endl;
  return;
}
#endif

#ifdef _SHM
// External OS thread
void async_sc_cosim_int::async_os_thread(async_channel& channel)
{
  int key;
  sem_t *sharedsemA, * sharedsemB;
  const char *semnameA = "sharedsemA";
  const char *semnameB = "sharedsemB";
  int shmid;

  //timings structs
  struct tms tb;
  double t1;

  cout << name() << ":: "<< __func__ << ":: Initializing Share Memory Interface..." << endl;
  if ((key = ftok("/tmp/sc_cosim.shm.key", 'R')) == -1) /*Here the file must exist */ 
  { 
    cout << "Key File /tmp/sc_cosim.shm.key does not exists (" << strerror(errno) << "). Please if this is the first time touch the file" << endl;
    sc_stop();
  }

  shmid = shmget(key, sizeof(struct simple_async_packet), 0666 | IPC_CREAT );
  if(shmid < 0){
      cout << "Unable to get value for Share - Memory: " << strerror(errno) << endl;
      sc_stop();
  }else{
    printf("smid is %d\n", shmid); 
  }
  m_shptr=(struct simple_async_packet *)shmat(shmid,0,0);

  cout << name() << ":: "<< __func__ << ":: Initializing Semaphores..." << endl;
  sharedsemA = sem_open(semnameA, O_CREAT | O_EXCL, 0666, 0); //real, initial value is 0
  if (sharedsemA == SEM_FAILED){
      if (errno == EEXIST){
        sharedsemA = sem_open(semnameA, O_EXCL, 0666);
        if (sharedsemA == SEM_FAILED){
            cout << "Unable to bind to SEMA: " << strerror(errno) << endl;
            sc_stop();
        }
        int sem_value = 0;
        int sem_get_val = sem_getvalue(sharedsemA, &sem_value);
        if (sem_get_val == 0){
            cout << "value for SEMA: " << sem_value << endl;
            while (sem_value > 0){
                sem_wait(sharedsemA);
                sem_getvalue(sharedsemA, &sem_value);
                cout << "value for SEMA: " << sem_value << endl;
            }
        }else{
            cout << "Unable to get value for SEMA: " << strerror(errno) << endl;
            sc_stop();
        }
      }else{
	cout << "Unable to open SEMA: " << strerror(errno) << endl;
        sc_stop();
      }
  }
  sharedsemB = sem_open(semnameB, O_CREAT | O_EXCL, 0666, 0); //virtual , initial value is 0
  if (sharedsemB == SEM_FAILED){
      if (errno == EEXIST){
        sharedsemB = sem_open(semnameB, O_EXCL, 0666);
        if (sharedsemB == SEM_FAILED){
            cout << "Unable to bind to SEMB: " << strerror(errno) << endl;
            sc_stop();
        }
        int sem_value = 0;
        int sem_get_val = sem_getvalue(sharedsemB, &sem_value);
        if (sem_get_val == 0){
            cout << "value for SEMB: " << sem_value << endl;
            while (sem_value > 0){
                sem_wait(sharedsemB);
                sem_getvalue(sharedsemB, &sem_value);
                cout << "value for SEMB: " << sem_value << endl;
            }
        }else{
            cout << "Unable to get value for SEMB: " << strerror(errno) << endl;
            sc_stop();
        }
      }else{
	cout << "Unable to open SEMB: " << strerror(errno) << endl;
        sc_stop();
      }
  }
  cout << "SEMA: " << sharedsemA << " | SEMB: " << sharedsemB << endl;
  cout << name() << ":: "<< __func__ << ":: Begining Event loop..." << endl;
  while (true) {
	cout << name() << ":: "<< __func__ << ":: Waiting for new Data..." << endl;
	sem_wait(sharedsemA);
    clock_gettime(CLOCK_REALTIME, &(m_shptr->channel_in));
	cout << name() << ":: "<< __func__ << ":: New data received: ID: " << m_shptr->id << " Command: " << async_command_to_str((async_command_t) m_shptr->command) 
	     << " Address: " <<  hex << m_shptr->address << dec << " Data: " <<  hex << m_shptr->data << dec << endl;
    t1=(double)times(&tb);
    m_shptr->systemc_start_time = t1;

    //load the data to the packet
    uint8_t payload[4];
    memcpy(payload, &m_shptr->data, sizeof(uint32_t));
    /*
    payload[0] = (m_shptr->data >> 24) & 0xFF;
	payload[1] = (m_shptr->data >> 16) & 0xFF;
	payload[2] = (m_shptr->data >> 8) & 0xFF;
	payload[3] = m_shptr->data & 0xFF;
	*/
    async_packet_ptr async_trans_ptr(new async_packet( (async_command_t)m_shptr->command, (uint64_t)(m_shptr->address), 4, payload ));

    cout << name() << ":: "<< __func__ << ":: Sending @ " << time(NULL) << " to systemC: " << async_trans_ptr->str() << endl;

    // Send request to SystemC
    cout << name() << ":: "<< __func__ << ":: Pushing to async_channel..." << endl;
    clock_gettime(CLOCK_REALTIME, &(m_shptr->sc_in));
    channel.push(async_trans_ptr);

    // Wait for channel to pass payload to initiator_sysc_thread_process & return results
    cout << name() << ":: "<< __func__ << ":: Waiting for async_channel ..." << endl;
    channel.wait_for_put();
    clock_gettime(CLOCK_REALTIME, &(m_shptr->sc_out));

    // Pull response from SystemC
    cout << name() << ":: "<< __func__ << ":: Pulling from async_channel ..." << endl;
    if (not channel.nb_pull(async_trans_ptr)) {
      cout << "[CRITICAL ERROR] " << name() << ":: "<< __func__ << ":: Missing response" << endl;
	  m_shptr->status = ASYNC_INCOMPLETE_RESPONSE;
	  sem_post(sharedsemB); 
	  continue;
    }
    cout << name() << ":: "<< __func__ << ":: Response from SystemC received: " << time(NULL) << " : " << async_trans_ptr->str() << endl;
    
    // Seting responce status
    m_shptr->status = async_trans_ptr->status;
    //Check for errors and adjust
    if (async_trans_ptr->status != ASYNC_OK_RESPONSE) {
      cout << "[ERROR] " << name() << ":: "<< __func__ << ":: Expecting ASYNC_OK_RESPONSE but obtained " << async_status_to_str(async_status_t(async_trans_ptr->status)) << endl;
    }else{
	  memcpy((&m_shptr->data), async_trans_ptr->data_ptr, sizeof(uint32_t));
      cout << name() << ":: "<< __func__ << ":: Got Data:" << hex << m_shptr->data << dec << ". Sending Ok Responce to Client" << endl;
    }

    t1=(double)times(&tb);
    m_shptr->systemc_end_time = t1;
    
    // Inform that procces has been complited
    clock_gettime(CLOCK_REALTIME, &(m_shptr->channel_out));
	sem_post(sharedsemB); 

    // Exit if commanded
    if (async_trans_ptr->command == ASYNC_EXIT) {
      cout << name() << ":: "<< __func__ << ":: Exiting due to ASYNC_EXIT..." << endl;
	  // Inform that procces has been complited
      break;
    }
  }
  cout << name() << ":: "<< __func__ << ":: Cleaning Share Memory and semaphores..." << endl;
  shmctl(shmid,IPC_RMID,0); //destroy shared memory
  sem_close(sharedsemA);
  sem_close(sharedsemB);
  cout << name() << ":: "<< __func__ << ":: Closing down ..." << endl;
  return;
}
#endif

#ifdef _TCP
// External OS thread
void async_sc_cosim_int::async_os_thread(async_channel& channel)
{
  //timings structs
  struct tms tb;
  double t1;
  // port to start the server on
  int wait_size = 16;  // maximum number of waiting clients, after which
	                   // dropping begins

  // socket address used for the server
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;

  // htons: host to network short: transforms a value in host byte
  // ordering format to a short value in network byte ordering format
  server_address.sin_port = htons(_SERVER_PORT);
  // htonl: host to network long: same as htons but to long
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);

  // create a TCP socket, creation returns -1 on failure
  int listen_sock;
  if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	printf("ERROR:: could not create listen socket\n");
	sc_stop();
	return;
  }

  // bind it to listen to the incoming connections on the created server address
  if ((bind(listen_sock, (struct sockaddr *)&server_address, sizeof(server_address))) < 0) {
	printf("ERROR:: could not bind socket\n");
	sc_stop();
	return;
  }
  if (listen(listen_sock, wait_size) < 0) {
	printf("ERROR:: could not open socket for listening\n");
	sc_stop();
	return;
  }
  
  cout << name() << ":: "<< __func__ << ":: Begining Event loop..." << endl;
  while (true) {
	int sock;
	int n = 0;
	int len = 0, maxlen = 512;
	// socket address used to store client address
	struct sockaddr_in client_address;
	socklen_t client_address_len = 0;

	char buffer[maxlen];
	struct simple_async_packet m_simple_packet;
	bzero(buffer, maxlen);
	struct simple_async_packet *m_shptr;
	char *pbuffer = buffer;
	m_shptr = &m_simple_packet;
	
	cout << name() << ":: "<< __func__ << ":: Waiting for new Data..." << endl;
	if ((sock = accept(listen_sock, (struct sockaddr *)&client_address, &client_address_len)) < 0) {
		printf("ERROR:: could not open a socket to accept data\n");
		sc_stop();
		return;
	}
	printf("client connected with ip address: %s\n", inet_ntoa(client_address.sin_addr));

	// Receive the data and keep the client connection open
	while ((n = recv(sock, pbuffer, maxlen, 0)) > 0) {
		pbuffer += n;
		maxlen -= n;
		len += n;
		printf("received: %d bytes, total as far %d\n", n, len);
		if (len == sizeof(simple_async_packet))
			break;
	}
	memcpy(m_shptr, buffer, len);
	clock_gettime(CLOCK_REALTIME, &(m_shptr->channel_in));
	cout << name() << ":: "<< __func__ << ":: New data received: ID: " << m_shptr->id << " Command: " << async_command_to_str((async_command_t) m_shptr->command) 
		 << " Address: " <<  hex << m_shptr->address << dec << " Data: " <<  hex << m_shptr->data << dec << endl;
	t1=(double)times(&tb);
	m_shptr->systemc_start_time = t1;
	//load the data to the packet
	uint8_t payload[4];
	memcpy(payload, &m_shptr->data, sizeof(uint32_t));
    /*
    payload[0] = (m_shptr->data >> 24) & 0xFF;
	payload[1] = (m_shptr->data >> 16) & 0xFF;
	payload[2] = (m_shptr->data >> 8) & 0xFF;
	payload[3] = m_shptr->data & 0xFF;
	*/
    async_packet_ptr async_trans_ptr(new async_packet( (async_command_t)m_shptr->command, (uint64_t)(m_shptr->address), 4, payload ));
    cout << name() << ":: "<< __func__ << ":: Sending @ " << time(NULL) << " to systemC: " << async_trans_ptr->str() << endl;

    // Send request to SystemC
    cout << name() << ":: "<< __func__ << ":: Pushing to async_channel..." << endl;
    clock_gettime(CLOCK_REALTIME, &(m_shptr->sc_in));
    channel.push(async_trans_ptr);

    // Wait for channel to pass payload to initiator_sysc_thread_process & return results
    cout << name() << ":: "<< __func__ << ":: Waiting for async_channel ..." << endl;
    channel.wait_for_put();
    clock_gettime(CLOCK_REALTIME, &(m_shptr->sc_out));

    // Pull response from SystemC
    cout << name() << ":: "<< __func__ << ":: Pulling from async_channel ..." << endl;
    if (not channel.nb_pull(async_trans_ptr)) {
      cout << "[CRITICAL ERROR] " << name() << ":: "<< __func__ << ":: Missing response" << endl;
	  m_shptr->status = ASYNC_INCOMPLETE_RESPONSE;
	  memcpy(buffer, m_shptr, len);
	  send(sock, buffer, len, 0);
	  continue;
    }
    cout << name() << ":: "<< __func__ << ":: Response from SystemC received: " << time(NULL) << " : " << async_trans_ptr->str() << endl;
    
    // Seting responce status
    m_shptr->status = async_trans_ptr->status;
    //Check for errors and adjust
    if (async_trans_ptr->status != ASYNC_OK_RESPONSE) {
      cout << "[ERROR] " << name() << ":: "<< __func__ << ":: Expecting ASYNC_OK_RESPONSE but obtained " << async_status_to_str(async_status_t(async_trans_ptr->status)) << endl;
    }else{
	  memcpy((&m_shptr->data), async_trans_ptr->data_ptr, sizeof(uint32_t));
      cout << name() << ":: "<< __func__ << ":: Got Data:" << hex << m_shptr->data << dec << ". Sending Ok Responce to Client" << endl;
    }

    t1=(double)times(&tb);
    m_shptr->systemc_end_time = t1;
    
    // Inform that procces has been complited
    clock_gettime(CLOCK_REALTIME, &(m_shptr->channel_out));
    memcpy(buffer, m_shptr, len);
	send(sock, buffer, len, 0);
	
    // Exit if commanded
    if (async_trans_ptr->command == ASYNC_EXIT) {
      cout << name() << ":: "<< __func__ << ":: Exiting due to ASYNC_EXIT..." << endl;
	  // Inform that procces has been complited
      break;
    }
	cout << name() << ":: "<< __func__ << ":: Closing Socket..." << endl;
	close(sock);
  }
  cout << name() << ":: "<< __func__ << ":: Cleaning Memory..." << endl;
  close(listen_sock);
  cout << name() << ":: "<< __func__ << ":: Closing down ..." << endl;
  return;
}
#endif
