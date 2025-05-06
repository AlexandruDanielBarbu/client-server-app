#include "client.h"
#include <iostream>
#include <sstream>
#include <arpa/inet.h> 
#include "client.h"
#include "node.h"

Client::Client(string id, int known_port, std::string known_ip_address) {
    this->id = id;
    this->known_port = known_port;
    this->known_ip_address = known_ip_address;
    this->client_fd = -1;
}
Client::~Client() {
    // nothing
}

int Client::get_client_fd() {
    return client_fd;
}
    
void Client::set_client_fd(int fd) {
    this->client_fd = fd;
}

string Client::get_id() {
    return id;
}

string Client::get_client_ip() {
    return known_ip_address;
}

int Client::get_client_port_number() {
    return known_port;
}

void Client::subscribe_to_topic(Node *topic_end_node) {
    subscribed_topics.push_back(topic_end_node);
}

void Client::unsubscribe_client_from_all_topics() {
    for (Node *topic_node : subscribed_topics) {
        topic_node->remove_subscriber(this);
    }
}

int Client::unsubscribe_client_from_topic(Node *root, string topic) {
    vector<string> tokens;
    stringstream check1(topic);
    string intermediate;

    while(getline(check1, intermediate, '/')) {
        tokens.push_back(intermediate);
    }

    for (string wd : tokens) {
        Node *next = root->find_child_by_word(wd); 
        
        if (next) {
            root = next;  // advance the search
        } else {
            return -1;    // no match found
        }
    }

    // At the end unsubscribe
    root->remove_subscriber(this);
    return 0;
}

void Client::print_client() {
    cout << "ID: " << id << "\n"
            << "Port: " << known_port << "\n"
            << "IP: " << known_ip_address << "\n";
        //  << "Topic: " << topic << endl;
}

// sends ints floats etc in network order
string Client::serialize_client(){
    return id + " " + known_ip_address + " " + to_string(known_port);
}
