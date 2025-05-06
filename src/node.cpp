#include "node.h"
#include <iostream>

Node::Node(string word) {
    this->word = word;
}

Node::~Node() {
    // nothing
}

vector<Client*> Node::get_clients() {
    return subscribers;
}

string Node::get_word() {
    return word;
}

vector<Node*> Node::get_children() {
    return children;
}

int Node::add_subscriber(Client *subscriber) {
    for(Client *sub : subscribers) {
        if (sub->get_id() == subscriber->get_id())
        {
            return -1; // nu s-a adaugat
        }
    }

    subscribers.push_back(subscriber);
    return 0;  // s-a adaugat!
}

void Node::remove_subscriber(Client *subscriber) {
    for (vector<Client*>::iterator it = subscribers.begin(); it != subscribers.end(); it++) {
        if (subscriber->get_id() == (*it)->get_id()) {
            subscribers.erase(it);
            break;
        }
    }
}

void Node::add_child(Node *child) {
    children.push_back(child);
}

bool Node::find_child_with_word(string wd) {
    for(Node *child : children) {
        if (child->get_word() == wd)
            return true;
    }

    return false;
}

Node *Node::find_child_by_word(string wd) {
    for(Node *child : children) {
        if (child->get_word() == wd)
            return child;
    }

    return NULL;
}

void Node::print_node_recursively() {
    cout << word << endl;
    for (Node *child : children) {
        child->print_node_recursively();
    }
}