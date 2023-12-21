#include <iostream>
#include "scsp_queue.h"
#include "interface_queue.h"
#include "test_1.h"
#include "test_speed.h"
#include "test_2.h"

int main(int argc, char* argv[]) {

    std::cout << "\n\nNOTES:\n1: TEST COMPLETED, 0: TEST FAILED\n\n";

    i_queue* Q = new SCSP;
    Q->print_items();
    std::cout << "SCSP PRODUCER SOLO TEST: " << SCSP_PRODUCE_TEST(Q) << "\n";
    Q->print_items();
    std::cout << "SCSP CONSUMER SOLO TEST: " << SCSP_COMSUME_TEST(Q) << "\n";
    Q->print_items();
    std::cout << "SCSP GENERAL TEST: " << SCSP_GENERAL_TEST(Q) << "\n";
    Q->print_items();
    SCSP_SPEED_TEST();

    std::cout << "\n";

    i_queue* Q_m = new MCSP;
    Q_m->print_items();
    std::cout << "MCSP PRODUCER SOLO TEST: " << MCSP_PRODUCE_TEST(Q_m) << "\n";
    Q_m->print_items();
    std::cout << "MCSP CONSUMER-S (MANY THREADS) SOLO TEST: " << MCSP_COMSUME_TEST(Q_m) << "\n";
    Q_m->print_items();
    std::cout << "MCSP GENERAL TEST: " << MCSP_GENERAL_TEST(Q_m) << "\n";
    Q_m->print_items();
    MCSP_SPEED_TEST();


    delete Q;
    delete Q_m;
    
    return 0;
}