#include "scsp_queue.h"
#include <iostream>

SCSP::SCSP() {}

SCSP::~SCSP() {}

void SCSP::enqueue(int item) {
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

bool SCSP::dequeue(int& item) {
    // особенный для SCSP метод
    Node* current_head = head.load();
    if (!current_head) {
        return false;  // пустая очередь
    } else {
        item = current_head->data;
        head.store(current_head->next);
        if (!head.load()) {
            tail = nullptr;
        }
        delete current_head;
    }

    return true;
}

bool SCSP::empty() {
    if (head == nullptr && tail == nullptr) {
        return true;
    }
    return false;
}

