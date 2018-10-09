//
// Created by quintero on 10/5/18.
//
#include "dug.h"

using namespace std;

void processHostname(char *input);

// argv[0] = ./dug
int main (int argc, char *argv[]) {

    char * name = argv[1];
    char * dns_serv = argv[2];

    cout << "Input name        : " << name << endl;
    cout << "DNS Server        : " << dns_serv << endl;
    processHostname(name);
    cout << "Converted Message : " << name << endl;

    struct Header{
        uint16_t id;

        struct {
            uint8_t rd : 1;
            uint8_t tc : 1;
            uint8_t aa : 1;
            uint8_t opcode : 4;
            uint8_t qr : 1;

            uint8_t rcode : 4;
            uint8_t z : 3;
            uint8_t ra : 1;
        } flags;

        uint16_t q_count;
        uint16_t ans_count;
        uint16_t auth_count;
        uint16_t add_count; 
    };

    struct Q_details{
        uint16_t qtype;
        uint16_t qclass;
    };



    // Step One: create the socket
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    DEBUG << "Calling socket() -> " << sockfd << ENDL;

    // Step Two: fill in the address
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = PF_INET;
    servaddr.sin_port = htons(53);

    if (!inet_aton(argv[2], &servaddr.sin_addr)) {
        FATAL << "inet_aton failed" << ENDL;
        exit(-1);
    }

    // Step Three: call connect
    // if (connect(sockfd, (const sockaddr*) &servaddr, sizeof(servaddr))) {
    //     FATAL << "Connect failed." << ENDL;
    //     FATAL << strerror(errno) << ENDL;
    // }

    // Step Four : read and write
    // write up the packet
    Header *header = (Header *) calloc(1, sizeof(struct Header));
    header->id = htons(getpid());
    header->flags.rd = htons(0);
    header->flags.tc = htons(0);
    header->flags.aa = htons(0);
    header->flags.opcode = htons(0);
    header->flags.qr = htons(0);
    header->flags.rcode = htons(0);
    header->flags.z = htons(0);
    header->flags.ra = htons(0);
    header->q_count = htons(1);
    header->ans_count = htons(0);
    header->auth_count = htons(0);
    header->add_count = htons(0);

    Q_details * q_details = (Q_details *) calloc(1, sizeof(Q_details));
    q_details->qtype = htons(1);
    q_details->qclass = htons(1);

    //write stuff to the buffer
    char * write_buff = (char *) calloc(1024, sizeof(char));
    int total_size = 0;
    memcpy(write_buff, header, sizeof(struct Header));
    total_size += sizeof(struct Header);
    memcpy(write_buff + total_size, name, sizeof(name));
    total_size += sizeof(name);
    memcpy(write_buff + total_size, q_details, sizeof(struct Q_details));
    total_size += sizeof(struct Q_details);

    cout << "Sort of expected size : " << total_size << endl;
    char * read_buff = (char *) calloc(1024, sizeof(char));

    // Send (write) the query
    int bytesSent = -1;
    // if ((bytesSent = write(sockfd, write_buff, total_size)) < 0) {
    //     cerr << "write failed" << endl;
    //     cerr << strerror(errno) << endl;
    //     close(sockfd);
    //     exit(-1);
    // }
    if ((bytesSent = sendto(sockfd, write_buff, total_size, 0,(const sockaddr*) &servaddr, sizeof(servaddr))) < 0) {
        cerr << "sendto failed" << endl;
        cerr << strerror(errno) << endl;
        close(sockfd);
        exit(-1);
    }
    cout << "Sent " << bytesSent << " bytes" << endl;

    //read the result
    int bytesRead = -1;
    if ((bytesRead = read(sockfd, read_buff, 5000)) < 0) {
        cerr << "read failed" << endl;
        cerr << strerror(errno) << endl;
        close(sockfd);
        exit(-1);
    }

    // Finally, close things out
    cout << "Closing Socket" << endl;
    close(sockfd);

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
}

