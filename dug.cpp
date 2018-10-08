//
// Created by quintero on 10/5/18.
//
#include "dug.h"

using namespace std;

void processHostname(char *name);

// argv[0] = ./dug
int main (int argc, char *argv[]) {
    char * web_name = (char*) calloc(100, sizeof(char));
    char * dns_address = (char*) calloc(100, sizeof(char));
    char * write_buffer = (char*) calloc(1024, sizeof(char));
    char * read_buffer = (char*) calloc(1024, sizeof(char));

    struct Header {
        unsigned short id;

        // the good header stuff
        struct {
        unsigned char qr :1; // query bit
        unsigned char opcode :4; // opcode
        unsigned char aa :1; // authoritative answer
        unsigned char tc :1; // truncate
        unsigned char rd :1; // recursion desired
        unsigned char ra :1; // recursion available
        unsigned char z  :4; // set to 0
        unsigned char rcode :4; // response code
    } flags;

        // pretty much the body of the packet
        unsigned q_count :16; // question entries
        unsigned ans_count :16; // answer entries
        unsigned auth_count :16; // authority entries
        unsigned add_count :16; // resource entries

    };

   struct Question_details {
	    unsigned qtype :16; // type of query
	    unsigned qclass :16; // query class
    };

    struct Answer {
	string name;
	unsigned type :16;
	unsigned a_class :16;
	unsigned ttl :32;
	unsigned rdlength :16;
	string rdata;
    };

    // * Process the command line arguments
    boost::log::add_console_log(std::cout, boost::log::keywords::format = "%Message%");
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    // * Process the command line arguments
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


    web_name = argv[1+v];
    dns_address = argv[2+v];
    DEBUG << "Name to look up : " << web_name << ENDL;
    DEBUG << "DNS Server to use : " << dns_address << ENDL;

    processHostname(web_name);
// ******************************************************************

    // Step One: create the socket
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    DEBUG << "Calling socket() -> " << sockfd << ENDL;

    // Step Two: fill in the address
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = PF_INET;
    servaddr.sin_port = htons(53);
    servaddr.sin_addr.s_addr = inet_addr(dns_address);

    if (!inet_aton(argv[2 + v], &servaddr.sin_addr)) {
        FATAL << "inet_aton failed" << ENDL;
        exit(-1);
    }

    // Step Three: call connect
    if (connect(sockfd, (const sockaddr*) &servaddr, sizeof(servaddr))) {
        FATAL << "Connect failed." << ENDL;
        FATAL << strerror(errno) << ENDL;
    }

    // Step Four: write and read as needed
    int bytesRead = 0;
    int bytesSent = 0;

    // create the header, name, and qdetails
    Header * hdr = (struct Header *) malloc(sizeof(struct Header));
    srand(time(NULL));
    hdr->id = (unsigned short) htons(getpid());
    hdr->flags.qr = htons(1);
    hdr->flags.opcode = htons(0);
    hdr->flags.aa = htons(0);
    hdr->flags.tc = htons(0);
    hdr->flags.rd = htons(0);
    hdr->flags.ra = htons(0);
    hdr->flags.z = htons(0);
    hdr->flags.rcode = htons(0);
    hdr->q_count = htons(1);
    hdr->ans_count = htons(0);
    hdr->auth_count = htons(0);
    hdr->add_count = htons(0);

    Question_details *qdetails = (struct Question_details *) malloc(sizeof(struct Question_details));
    qdetails->qtype = htons(1);
    qdetails->qclass = htons(1);

    int totalSize = 0;
    memcpy(write_buffer, hdr, sizeof(struct Header));
    totalSize += sizeof(struct Header);
    memcpy(write_buffer+totalSize, web_name, sizeof(web_name));
    totalSize += sizeof(web_name);
    memcpy(write_buffer+totalSize, qdetails, sizeof(Question_details));
    totalSize += sizeof(Question_details);

    if ((bytesSent = write(sockfd, write_buffer, totalSize)) < 0) {
        FATAL << "write returned error " << strerror(errno) << ENDL;
    }

    DEBUG << "Sent " << bytesSent << " bytes" << ENDL;

    if ((bytesRead = read(sockfd, read_buffer,5000)) < 0) {
        FATAL << "read returned error "<< strerror(errno) << ENDL;
    }

    DEBUG << "Received " << bytesRead << " bytes" << ENDL;

    close(sockfd);
    DEBUG << "Socket " << sockfd << " closed." << ENDL;
    return 0;
}

void processHostname(char * input) {
    string name = string(input);
    // 3www5mines3edu0

    string str = "";
    int counter = 0;
    int last = 0;
    for (int i = 0; i < name.length(); i++) {
        if (name[i] != '.') {
            counter++;
        } else {
            if (last == 0) { last--; }
            str = str + to_string(counter) + name.substr(last + 1, counter);
            last = i;
            counter = 0;
        }
    }
    str = str + to_string(counter) + name.substr(last + 1) + '0';
    
    strcpy(input, str.c_str());

    DEBUG << "Processed input name: " << input << ENDL;
}
