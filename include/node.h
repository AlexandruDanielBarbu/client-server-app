#ifndef Node_H
#define Node_H

#include <vector>
#include <string>

#include "client.h"

using namespace std;

class Client;

class Node {
  private:
    vector<Client*> subscribers;
    vector<Node*> children;
    string word;

  public:
    Node(string word);
    ~Node();
    
    vector<Client*> get_clients();
    string get_word();
    vector<Node*> get_children();
    int add_subscriber(Client *subscriber);
    void remove_subscriber(Client *subscriber);
    void add_child(Node *child);
    bool find_child_with_word(string wd);
    Node *find_child_by_word(string wd);
    void print_node_recursively();
};

#endif  // Node_H