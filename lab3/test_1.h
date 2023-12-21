#ifndef TEST_1_H
#define TEST_1_H

#include <pthread.h>
#include "scsp_queue.h"

// модуль с тестеми для SCSP

void* tpf(void* q) {
    i_queue* q_formated = static_cast<i_queue*>(q);
    for(int i = 0; i < ITEMS; ++i) {
        q_formated->enqueue(i);
    }
    return nullptr;
}

bool SCSP_PRODUCE_TEST(i_queue* q) {
    pthread_t my_thread;
    const int T_NUMBER = ITEMS;

    int result = pthread_create(&my_thread, NULL, tpf, q);

    if (result) {
        std::cerr << "Error creating thread " << result << std::endl;
        return -1;
    }

    pthread_join(my_thread, NULL);

    int check_array[T_NUMBER];
    q->get_items(check_array);

    int sum = 0;
    for (auto& item: check_array) {
        sum += item;
    }

    if (sum == (T_NUMBER - 1) * T_NUMBER / 2) {
        return true;
    }
    return false;
}


struct ThreadData {  // создаем такую структуру, т.к. pthread принимает только void*
    int number[ITEMS] = {0};
    i_queue* q;
};

void* tcf(void* thread_data) {
    ThreadData* td_formated = static_cast<ThreadData*>(thread_data);
    
    for(int i = 0; i < ITEMS; i++) {
        td_formated->q->dequeue(td_formated->number[i]);
    }
    return nullptr;
}

int SCSP_COMSUME_TEST(i_queue* q) {

    // создаем 1 поток и считываем из очереди 10 раз

    ThreadData td;
    pthread_t my_thread;
    const int T_NUMBER = ITEMS;

    td.q = q;
    
    int result = pthread_create(&my_thread, NULL, tcf, &td);
    if (result) {
        std::cerr << "Error creating thread " << result << std::endl;
        return -1;
    }

    pthread_join(my_thread, NULL);
    

    int sum = 0;
    for (auto& item: td.number) {
        sum += item;
    }

    if (sum == (T_NUMBER - 1) * T_NUMBER / 2) {
        return true;
    }
    return false;
}

void* dual_tcf(void* thread_data) {
    ThreadData* td_formated = static_cast<ThreadData*>(thread_data);
    
    for(int i = 0; i < ITEMS; i++) {
        if (!td_formated->q->dequeue(td_formated->number[i])) {
            i--;
        }
        else {
            td_formated->number[i] = 1;
        }
    }
    return nullptr;
}


bool SCSP_GENERAL_TEST(i_queue* q) {
    pthread_t p_thread;
    pthread_t c_thread;
    const int T_NUMBER = ITEMS;
    ThreadData td;
    td.q = q;

    int p_result = pthread_create(&p_thread, NULL, tpf, q);
    int c_result = pthread_create(&c_thread, NULL, dual_tcf, &td);

    pthread_join(p_thread, NULL);
    pthread_join(c_thread, NULL);
    
    for (auto& item: td.number) {
        if (item == 0) {
            return false;
        }
    }
    return true;
}


#endif 