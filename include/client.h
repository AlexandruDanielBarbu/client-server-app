#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include <vector>
#include <string>

using namespace std;

class Node;

class Client {
  private:
    string id;
    int known_port;
    std::string known_ip_address;
    vector<Node*> subscribed_topics;
    int client_fd;

  public:
    Client() = default;
    Client(string id, int known_port, std::string known_ip_address);
    ~Client();

    int get_client_fd();
    void set_client_fd(int fd);
    string get_id();
    string get_client_ip();
    int get_client_port_number();
    void subscribe_to_topic(Node *topic_end_node);
    void unsubscribe_client_from_all_topics();
    int unsubscribe_client_from_topic(Node *root, string topic);
    void print_client();
    string serialize_client();
};

#endif  // CLIENT_H