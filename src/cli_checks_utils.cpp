#include <iostream>
#include <cstring>

using namespace std;

bool is_port_number(const char *number) {
    for (size_t i = 0; i < strlen(number); i++)
        if (!isdigit(number[i]))
            return false;

    if (stoi(number) < 0 ||
        stoi(number) > 65535)
        return false;

    return true;
}

bool is_pos_number(const char *number) {
    for (size_t i = 0; i < strlen(number); i++)
        if (!isdigit(number[i]))
            return false;

    if (stoi(number) < 0)
        return false;

    return true;
}

bool is_ip_address(const char *ip_address) {
    char *address = strdup(ip_address), *p;
    char dlm = '.';
    int n = 0;

    p = strtok(address, &dlm);


    while (p) {
        if (!is_pos_number(p) ||
            stoi(p) > 255) {
            free(address);
            return false;
        }
        n++;   

        p = strtok(NULL, &dlm); 
    }

    if (n != 4) {
        free(address);
        return false;
    }
    
    // A group of 4 numbers in range [0, 255]
    free(address);
    return true;
}
