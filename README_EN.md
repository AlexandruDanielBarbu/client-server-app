# Client Server App
Barbu Alexandru Daniel

## Table of Contents

- [Context](#context)
- [Project Description](#project-description)
- [What I Learned](#what-i-learned-from-this-project)
- [How to Run](#how-to-run)
- [Possible Commands](#possible-commands)
  - [Server](#server)
  - [Client](#client)
- [Implementation Details](#implementation-details)
  - [Why epoll and not poll](#why-epoll-and-not-poll)
  - [How do I transmit data from client to server and vice versa?](#how-do-i-transmit-data-from-client-to-server-and-vice-versa)
  - [How and when to exit, and in what order](#how-and-when-to-exit-and-in-what-order)
  - [How I implemented wildcards](#how-i-implemented-wildcards)
  - [Why a small number for listen?](#why-a-small-number-for-listen)
  - [How many clients can connect to the server?](#how-many-clients-can-connect-to-the-server)
  - [Client Output](#client-output)
  - [Why C++?](#why-c)
  - [Makefile](#makefile)
- [What I Added](#what-i-added)
- [What Could Be Improved](#what-could-be-improved)
- [Impressions about the Project](#impressions-about-the-project)

---

## Context

This project was initially developed as an assignment for a university networking course. It was later extended and improved to reflect a clearer architecture and a more professional, practice-oriented approach.

---

## Project Description

The project is a client-server application.

Multiple UDP clients can connect to a server and send messages to specific topics, which multiple TCP clients can subscribe to.

---

## What I Learned from This Project

- I gained practical experience working with **sockets** and implementing a robust client-server application.
- I consolidated my knowledge of **object-oriented programming** using **C++ classes**, structuring the code in a clear and modular way.
- I consciously applied **RAII (Resource Acquisition Is Initialization)** principles to efficiently manage resources in components like `Socket.h` and `Epoll.h`.
- I learned how to **integrate C++ code with C code**, which gave me a clearer perspective on the interoperability between the two languages and extended my ability to work with existing libraries.
- I familiarized myself with STL concepts, such as **`std::vector`**, used for dynamic data management.
- I learned to **better plan and structure my work**, anticipating necessary steps and adjusting my approach to complete the project efficiently and sustainably.

---

## How to Run

> NOTE: An implemented UDP client is needed to fully test the application.

```bash
make # compiles everything
make run_server # starts the server on port 4040
make run_subscriber # starts the client with ID "C1" on IP address 127.0.0.1 on the server's port 4040
```

## Possible Commands
### Server

On the server, only `exit` can be entered. Any other string or incorrect capitalization of `exit` will be ignored.

``` bash
exit
```

### Client

```bash
subscribe ana/are/mere          # this will subscribe to the topic ana/are/mere
unsubscribe ana/are/pere        # this will unsubscribe from the topic ana/are/pere
exit                            # this will exit the application
```

## Implementation Details
### Why epoll and not poll

Honestly, I started with `epoll` by mistake. I was familiar with the term, and starting the assignment late, I thought we had covered `epoll` in the lab, so I continued with `epoll`. From what I understood from online research, `epoll` is faster than `poll` or `select`, performing selection in **O(1)**.

The article I used to learn how to use `epoll` is [this one](https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642).

### How do I transmit data from client to server and vice versa?

Using ***Alex Simple Protocol (ASP)*** :)))) It's a custom invention that transmits data in a "human-readable" format, controlling what happens with a series of flags.

### How and when to exit, and in what order

When the client exits, it informs the server that it's exiting and then simply terminates execution. If it receives a message from the server intended for it, indicating that it can exit, and the client hasn't already exited, it will exit. There's no particular reason for a client to wait for a confirmation signal from the server or not to exit if the server says "no," but I left it in case I wanted to add something later.

The server only exits after closing all clients, unsubscribing them from all topics, and deallocating the topic tree.

### How I implemented wildcards

> CONTEXT: A client can subscribe or unsubscribe to topics like `ana/*`, which matches topics starting with `ana/` and ending or being followed by any number of levels. The same concept applies to `+`, which generally does a similar thing.

I created a tree from the topics to which clients subscribe. In the leaf nodes, I store the clients subscribed to that specific topic. Client notification is done by traversing the tree and notifying clients in the leaf node.

When I encounter my custom word or `+` during topic parsing, I accept it and move forward. If I encounter `*`, I accept it and continue traversing the graph, keeping in mind that I also want to search for the rest of the words in the topic among the children of the `*` node.

> NOTE: It's possible that tests that run out of time do so because of the algorithm for `*`.

### Why a small number for listen?

From what I understand, the maximum number of users set in the `listen` function is a client queue used only if there are pending clients; it doesn't limit in any way how many clients can connect to the server.

### How many clients can connect to the server?

As many as needed! Periodically, I check the size of `events_array` and double it if necessary.

### Client Output

The requirement was `<IP client udp>:<port client udp> ....`. I implemented this feature, but I commented it out to make the checker work.

### Why C++?

I wanted to learn C++ and desired to show that I could handle this language.

### Makefile

I didn't create it myself; artificial intelligence helped me make it. In my defense, I had a primitive makefile already, but it wasn't generic enough for so many files. However, I did think independently about generating object files and intermediate binaries in separate folders.

## What I Added

The original version used a naive socket implementation with many `goto` statements and system call checks. To unify the process, I created a `Socket` and `Epoll` class so that if something goes wrong, I can `return` and have the sockets automatically closed.

## What Could Be Improved

Buffer allocations: Now that I know more, I could have extensively used `vector<char>data.resize` to perform `memcpy`, `memset`, etc., and then use `data.data()`.

## Impressions about the Project

I can't wait to put it on Git, refine it further, add it to my CV, etc. In short, I enjoyed it more towards the end when I was debugging and truly understood what was happening. The project was interesting and a welcome challenge.