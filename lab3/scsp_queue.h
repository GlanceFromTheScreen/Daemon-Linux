#ifndef SCSP_QUEUE_H
#define SCSP_QUEUE_H

#include "interface_queue.h"

class SCSP: public i_queue {

public:
    SCSP ();
    ~SCSP();
    void enqueue(int item) override;
    bool dequeue(int& item) override;
    bool empty() override; 
};

#endif