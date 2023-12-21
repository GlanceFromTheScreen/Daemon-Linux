#ifndef INTERFACE_QUEUE_H
#define INTERFACE_QUEUE_H

#include <atomic>
#include <iostream>

#define ITEMS 10

class i_queue {
public:

    virtual void enqueue(int item) = 0;
    virtual bool dequeue(int& item) = 0;
    virtual bool empty() = 0;

    void get_items(int* arr) {
        int i = 0;
        Node* tmp = head.load();
        while (tmp) {
            arr[i] = tmp->data;
            tmp = tmp->next;
            i++;
        }
    }

    void print_items() {
        Node* tmp = head.load();
        std::cout << "queue data: ";
        if(!tmp) {
            std::cout << "empty";
        }
        else {
            while (tmp) {
                std::cout << tmp->data << ' ';
                tmp = tmp->next;
            }
        }
        std::cout << "\n";
    }

protected:
    struct Node {
        int data;
        Node* next;

        Node(const int& val) : data(val), next(nullptr) {}
    };

    std::atomic<Node*> head = nullptr;
    std::atomic<Node*> tail = nullptr;
};

#endif