

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

	class udp_client_server_runtime_error : public std::runtime_error
	{
	public:
	    udp_client_server_runtime_error(const char *w) : std::runtime_error(w) {}
	};


	class udp_client
	{
	public:
		void operator()(){
			return;
			};

		udp_client(const std::string& addr);
		~udp_client();
	  	void  send(const char *msg, size_t size);
			void test(int n);
	private:
	    int                 f_socket;
	    std::string         f_addr;
	    struct addrinfo *   f_addrinfo;
	};


	class udp_server
	{
	public:
		                udp_server(const std::string& addr);
		                ~udp_server();

	    int                 recv_msg(char *msg, size_t max_size);
	    void                timed_recv(char *msg, size_t max_size, int max_wait_sec);

	private:
	    int                 f_socket;
	    std::string         f_addr;
	    struct addrinfo *   f_addrinfo;
};
