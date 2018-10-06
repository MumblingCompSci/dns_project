//
// Created by quintero on 10/5/18.
//
#include "dug.h"

using namespace std;




// argv[0] = ./dug
int main (int argc, char *argv[]) {
    // The important arguments
    string web_name;
    string dns_host;

    struct Header {
        unsigned short id;

        // the good header stuff
        unsigned char qr :1; // query bit
        unsigned char opcode :4; // opcode
        unsigned char aa :1; // authoritative answer
        unsigned char tc :1; // truncate
        unsigned char rd :1; // recursion desired
        unsigned char ra :1; // recursion available
        unsigned char z  :4; // set to 0
        unsigned char rcode :4; // response code

        // pretty much the body of the packet
        unsigned q_count :16; // question entries
        unsigned ans_count :16; // answer entries
        unsigned auth_count :16; // authority entries
        unsigned add_count :16; // resource entries

    };

// ********************************************************************
    // * Process the command line arguments
    // ********************************************************************
    boost::log::add_console_log(std::cout, boost::log::keywords::format = "%Message%");
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    // ********************************************************************
    // * Process the command line arguments
    // ********************************************************************
    int opt = 0;
    int v = 0;
    while ((opt = getopt(argc,argv,"v")) != -1) {

        switch (opt) {
            case 'v':
                boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
                v = 1;
                break;
            case ':':
            case '?':
            default:
                std::cout << "useage: " << argv[0] << " -v" << std::endl;
                exit(-1);
        }
    }

    if (argc < 3 || argc > 4) {
      std::cout << "usage: " << argv[0] << " [-v] [name_to_lookup] [dns_server]" << endl;
      exit(-1);
    }

    web_name = argv[1 + v];
    dns_host = argv[2 + v];

    DEBUG << "Name to look up : " << web_name << ENDL;
    DEBUG << "DNS Server to use : " << dns_host << ENDL;
// ******************************************************************

    // Step One: create the socket
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    DEBUG << "Calling socket() -> " << sockfd << ENDL;

    // Step Two: fill in the address
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    if (!inet_aton("138.67.1.2", &servaddr.sin_addr)) {
        FATAL << "inet_aton failed" << ENDL;
        exit(-1);
    }

    // Step Three: call connect
    if (connect(sockfd, (const sockaddr*) &servaddr, sizeof(servaddr))) {
        FATAL << "Connect failed." << ENDL;
        FATAL << strerror(errno) << ENDL;
    }

    // Step Four: write and read as needed



    close(sockfd);
    DEBUG << "Socket " << sockfd << " closed." << ENDL;
    return 0;
}
