#ifndef TEST_SPEED_H
#define TEST_SPEED_H

#include "test_1.h"
#include "test_2.h"
#include "scsp_queue.h"
#include <chrono>

// модуль с тестеми скорости для SCSP и MCSP

void SCSP_SPEED_TEST() {

    std::cout << "SCSP_PRODUCE_SPEED_TEST:\n";

    i_queue* q = new SCSP;
    const int N = 100;
    long int p_arr[N];
    long int c_arr[N];

    for (int i = 0; i < N; i++) {
        auto start_time_p = std::chrono::high_resolution_clock::now();
        SCSP_PRODUCE_TEST(q);  
        auto end_time_p = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time_p - start_time_p).count();
        p_arr[i] = duration;

        auto start_time_c = std::chrono::high_resolution_clock::now();
        SCSP_COMSUME_TEST(q);
        auto end_time_c = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time_c - start_time_c).count();
        c_arr[i] = duration;
    }

    float p_sum = 0;
    float c_sum = 0;

    for(int i = 0; i < N; i++) {
        p_sum += p_arr[i];
        c_sum += c_arr[i];
    }

    std::cout << "AVG PRODUCE time taken: " << p_sum / N / ITEMS << " microseconds for operation" << std::endl;
    std::cout << "AVG CONSUME time taken: " << c_sum / N / ITEMS << " microseconds for operation" << std::endl;
   
    delete q;
}


void MCSP_SPEED_TEST() {

    std::cout << "MCSP_PRODUCE_SPEED_TEST:\n";

    i_queue* q = new MCSP;
    const int N = 100;
    long int p_arr[N];
    long int c_arr[N];

    for (int i = 0; i < N; i++) {
        auto start_time_p = std::chrono::high_resolution_clock::now();
        MCSP_PRODUCE_TEST(q);  
        auto end_time_p = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time_p - start_time_p).count();
        p_arr[i] = duration;

        auto start_time_c = std::chrono::high_resolution_clock::now();
        MCSP_COMSUME_TEST(q);
        auto end_time_c = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time_c - start_time_c).count();
        c_arr[i] = duration;
    }

    float p_sum = 0;
    float c_sum = 0;

    for(int i = 0; i < N; i++) {
        p_sum += p_arr[i];
        c_sum += c_arr[i];
    }

    std::cout << "AVG PRODUCE time taken: " << p_sum / N / ITEMS << " microseconds for operation" << std::endl;
    std::cout << "AVG CONSUME time taken: " << c_sum / N / ITEMS << " microseconds for operation" << std::endl;
   

    delete q;
}


#endif 