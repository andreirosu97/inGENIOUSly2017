/*UDP client version 1.0 */

#include "udp_client_server.h"
#include <string.h>
#include <unistd.h>
#include <iostream>

udp_client::udp_client(const char *hostname) {
	memset( (char*) &me ,0 ,sizeof(me));
	struct hostent *hostinfo;

	f_socket = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
	if(f_socket == -1)
		throw udp_client_server_runtime_error("could not create UDP socket for: \"");

	me.sin_family = AF_INET;
	me.sin_port = htons(5000);
	hostinfo= gethostbyname(hostname);
	if(!hostinfo) {
		fprintf(stderr,"Unknown host %s.\n",hostname);
		exit(EXIT_FAILURE);
	}
	me.sin_addr=*(struct in_addr *)hostinfo->h_addr;
}

udp_client::~udp_client() {
	close(f_socket);
}

void udp_client::send(const char *msg, size_t size) {//sending info at speed = 1 pack/100ns
  struct timespec timeout;
  timeout.tv_sec = 0;
  timeout.tv_nsec = 100;
	pselect(f_socket+1, NULL, NULL ,NULL , &timeout, NULL);
	sendto(f_socket, msg, size, 0, (struct sockaddr *)&me,sizeof(me));
	send(msg,size);
}

int main(int argc, char *argv[]) {
	std::cout<<"Hello, this is the client!\n";
	if(argc<2) {
	  fprintf(stderr,"No hostname recieved!\n");
		exit(EXIT_FAILURE);
	}
	udp_client client(argv[1]);
	char sendBuff[1024]={0};
	std::cin.get(sendBuff,1024);
	std::thread client_th(&udp_client::send,&client,sendBuff,strlen(sendBuff));
	client_th.join();
  return 0;
}
