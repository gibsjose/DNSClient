#include <iostream>
#include <sys/socket.h> //Socket features
#include <netinet/in.h> //Internet-specific features of sockets
#include <arpa/inet.h>

#include "DNSPacket.h"
#include "ConfigManager.hpp"
#include "Exception.h"

int main(int argc, char * argv[]) {
    //Initialization
    ConfigManager lConfigManager;
    try
    {
        lConfigManager.parseArgs(argc, argv);
    }
    catch(ParseException & e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return -2;
    }
    catch(FileIOException & e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return -3;
    }
    catch(...)
    {
        std::cerr << "Error: unhandled exception.\n";
        return -1;
    }

    std::cout << "Resolver info:" << std::endl;
    std::cout << "IP: " << lConfigManager.getResolverIPString() << std::endl;
    std::cout << "Port: " << lConfigManager.getResolverPort() << std::endl;

    //Connect to server
    char port_str[16];
    char address[64];   //Address of DNS resolver, get from config class
    char domain[64];    //Domain used for request
    unsigned int port;  //Port of DNS resolver, get from config class

    //Create a socket:
    //socket() system call creates a socket, returning a socket descriptor
    //  AF_INET specifies the address family for internet
    //  SOCK_DGRAM says we want UDP as our transport layer
    //  0 is a parameter used for some options for certain types of sockets, unused for INET sockets
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    //Create timeval struct
    struct timeval to;
    to.tv_sec = 5;
    to.tv_usec = 0;

    //Make socket only wait for 5 seconds w/ setsockopt
    //  socket descriptor
    //  socket level (internet sockets, local sockets, etc.)
    //  option we want (SO_RCVTIMEO = Receive timeout)
    //  timeout structure
    //  size of structure
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));

    if(sockfd < 0) {
        std::cerr << "Could not open socket\n";
        return -1;
    }

    std::cout << "===========================================\n";
    std::cout << "Connecting to " << address << ":" << port <<std::endl;
    std::cout << "===========================================\n" << std::endl;

    //inet_addr() converts a string-address into the proper type
    //Specify the address for the socket
    //Create the socket address structure and populate it's fields
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;                            //Specify the family again (AF_INET = internet family)
    serveraddr.sin_port = htons(port);                          //Specify the port on which to send data (16-bit) (# < 1024 is off-limits)
    serveraddr.sin_addr.s_addr = inet_addr(address);        //Specify the IP address of the server with which to communicate

    //"Connect" to the server by sending it 'STX' and expect an 'ACK' back.
    char lSTX = STX_CHAR;
    if(!send_get_ack(&lSTX, 1, sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) {
        std::cerr << "Could not connect to server\n";
        return -1;
    }

    fd_set sockets;

    // Clear the fd set
    FD_ZERO(&sockets);

    //Add server socket and stdin to file desciptor set
    FD_SET(sockfd, &sockets);
    FD_SET(STDIN_FILENO, &sockets);

    std::cout << "DNS Client" << std::endl;

    char * response = (char*)malloc(MAX_INPUT_SIZE);

    while(1) {
        //Get domain from user
        std::cout << "Enter a domain name: " << std::endl;
        fgets(domain, 64, stdin);
        ntrim(domain);

        //Create a DNS packet
        std::string name(domain);
        DNSPacket packet(name);
        packet.Print();

        //Send packet to server

        //Wait for response

        //Print response
    }

    //Close
}

//Trim the first newline character from the string
void ntrim(char *str) {
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}
