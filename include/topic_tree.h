#ifndef TOPIC_TREE_H
#define TOPIC_TREE_H

#include <iostream>
#include <vector>
#include <sstream> 
#include <stdint.h>
#include <stack>

#include "client.h"
#include "node.h"

using namespace std;

int add_path_to_tree(Node *root, Client *subscriber, string topic);
void topic_tree_cleanup(Node *root);
void notify_clients(Node *root, const vector<string>& topic, vector<string>::iterator it, string& data);

#endif  // TOPIC_TREE_H