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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <inttypes.h>
#include <unistd.h>
#include <sys/times.h> //measure time
#include <errno.h>
#include <string.h>

#ifdef _TCP
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#endif

#include "simple_async_packet.h"

struct simple_async_packet *shptr;

#ifdef _POSIX
int main (int argc, char *argv[])
{
    sem_t *sharedsemA, * sharedsemB;
    const char *semnameA = "sharedsemA";
    const char *semnameB = "sharedsemB";
    const char *shmem_name = "sc_cosim.posix.key";
    int shmem_fd;
    //int oflags=O_RDWR | O_CREAT | O_TRUNC; //for server
    int oflags=O_RDWR; //for client
    off_t shmem_length = sizeof(struct simple_async_packet);

	int repeats = 0;;

    printf("Initializing Posix Share Memory Interface...\n");
    shmem_fd = shm_open(shmem_name, oflags, 0644 );
    if (shmem_fd == -1){
      fprintf(stderr,"Unable to get Open sc_cosim.posix.key for Share - Memory:%s\n", strerror(errno));	  
	  exit(-1);
    }
    
    struct stat sb;
	int fstat_res = fstat(shmem_fd, &sb);
    if (fstat_res == -1){
	  fprintf(stderr,"Unable to set size for Share - Memory:%s\n", strerror(errno));	  
      exit(-1);
    }	
	if (shmem_length >  sb.st_size){
	  fprintf(stderr,"Size of Share Memory missmach %ld > %ld \n", shmem_length, sb.st_size);	  
      exit(-1);
    }else{
	  fprintf(stderr,"Posix Shared Mem Descriptor: fd=%d\n", shmem_fd);	  
    }
    shptr = (struct simple_async_packet *) mmap(NULL, shmem_length, PROT_READ|PROT_WRITE, MAP_SHARED, shmem_fd, 0);
    if ((void *)shptr == MAP_FAILED){
	  fprintf(stderr,"Unable to get virual mem descriptor: %s \n", strerror(errno));	  
      exit(-1);
    }

    sharedsemA = sem_open(semnameA, O_EXCL, O_RDWR);
    if (sharedsemA == SEM_FAILED){
		printf("Unable to open SEMA %s\n", strerror(errno)); 
		exit(-1);
	}
    sharedsemB = sem_open(semnameB, O_EXCL, O_RDWR);
	if (sharedsemB == SEM_FAILED){
		printf("Unable to open SEMB %s\n", strerror(errno)); 
		exit(-1);
	}
//	printf("SEMA: %u | SEMB: %u\n", (unsigned int)sharedsemA, (unsigned int)sharedsemB); 

	
    while(repeats < 1){
		shptr->id = repeats + 1;
		shptr->command = 2;
		shptr->status =  0;
//		shptr->address = 0x40000000 + (repeats + 1) * 8;
		shptr->address = (repeats + 1) * 8;
		shptr->data  = (repeats + 1) * 2;

		printf(" Testbench send data: %u %u %u 0x%x %u\n", shptr->id, shptr->command, shptr->status, shptr->address, shptr->data);
	
		/*sent data to systemc */
		sem_post(sharedsemA);
	
		printf(" Waiting for responce\n");

		/*wait for systemC to finish*/
		sem_wait(sharedsemB);

		printf(" Testbench reseived data: %u %u\n", shptr->status, shptr->data);
		
		repeats++;
    }
    
	shptr->id = repeats + 1;
	shptr->command = 9;
	shptr->status =  0;
	shptr->address = 0x0;
	shptr->data = 0;
	printf(" Testbench send data: %u %u %u 0x%x %u\n", shptr->id, shptr->command, shptr->status, shptr->address, shptr->data);
	/*sent data to systemc */
	sem_post(sharedsemA);
	/*wait for systemC to finish*/
//	printf(" Waiting for responce\n");
//	sem_wait(sharedsemB);
//	printf(" Testbench reseived data: %u %u\n", shptr->status, shptr->data);
	munmap((void *)shptr , shmem_length);
	close(shmem_fd);    
	return(0);
}
#endif

#ifdef _SHM
int main (int argc, char *argv[])
{
    key_t key;
    sem_t *sharedsemA, * sharedsemB;
    const char *semnameA = "sharedsemA";
    const char *semnameB = "sharedsemB";
    int shmid;

	int repeats = 0;;

    key = ftok("/tmp/sc_cosim.shm.key", 120);
    shmid = shmget(key, sizeof(struct simple_async_packet), IPC_CREAT | 0666);
    if(shmid < 0)
        printf("shmid now created");
    else
        printf("smid is %d", shmid); 

    shptr=(struct simple_async_packet *)shmat(shmid,0,0);

    sharedsemA = sem_open(semnameA, O_EXCL, O_RDWR);
    if (sharedsemA == SEM_FAILED){
		printf("Unable to open SEMA %s\n", strerror(errno)); 
		exit(-1);
	}
    sharedsemB = sem_open(semnameB, O_EXCL, O_RDWR);
	if (sharedsemB == SEM_FAILED){
		printf("Unable to open SEMB %s\n", strerror(errno)); 
		exit(-1);
	}
	//printf("SEMA: %u | SEMB: %u\n", (unsigned int)sharedsemA, (unsigned int)sharedsemB); 

	
    while(repeats < 1){
		shptr->id = repeats + 1;
		shptr->command = 2;
		shptr->status =  0;
//		shptr->address = 0x40000000 + (repeats + 1) * 8;
		shptr->address = (repeats + 1) * 8;
		shptr->data  = (repeats + 1) * 2;

		printf(" VirtualServer send data: %u %u %u 0x%x %u\n", shptr->id, shptr->command, shptr->status, shptr->address, shptr->data);
	
		/*sent data to systemc */
		sem_post(sharedsemA);
	
		printf(" Waiting for responce\n");

		/*wait for systemC to finish*/
		sem_wait(sharedsemB);

		printf(" VirtualServer reseived data: %u %u\n", shptr->status, shptr->data);
		
		repeats++;
    }
    
	shptr->id = repeats + 1;
	shptr->command = 9;
	shptr->status =  0;
	shptr->address = 0x0;
	shptr->data = 0;
	printf(" VirtualServer send data: %u %u %u 0x%x %u\n", shptr->id, shptr->command, shptr->status, shptr->address, shptr->data);
	/*sent data to systemc */
	sem_post(sharedsemA);
	/*wait for systemC to finish*/
	printf(" Waiting for responce\n");
	sem_wait(sharedsemB);
	printf(" VirtualServer reseived data: %u %u\n", shptr->status, shptr->data);
    
	return(0);
}
#endif

#ifdef _TCP
void init_connection(struct sockaddr_in *server_address){
	const char* server_name = "localhost";
	const int server_port = 1999;
	memset(server_address, 0, sizeof(struct sockaddr_in));
	server_address->sin_family = AF_INET;
	// creates binary representation of server name
	inet_pton(AF_INET, server_name, &(server_address->sin_addr));
	// htons: port in network order format
	server_address->sin_port = htons(server_port);
	return;
};

int send_data(struct sockaddr_in *server_address, struct simple_async_packet *shptr){
	int n = 0;
	int len = 0, maxlen = 512;
	char buffer[maxlen];
	char* pbuffer = buffer;
	bzero(buffer, maxlen);
	// open a stream socket
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("could not create socket\n");
		return (-1);
	}
	// TCP is connection oriented, a reliable connection
	// **must** be established before any data is exchanged
	if (connect(sock, (struct sockaddr*) server_address, sizeof(struct sockaddr_in)) < 0) {
		printf("could not connect to server\n");
		return (-1);
	}
	// data that will be sent to the server
	memcpy(buffer, shptr, sizeof(struct simple_async_packet));
	
	send(sock, buffer, sizeof(struct simple_async_packet), 0);

	bzero(buffer, maxlen);
	// will remain open until the server terminates the connection
	while ((n = recv(sock, pbuffer, maxlen, 0)) > 0) {
		pbuffer += n;
		maxlen -= n;
		len += n;
		printf("received: %d bytes, total as far %d\n", n, len);
		if (len == sizeof(struct simple_async_packet))
			break;
	}
	memcpy(shptr, buffer, len);
	printf("New data received: ID: %d Command: %d  Address: %x  Data: %x\n", shptr->id, shptr->command, shptr->address, shptr->data);
	// close the socket
	close(sock);		
	return(0);
};

int main (int argc, char *argv[])
{
	int repeats = 0;;
	struct simple_async_packet m_shptr;
	struct sockaddr_in server_address;
	init_connection(&server_address);
	
    while(repeats < 2){

		m_shptr.id = repeats + 1;
		m_shptr.command = 2;
		m_shptr.status =  0;
		m_shptr.address = 0x40000000 + (repeats + 1) * 8;
		m_shptr.address = (repeats + 1) * 8;
		m_shptr.data  = (repeats + 1) * 2;
		printf(" VirtualServer sending data: %u %u %u 0x%x %u\n", m_shptr.id, m_shptr.command, m_shptr.status, m_shptr.address, m_shptr.data);
		if (send_data(&server_address, &m_shptr) <0)
			exit(-1);
		printf(" VirtualServer reseived data: %u %u\n", m_shptr.status, m_shptr.data);
		repeats++;
    }
	m_shptr.id = repeats + 1;
	m_shptr.command = 9;
	m_shptr.status =  0;
	m_shptr.address = 0x0;
	m_shptr.data = 0;
	printf(" VirtualServer send data: %u %u %u 0x%x %u\n", m_shptr.id, m_shptr.command, m_shptr.status, m_shptr.address, m_shptr.data);
	if (send_data(&server_address, &m_shptr) <0)
		exit(-1);
	printf(" VirtualServer reseived data: %u %u\n", m_shptr.status, m_shptr.data);
	return(0);
}
#endif
