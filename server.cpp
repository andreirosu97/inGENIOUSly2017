/*UDP server version 1.0 */

#include "udp_client_server.h"
#include <string.h>
#include <unistd.h>

udp_server::udp_server(const std::string& addr)
{
	    char decimal_port[16];
	    f_addr=htonl(INADDR_ANY);
	    snprintf(decimal_port, sizeof(decimal_port), "%d", 8889);
	    decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';
	    struct addrinfo hints;
	    memset(&hints, 0, sizeof(hints));

	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_DGRAM;
	    hints.ai_protocol = IPPROTO_UDP;

	    int r(getaddrinfo(addr.c_str(), decimal_port, &hints, &f_addrinfo));
	    if(r != 0 || f_addrinfo == NULL)
	    {
		throw udp_client_server_runtime_error(("invalid address or port for UDP socket: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	    f_socket = socket(f_addrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
	    if(f_socket == -1)
	    {
		freeaddrinfo(f_addrinfo);
		throw udp_client_server_runtime_error(("could not create UDP socket for: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	    r = bind(f_socket, f_addrinfo->ai_addr, f_addrinfo->ai_addrlen);
	    if(r != 0)
	    {
		freeaddrinfo(f_addrinfo);
		close(f_socket);
		throw udp_client_server_runtime_error(("could not bind UDP socket with: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	}

	udp_server::~udp_server()
	{
	    std::cout<<"CLOSING SERVER!\n";
	    freeaddrinfo(f_addrinfo);
	    close(f_socket);
	}

	int udp_server::recv_msg(char *msg, size_t max_size)
	{
	    return recv(f_socket, msg, max_size, 0);
	}


	void udp_server::timed_recv(char *msg, size_t max_size, int max_wait_sec)
	{
	    fd_set s;
	    FD_ZERO(&s);
	    FD_SET(f_socket, &s);
	    struct timeval timeout;
	    timeout.tv_sec = max_wait_sec;
	    timeout.tv_usec = 0;
	    int retval = select(f_socket+1, &s, NULL ,NULL , &timeout);
	    if(retval == -1)
	    {
		std::cout<<"Select error!";
	    }
	    else
		if(retval==0)
		{
		    std::cout<<"Server timed out!\n";
		    return;
		}
		else
		{
		        recv(f_socket, msg, max_size, 0);
		        std::cout<<msg<<"\n";
			
		        timed_recv(msg,max_size,max_wait_sec);
		}
	  
}

int main()
{
    std::cout<<"Hello, this is the server!\n";  
    char recvBuff[1024]={0};
    udp_server server("127.0.0.1");
    server.timed_recv(recvBuff,sizeof(recvBuff)+1,5);
    return 0;
}

