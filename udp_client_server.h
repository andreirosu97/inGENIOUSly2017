

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <netdb.h>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <arpa/inet.h>
#include <netinet/in.h>

	class udp_client_server_runtime_error : public std::runtime_error
	{
	public:
	    udp_client_server_runtime_error(const char *w) : std::runtime_error(w) {}
	};


class udp_client
{
	public:
		udp_client(const char *addr);
		~udp_client();
		void operator()(){return;};
	  void  send(const char *msg, size_t size);

	private:
	    int                 f_socket;
	    struct sockaddr_in  me;
};


class udp_server
{
	public:
		udp_server();
		~udp_server();
		int  recv_msg(char *msg, size_t max_size);
		void timed_recv(char *msg, size_t max_size, int max_wait_sec);

	private:
		int f_socket;
		struct sockaddr_in  me;
};
