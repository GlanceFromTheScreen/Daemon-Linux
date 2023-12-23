#ifndef MCSP_QUEUE_H
#define MCSP_QUEUE_H

#include "interface_queue.h"

class MCSP: public i_queue {

public:
    MCSP ();
    ~MCSP();
    void enqueue(int item) override;
    bool dequeue(int& item) override;
    bool empty() override; 
};

#endif