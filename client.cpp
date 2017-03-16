/*UDP client version 1.0 */

#include "udp_client_server.h"
#include <string.h>
#include <unistd.h>



namespace udp_client_server
{

	udp_client::udp_client(const std::string& addr): f_addr(addr)
	{
	    char decimal_port[16];
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
		throw udp_client_server_runtime_error(("invalid address or port: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	    f_socket = socket(f_addrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
	    if(f_socket == -1)
	    {
		freeaddrinfo(f_addrinfo);
		throw udp_client_server_runtime_error(("could not create socket for: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	}

	udp_client::~udp_client()
	{
	    freeaddrinfo(f_addrinfo);
	    close(f_socket);
	}


	int udp_client::send(const char *msg, size_t size)
	{
	    return sendto(f_socket, msg, size, 0, f_addrinfo->ai_addr, f_addrinfo->ai_addrlen);
	}

}

int main()
{
    std::cout<<"Hello, this is the client!\n";  
    char sendBuff[1024]={0};
    std::cin.get(sendBuff,1024);
    udp_client_server::udp_client client("127.0.0.1");
    client.send(sendBuff,sizeof(sendBuff)+1);

    return 0;
}
