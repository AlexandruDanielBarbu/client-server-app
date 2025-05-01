#include <iostream>
#include <vector>
#include <sstream> 
#include <stdint.h>

using namespace std;

class client {
  private:
    int id;
    int known_port;
    uint32_t known_ip_address;

  public:
    string topic;
    
    client(int id, int known_port, uint32_t known_ip_address, string topic) {
        this->id = id;
        this->known_port = known_port;
        this->known_ip_address = known_ip_address;
        this->topic = topic;
    }
    ~client() {
        // nothing
    }

    void print_client() {
        cout << "ID: " << id << "\n"
             << "Port: " << known_port << "\n"
             << "IP: " << known_ip_address << "\n"
             << "Topic: " << topic << endl;
    }
};

class node {
  private:
    vector<client*> subscribers;
    vector<node*> children;
    string word;

  public:
    node(string word) {
        this->word = word;
    }
    ~node() {
        // nothing
    }
    
    vector<client*> get_clients() {
        return subscribers;
    }

    string get_word() {
        return word;
    }

    vector<node*> get_children() {
        return children;
    }

    void add_subscriber(client *subscriber) {
        subscribers.push_back(subscriber);
    }

    void add_child(node *child) {
        children.push_back(child);
    }

    bool find_child_with_word(string wd) {
        for(node *child : children) {
            if (child->get_word() == wd)
                return true;
        }

        return false;
    }

    node *find_child_by_word(string wd) {
        for(node *child : children) {
            if (child->get_word() == wd)
                return child;
        }

        return NULL;
    }

    void print_node_recursively() {
        cout << word << endl;
        for (node *child : children) {
            child->print_node_recursively();
        }
    }
};

void add_path_to_tree(node *root, client *subscriber) {
    vector<string> tokens;
    stringstream check1(subscriber->topic);
    string intermediate;

    while(getline(check1, intermediate, '/')) {
        tokens.push_back(intermediate);
    }

    for (string wd : tokens) {
        node *next = root->find_child_by_word(wd); 
        
        if (next) {
            root = next;  // advance the insertion
        } else {
            node *child = new node(wd);     // create the new node
            root->add_child(child);         // add the new note to children list
            root = child;                   // advance search to the child
        }
    }

    root->add_subscriber(subscriber);
}

void topic_tree_cleanup(node *root) {
    for (node *child : root->get_children()) {
        topic_tree_cleanup(child);
    }
    
    delete root;
}

int main() {
    client client1(1, 1010, 123456, "upb/precis/temp");
    client client2(2, 2020, 123457, "+/precis/temp");
    client client3(3, 3030, 123458, "+/temp");

    node* topic_tree_root = new node("");

    add_path_to_tree(topic_tree_root, &client1);
    add_path_to_tree(topic_tree_root, &client2);
    add_path_to_tree(topic_tree_root, &client3);

    topic_tree_root->print_node_recursively();
    topic_tree_cleanup(topic_tree_root);
    cout << "i think that all is good\n";
    return 0;
}
