#include "mcsp_queue.h"
#include <iostream>

MCSP::MCSP() {}

MCSP::~MCSP() {}

void MCSP::enqueue(int item) {
    Node* newNode = new Node(item);
    if (!head) {  // очередь пуста
        head.store(newNode, std::memory_order_release);
        tail.store(newNode, std::memory_order_release);
    } else { 
        Node* new_node = new Node(item);
        Node* old_tail = tail.exchange(new_node);
        if (old_tail) {
            old_tail->next = new_node;
        } else {  // сработает если в этот момент очередь стала пустой
            head.store(new_node);
        }
    }

}

bool MCSP::dequeue(int& item) {
    Node* currentHead = head.load();
    if (!currentHead) {
        return false;  // пустая очередь
    } else {
        item = currentHead->data;
        Node* newHead;
        do {
            newHead = currentHead->next;
        } while (!head.compare_exchange_weak(currentHead, newHead));

        
        delete currentHead;

        return true;
    } 
}

bool MCSP::empty() {
    if (head == nullptr && tail == nullptr) {
        return true;
    }
    return false;
}

