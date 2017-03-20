/*UDP server version 1.0 */

#include "udp_client_server.h"
#include <string.h>
#include <unistd.h>

udp_server::udp_server()
{
			struct sockaddr_in me;
	    memset( (char*) &me ,0 ,sizeof(me));

	    f_socket = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
	    if(f_socket == -1)
				throw udp_client_server_runtime_error("could not create UDP socket for: \"");

			me.sin_family = AF_INET;
			me.sin_port = htons(8889);
			me.sin_addr.s_addr=htonl(INADDR_ANY);

		 	if(bind( f_socket, (const sockaddr*) &me, sizeof(me))==-1)
	    {
				close(f_socket);
				throw udp_client_server_runtime_error("could not bind UDP socket with: \"" );
	    }
}

	udp_server::~udp_server()
	{
	    std::cout<<"CLOSING SERVER!\n";
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
    udp_server server;
    server.timed_recv(recvBuff,sizeof(recvBuff)+1,5);
    return 0;
}
