//
// Created by quintero on 10/5/18.
//
#include "dug.h"

using namespace std;

void processHostname(char *name);

// argv[0] = ./dug
int main (int argc, char *argv[]) {
    // Step One : Create the socket //

    // Step Two : Connect that socket //

    // Step Three : 


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
