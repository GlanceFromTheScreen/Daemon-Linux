#ifndef TEST_2_H
#define TEST_2_H

#include <pthread.h>
#include "test_1.h"
#include "mcsp_queue.h"

// модуль с тестеми для MCSP


bool MCSP_PRODUCE_TEST(i_queue* q) {
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

struct ThreadDataMCSP {  // создаем такую структуру, т.к. pthread принимает только void*
    int number;
    i_queue* q;
};


void* tcf_2(void* thread_data) {
    ThreadDataMCSP* td_formated = static_cast<ThreadDataMCSP*>(thread_data);
    td_formated->q->dequeue(td_formated->number);

    return nullptr;
}

int MCSP_COMSUME_TEST(i_queue* q) {

    // создаем 10 потоков и в каждом считываем из очереди

    ThreadDataMCSP td[ITEMS];
    pthread_t my_thread[ITEMS];
    const int T_NUMBER = ITEMS;

    for (int i = 0; i < ITEMS; i++) {
        td[i].q = q;
        int result = pthread_create(&my_thread[i], NULL, tcf_2, &td[i]);
        if (result) {
            std::cerr << "Error creating thread " << result << std::endl;
            return -1;
        }
    }

    for(int i = 0; i < ITEMS; i++) {
        pthread_join(my_thread[i], NULL);
    }
    
    int sum = 0;
    for (auto& item: td) {
        // std::cout << item.number << "\n";
        sum += item.number;
    }

    if (sum == (T_NUMBER - 1) * T_NUMBER / 2) {
        return true;
    }
    return false;
}

void* dual_tcf_2(void* thread_data) {
    ThreadDataMCSP* td_formated = static_cast<ThreadDataMCSP*>(thread_data);
    
    while(!td_formated->q->dequeue(td_formated->number)) {}
    td_formated->number = 1;
    
    return nullptr;
}


bool MCSP_GENERAL_TEST(i_queue* q) {
    pthread_t p_thread;
    pthread_t c_thread[ITEMS];
    const int T_NUMBER = ITEMS;
    ThreadDataMCSP td[ITEMS];

    int p_result = pthread_create(&p_thread, NULL, tpf, q);

    for (int i = 0; i < ITEMS; i++) {
        td[i].q = q;
        int result = pthread_create(&c_thread[i], NULL, dual_tcf_2, &td[i]);
        if (result) {
            std::cerr << "Error creating thread " << result << std::endl;
            return -1;
        }
    }

    pthread_join(p_thread, NULL);
    for(int i = 0; i < ITEMS; i++) {
        pthread_join(c_thread[i], NULL);
    }
    
    for (auto& item: td) {
        if (item.number == 0) {
            return false;
        }
    }

    return true;
}


#endif 