#include "topic_tree.h"
#include "alex_simple_protocol.h"

int add_path_to_tree(Node *root, Client *subscriber, string topic) {
    vector<string> tokens;
    stringstream check1(topic);
    string intermediate;

    while(getline(check1, intermediate, '/')) {
        tokens.push_back(intermediate);
    }

    for (string wd : tokens) {
        Node *next = root->find_child_by_word(wd); 
        
        if (next) {
            root = next;  // advance the insertion
        } else {
            Node *child = new Node(wd);     // create the new node
            root->add_child(child);         // add the new note to children list
            root = child;                   // advance search to the child
        }
    }

    int rez = root->add_subscriber(subscriber);
    return rez;
}

void topic_tree_cleanup(Node *root) {
    for (Node *child : root->get_children()) {
        topic_tree_cleanup(child);
    }
    
    delete root;
}

void notify_clients(Node *root, const vector<string>& topic, vector<string>::iterator it, string& data) {
    if (it == topic.end()) {
        // cout << "busted it" << endl;
        for (Client *sub : root->get_clients()) {
            // send data
            send_response_data(data, sub->get_client_fd());
        }
        return;
    }
        
    stack<Node*> dfs_stack;

    for (Node *child : root->get_children()) {
        dfs_stack.push(child);
    }

    while (!dfs_stack.empty()) {
        Node *nod = dfs_stack.top();
        dfs_stack.pop();

        // cout << "Verific: " << nod->get_word() << " cu it: " << *it << endl;
        if (nod->get_word() != *it &&
            nod->get_word() != string("*") &&
            nod->get_word() != string("+")) {
            // cout << "am dat continue" << endl;
            continue;
        }

        if (nod->get_word() == *it || nod->get_word() == string("+")) {
            // cout << "cazul cu + sau cuvantul e acelasi" << endl;
            notify_clients(nod, topic, next(it), data);
        } else if (nod->get_word() == string("*")) {
            for (vector<string>::iterator itr = it; itr != topic.end(); itr++) {
                Node *rez = nod->find_child_by_word(*itr);
                if (rez) {
                    // s-a gasit copil cu acest cuvant
                    // // cout << "caz steluta, caut de la " << *it<< " la " << *(topic.end() - 1) << endl;
                    // cout << "gasit la: " << *itr << endl;
                    notify_clients(rez, topic, next(itr), data);
                }
            }
        }
    }
}

// int main() {
//     Client client1("1", 1010, "123456");
//     Client client2("2", 2020, "123457");
//     Client client3("3", 3030, "123458");
//     Client client4("4", 4040, "123459");

//     Node* topic_tree_root = new Node("");

//     add_path_to_tree(topic_tree_root, &client1, "upb/precis/temp");
//     add_path_to_tree(topic_tree_root, &client2, "+/precis/temp");
//     add_path_to_tree(topic_tree_root, &client3, "*/temp");
//     add_path_to_tree(topic_tree_root, &client4, "*/temp");

//     topic_tree_root->print_node_recursively();

//     // if (client1.unsubscribe_client_from_topic(topic_tree_root, "*") == -1) {
//     //     // cout<<"nu s-a putut da unsub" <<endl;
//     // }

//     // if (client1.unsubscribe_client_from_topic(topic_tree_root, "upb/precis/temp") == 0)
//     // {
//     //     // cout << "s-a dat unsub" << endl;   
//     // }
    
//     vector<string> topics;
//     topics.push_back(string("upb"));
//     topics.push_back(string("precis"));
//     topics.push_back(string("temp"));

//     // notify_clients(topic_tree_root, topics, topics.begin());

//     topic_tree_cleanup(topic_tree_root);
//     // cout << "i think that all is good\n";

//     // cout << client1.serialize_client() << endl;
//     return 0;
// }
